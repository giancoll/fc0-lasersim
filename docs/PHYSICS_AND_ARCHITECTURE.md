# Physics and Software Architecture

## Scope

`fc0-lasersim` is a C++17 TPC simulation for comparing charged-particle and
laser-induced tracks in the FC0/HAT geometry. Garfield++ supplies gas transport,
Heed ionisation, and electron drift. The laser source, ERAM pad mapping,
resistive-layer charge spread, electronics response, digitisation, ROOT output,
and analysis are custom code.

The main cascade is implemented in `src/main.cc`:

```text
JSON configuration
  -> gas and fields
  -> primary source (Heed track or laser MPI)
  -> Garfield electron drift
  -> HAT pad assignment
  -> parameterised avalanche gain
  -> resistive-layer charge spread
  -> electronics impulse response and ADC sampling
  -> ROOT trees
  -> plotting and comparison scripts
```

Geant4 is linked because the bundled `external/oaEvent` data model contains
Geant4 truth objects. It is not currently used to transport the muon, laser,
electrons, or avalanche in the main ROOT-output simulation path.

## 1. Configuration

`Config::Load` in `src/config/Config.cc` parses JSON and supports one base file
through `extends`. Values are stored in `src/config/Config.hh`. Spatial
configuration values are in millimetres; the Garfield interfaces are converted
to centimetres at call boundaries.

Important run controls are `n_events`, `random_seed`, `drift_method`,
`field_type`, `output_level`, `output_format`, and `output_file`.

## 2. Gas and Fields

`GasModel::Initialize` creates a `Garfield::MediumMagboltz`. If `gas_file`
exists, `LoadFromFile` loads its transport table. Otherwise
`ComputeWithMagboltz` sets composition, pressure, temperature, an electric and
magnetic field grid, calls `GenerateGasTable`, and optionally writes the table.

Relevant parameters are gas species and fractions, `pressure_atm`,
`temperature_K`, and the gas-table path. These determine drift velocity,
longitudinal/transverse diffusion, Townsend and attachment coefficients used
by Garfield transport algorithms.

`FieldManager::BuildGeometry` creates a Garfield gas volume and sensor area.
`SetupUserField` installs callbacks in `Garfield::ComponentUser`;
`SetupConstantField` uses `Garfield::ComponentConstant`. The present field
strengths are hard-coded in `FieldManager.cc`, so they are not yet independently
configurable in JSON.

Check Garfield drift velocity and diffusion against Magboltz tables and
laser/cosmic drift-time data at every operating pressure, temperature, electric
field, magnetic field, and gas composition.

## 3. Charged-Particle Primary Ionisation

`TrackGenerator::Initialize` constructs `Garfield::TrackHeed`, attaches the
sensor, selects the particle and momentum, and enables electric field, magnetic
field, and delta-electron transport. `TrackGenerator::Generate` calls
`TrackHeed::NewTrack`, iterates `GetCluster`, then reads every primary electron
with `GetElectron`.

Relevant parameters are particle species, momentum, start `(x,y,z)`, direction,
gas properties, fields, and Heed stepping limits. The output is one
`PrimaryElectron` per Heed electron with position, time, and kinetic energy.

Validation should compare cluster density, primary-electron density, total
energy loss, cluster-size distribution, and delta-electron tails against Heed
reference examples and minimum-ionising-particle data. A Landau-like fit is
appropriate for finite-segment charged-particle energy-loss or charge-density
distributions, subject to detector acceptance and threshold effects.

## 4. Laser Optics and MPI Ionisation

`LaserGenerator::Generate` follows a straight ray to the TPC boundary or uses
`RayMirrorIntersect`, `MirrorNormal`, and `Reflect` for one specular reflection.
`RayTpcExit` clips segments to the configured TPC box.

For `laser.model="mpi"`, `BeamRadiiMm` implements an astigmatic Gaussian M2
beam. For transverse axis `i`:

```text
z_R,i = pi w_0,i^2 / (M2_i lambda)
w_i(s) = w_0,i sqrt(1 + ((s-s_waist)/z_R,i)^2)
```

`w_0` and `w(s)` are 1/e2 intensity radii. `PeakIntensityWPerCm2` normalises a
Gaussian temporal pulse and Gaussian transverse profile to pulse energy `E`:

```text
I0(waist) = 2 E / (pi w_0,x w_0,y tau sqrt(pi/(4 ln 2)))
I0(s) = I0(waist) (w_0,x/w_x(s)) (w_0,y/w_y(s))
```

`GenerateMpiSegment` divides the ray into `propagation_step_mm` intervals. For
an ionisation channel of photon order `N`, neutral density `n`, and effective
coefficient `sigma_N`, its expected electrons in a step are:

```text
mu = n sigma_N I0(s)^N
     tau sqrt(pi/(4 N ln 2))
     [pi w_x(s) w_y(s)/(2 N)] ds
```

The code draws `Poisson(mu)`. Electron transverse coordinates are Gaussian
with standard deviations `w_x/(2 sqrt(N))` and `w_y/(2 sqrt(N))`; time is
Gaussian with `tau/(2 sqrt(2 N ln 2))`. `ChannelNumberDensityCm3` obtains the
neutral density from an explicit value, a species fraction, or a named gas
component and the ideal-gas law.

Relevant controls are wavelength, pulse energy, temporal FWHM, waist radii and
position, both M2 values, propagation step, pointing jitter, channel order,
species density/fraction, coefficient, saturation flag, and per-step cap.

The generalized MPI coefficient is effective and must be calibrated. Validate
the optics separately with beam-profiler measurements of waist, M2, pointing,
energy, pulse width, and longitudinal intensity. Then fit ionisation using
charge versus pulse energy: an unsaturated N-photon channel should approach an
`E^N` law locally. Validate longitudinal and radial primary profiles before
using anode charge, because drift and acceptance otherwise obscure source
errors.

## 5. Electron Drift

`DriftEngine::Initialize` selects:

| `drift_method` | Garfield class | Intended use |
|---|---|---|
| `RKF` | `DriftLineRKF` | fast mean drift line |
| `MC` | `AvalancheMC` | macroscopic Monte Carlo with diffusion |
| `Simple` | `AvalancheMicroscopic` | microscopic electron tracking |

`DriftEngine::DriftElectron` converts mm to cm, invokes the selected Garfield
method, extracts endpoints, and converts back to mm. For density and spatial
resolution comparisons, `MC` is the current production choice.

Relevant inputs are the gas transport table, E/B fields, sensor volume,
initial electron position/time, drift method, and the MC distance step (fixed
at 0.1 cm in code). Validate drift time versus distance, Lorentz angle,
transverse and longitudinal diffusion versus drift distance, survival, and
boundary status. Compare to straight laser tracks at several x positions and
to dedicated field-distortion maps.

## 6. Endpoint Selection and Pad Mapping

The event loop records in-bounds endpoint `(y,z,t,status)` values. It maps each
endpoint with `HATGeometry::GetPadIndices`; quartz-window or inactive positions
receive pad indices `-1`. `HATGeometry` currently compiles eight ERAM positions
and 36 by 32 pads per ERAM with 11.16 by 10.05 mm active dimensions and 0.12 mm
gaps.

The ROOT `anode` tree is the correct source for drifted endpoint density. Apply
valid `anode_eram`, `anode_padZ`, and `anode_padY` selection when asking for
charge reaching instrumented pads.

Check endpoint status and exit-surface histograms, generated versus accepted
counts, ERAM boundaries over endpoint maps, and known pad centres and edges.
Acceptance must be identical for muon and laser comparisons.

## 7. Avalanche Gain

`WaveformBuilder::CollectLocalHits` converts accepted endpoints to ERAM-local
coordinates and calls `SampleAvalancheGain` for each electron. The gain is not
a Garfield Micromegas avalanche. It is either constant (`avalanche_gain`) or a
sample from a discretised Polya distribution built by `BuildPolyaGainTable`.

Polya parameters are mean gain, shape parameter, maximum sampled gain ratio,
table bins, and random seed. Validate with single-electron gain spectra and
55Fe or other calibrated charge data. A future microscopic amplification model
should include the actual mesh field and geometry, attachment, ion feedback,
and transparency.

## 8. Resistive-Layer Charge Spread

`ClusterSubPads` groups local hits into a configurable subpad grid and merges
nearby times. `WaveformBuilder::Build` then evaluates the analytic 2D diffusion
of charge on a resistive sheet, integrated over rectangular pads. In the code,
`k = 1/RC` and the kernel contains Gaussian exponentials and error functions at
the pad boundaries. Charge is evaluated only over `spread_pads_x/y` neighbours.

Relevant parameters are `subpads_x/y`, `subpad_merge_time_ns`,
`resistive_rc_ns_per_mm2`, `spread_pads_x/y`, internal time step, and number of
internal samples. Validate charge conservation over an expanding pad window,
width versus time, sharing fractions versus impact position, and RC using
focused charge-injection or laser data.

## 9. Electronics and ADC

`BuildResponseKernel` creates the current analytic electronics impulse response
from `response_time_ns`. `Build` convolves each pad current with this response,
samples at `1000/sampling_frequency_MHz`, clips to the ADC bit range, applies
`threshold_adc`, and writes only active pads.

With `normalize_waveforms=true`, every event is rescaled so its largest sample
reaches ADC full scale. This is useful only for displays and destroys absolute
event-to-event charge information. Quantitative studies should set it false
and calibrate `electronics.gain` in physical units using injected charge.

The parameters `shaping_time_ns` and `dynamic_range_mV` are parsed but are not
currently used by the response/ADC calculation. There is no explicit electron
to coulomb, preamplifier gain, mV, pedestal, noise, common mode, saturation, or
zero-suppression calibration chain.

Validation should use impulse/pulser waveforms, measured peaking time and pulse
shape, pedestal/noise distributions, ADC transfer curves, dynamic range,
threshold efficiency, and charge linearity.

## 10. Output, Readout, and Reconstruction

`ROOTOutput` writes:

| Tree | Main branches |
|---|---|
| `clusters` | `cl_x/y/z/t/ekin`, source primary count |
| `anode` | `anode_y/z/t/status`, ERAM and pad indices |
| `waveforms` | pad IDs, peak, sparse offsets/lengths, flattened ADC samples |

`analysis/event_display.py` reads these levels. The other scripts calculate
quick-look distributions, average maps, primary profiles, and drifted density.
`Analisi_Elena` contains legacy ROOT analyses adapted to the current geometry.

There is no complete waveform readout emulation or track-reconstruction
algorithm in this repository. The event display can overlay externally
reconstructed `(event, track,z_mm,y_mm)` points, but it does not fit tracks.
Any thesis statement about reconstruction resolution must identify the external
reconstruction and its calibration, selection, and uncertainty treatment.

## Known Risks and Required Improvements

1. Garfield endpoint status handling is unresolved. Existing files can report
   status `-5` while endpoints geometrically lie on the readout side; the code
   counts only status zero in `nAnodeHits`, yet waveform building accepts valid
   pad coordinates regardless of status. Store final x and classify the actual
   exit surface before physics use.
2. `ComponentUser` and `ComponentConstant` use field signs that should be
   reconciled and covered by a drift-direction test.
3. JSON pad dimensions are parsed but HAT mapping uses compiled constants.
   Consolidate geometry into one validated source.
4. Event-level waveform normalisation prevents absolute charge comparison.
5. Avalanche gain and electronics are parameterisations, not end-to-end
   detector calibrations.
6. Pointing jitter is currently sampled for every electron, broadening a track;
   physical pulse-to-pulse pointing jitter should be sampled once per event.
7. Resistive charge outside the configured neighbour window is discarded;
   quantify and bound the lost fraction.
8. The run seed controls the custom ROOT RNG and gain RNG, but Garfield random
   state is not explicitly seeded in the application.
9. `clusters.nElectrons` is assigned before the event loop removes primaries in
   front of ERAM 3, so the scalar can disagree with vector lengths.
10. The effective MPI model neglects depletion, competing channels, impurities
    with uncertain concentrations, pulse-shape deviations, optical losses, and
    space-charge effects.

## Validation Order

Validate one boundary at a time: gas tables, fields, charged-particle source,
laser optics, laser primary source, drift endpoints, pad acceptance, gain,
resistive sharing, electronics, readout, and only then reconstruction. At each
boundary compare distributions and conserved quantities, retain a fixed
reference sample, and scan one parameter at a time before a multi-parameter fit.
