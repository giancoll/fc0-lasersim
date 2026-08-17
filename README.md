# FC0 Laser Simulation

`fc0-lasersim` simulates charged-particle and laser-induced ionisation in the
FC0/HAT TPC. It combines Garfield++/Heed/Magboltz transport with a custom
Gaussian-beam multiphoton-ionisation source, HAT pad geometry, resistive charge
spreading, electronics response, ROOT output, and comparison analyses.

## Documentation

- [Installation](docs/INSTALL.md): ROOT, Geant4, Garfield++, and dependencies.
- [Build, run, and analysis](docs/RUNNING.md): configurations, production jobs,
  event displays, and muon/laser comparisons.
- [Physics and architecture](docs/PHYSICS_AND_ARCHITECTURE.md): complete cascade,
  functions, parameters, validation strategy, and known limitations.

The verified baseline is AlmaLinux 9, ROOT 6.38.00, Garfield++ 2025.12, and
Geant4 11.4.1.

## Quick Start

After installing the dependencies:

```bash
export FC0_DEPS="$HOME/opt/fc0"
source scripts/setup_env.sh

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DFC0_ROOT_PREFIX="$ROOT_PREFIX" \
  -DFC0_GARFIELD_PREFIX="$GARFIELD_INSTALL" \
  -DFC0_GEANT4_PREFIX="$GEANT4_INSTALL"
cmake --build build -j"$(nproc)"

./build/tpcmc config/laser_mpi_example.json
```

The build produces `build/tpcmc` and `build/viz_hat_geometry`. Muon jobs also
require `HEED_DATABASE`; `scripts/setup_env.sh` sets it from the Garfield++
installation.

## Distribution

This tree includes a bundled `external/oaEvent` snapshot without a repository
license notice. Keep the teaching repository private until the ownership and
redistribution terms of that dependency and the project as a whole are
confirmed.

For a first waveform-display test, edit `config/simulation.json` before running:

```json
"n_events": 1,
"output_level": 3
```

`output_level` is cumulative:

```text
1  clusters
2  clusters + anode drift endpoints
3  clusters + anode drift endpoints + pad waveforms
```

The current local config may be set to `output_level: 2`, which is enough for
electron projections but does not write waveforms. Use `output_level: 3` when
you want to inspect pad waveforms in the event display.

You can check whether a ROOT file contains waveforms with:

```bash
rootls -t output.root
```

If the listing only shows:

```text
clusters
anode
```

then the file was produced without waveform output. Re-run the simulation with
`output_level: 3` and then reopen the new `output.root` in the event display.
The listing must include:

```text
waveforms
```

for pad waveform display to be available.

## Waveform Gain Model

Pad waveforms use a configurable per-electron avalanche gain. The default is a
constant gain:

```json
"electronics": {
  "gain_model": "constant",
  "avalanche_gain": 1000.0,
  "normalize_waveforms": true
}
```

The old TPCMC `MC_ResMM::eGain()` Polya-style gain can be enabled with:

```json
"electronics": {
  "gain_model": "polya",
  "polya_gain": {
    "mean_gain": 1000.0,
    "polya_parameter": 2.3,
    "max_gain_ratio": 5.0,
    "n_bins": 1000,
    "random_seed": -1
  },
  "normalize_waveforms": true
}
```

`random_seed: -1` derives an independent gain RNG seed from `run.random_seed`.
The older `electronics.gain` field is still the ADC conversion factor used only
when `normalize_waveforms` is false.

## Laser MPI Model

The laser generator supports two source models:

```json
"generator": {
  "type": "laser",
  "laser": {
    "model": "ideal"
  }
}
```

`ideal` keeps the historical flat test source configured by
`laser.ideal.cluster_density`.

For the physics model described in
`laser_generated_tracks_tpc_report_v1.docx`, use:

```json
"generator": {
  "type": "laser",
  "laser": {
    "model": "mpi",
    "optics": {
      "wavelength_nm": 266.0,
      "pulse_energy_mJ": 1.0,
      "pulse_duration_ns": 5.0,
      "waist_x_mm": 0.5,
      "waist_y_mm": 0.5,
      "waist_s_mm": 450.0,
      "m2_x": 1.2,
      "m2_y": 1.2,
      "propagation_step_mm": 1.0,
      "pointing_jitter_mm": 0.0
    },
    "ionization": {
      "channels": [
        {
          "name": "isobutane_3photon_effective",
          "gas_component": "iC4H10",
          "photon_order": 3,
          "coefficient": 1.0e-30
        }
      ]
    }
  }
}
```

The optics model is a real Gaussian beam with separate `M2` propagation in the
two transverse directions. The ionization model uses the low-depletion
Gaussian-pulse MPI source term:

```text
n_e,N(r,s) = n_gas sigma_N I_spatial_peak(r,s)^N tau_FWHM sqrt(pi/(4 N ln 2))
```

The generalized MPI `coefficient` is intentionally an effective fit parameter.
Use measured beam parameters and tune coefficients against charge, width, and
energy-scan data. A runnable starting point is available in:

```bash
./build-almalinux9-gpt/tpcmc config/laser_mpi_example.json
```

Shared geometry templates for the horizontal active-row studies live in:

```bash
config/horizontal_active_row_shared.json
```

The muon and laser 100-event configs extend this base file and only override
the run label, seed, and generator-specific fields.

## Event Display

After producing a ROOT file, an interactive event display is available:

```bash
source /opt/software/root-6.38.00/build/bin/thisroot.sh
python3 analysis/event_display.py output.root
```

It shows the event-by-event Y-Z pad-plane projection, active waveform pads, and
the waveform for the pad under the cursor or selected by click. Waveforms require
ROOT output level 3. Files without a `waveforms` tree still display primary and
anode electron projections.

For a non-interactive PNG snapshot:

```bash
python3 analysis/event_display.py output.root --event 0 --save analysis_plots/event_000.png
```

Interactive controls:

```text
mouse over active pad  show that pad waveform
click pad              select that pad waveform
right / n              next event
left / p               previous event
```

Optional reconstructed tracks can be overlaid from a CSV file with columns:

```text
event,track,z_mm,y_mm
```

```bash
python3 analysis/event_display.py output.root --reco-csv reconstructed_tracks.csv
```

Useful CMake options:

```bash
-DFC0_ROOT_PREFIX=/opt/software/root-6.38.00/build
-DFC0_GARFIELD_PREFIX=/local/simulazioni/garfieldpp/install
-DFC0_GEANT4_PREFIX=/local/simulazioni/geant4/geant4-v11.4.1-install
-DFC0_WITH_GEANT4=ON
-DDEBUG_MODE=ON
```
