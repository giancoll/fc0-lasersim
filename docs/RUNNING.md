# Build, Run, and Analyse

## 1. Clone

Once access has been granted, clone the teaching repository and enter it:

```bash
git clone https://github.com/giancoll/fc0-lasersim.git
cd fc0-lasersim
```

Students should make changes on their own branch:

```bash
git switch -c student/<name>-<topic>
```

## 2. Configure and Build

Set up the dependencies as described in `docs/INSTALL.md`, then build:

```bash
source scripts/setup_env.sh

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DFC0_ROOT_PREFIX="$ROOT_PREFIX" \
  -DFC0_GARFIELD_PREFIX="$GARFIELD_INSTALL" \
  -DFC0_GEANT4_PREFIX="$GEANT4_INSTALL"
cmake --build build -j"$(nproc)"
```

Expected executables:

```text
build/tpcmc
build/viz_hat_geometry
```

Check runtime linkage before the first run:

```bash
ldd build/tpcmc | grep 'not found' && echo 'missing libraries' || echo 'libraries resolved'
```

## 3. Configuration Model

The executable accepts one JSON configuration:

```bash
./build/tpcmc config/simulation.json
```

Configurations may use `"extends"` to load a base file. Relative paths are
resolved from the directory containing each configuration file. The principal
sections are:

| Section | Purpose |
|---|---|
| `run` | event count, seed, drift/field method, output file and level |
| `gas` | gas mixture, pressure, temperature, Magboltz table |
| `detector` | TPC bounds and legacy configurable pad dimensions |
| `generator.track` | Heed particle, momentum, start and direction |
| `generator.laser` | ray, Gaussian optics, MPI channels, optional mirror |
| `electronics` | avalanche gain, resistive spread, shaping, ADC sampling |

Output levels are cumulative:

| Level | ROOT trees |
|---:|---|
| 1 | `clusters`, primary electrons before drift |
| 2 | `clusters` and `anode`, Garfield drift endpoints and pad assignment |
| 3 | `clusters`, `anode`, and `waveforms`, digitised pad signals |

The HAT pad mapping used by output and waveform code is currently compiled in
`src/detector/HATGeometry.hh`: eight ERAMs, each with 36 by 32 pads of
11.16 mm by 10.05 mm and 0.12 mm gaps. The pad dimensions in the JSON detector
section do not currently replace this compiled geometry.

## 4. Smoke Tests

The one-event laser test avoids Heed and exercises source generation, drift,
pad assignment, and ROOT output:

```bash
./build/tpcmc config/smoke_laser.json
rootls -t smoke_laser.root
```

For a charged track, first verify Heed and run its one-event equivalent:

```bash
test -d "$HEED_DATABASE"
./build/tpcmc config/smoke_muon.json
rootls -t smoke_muon.root
```

The supplied shared horizontal geometry is:

```text
x = 187.0 mm, y = -170.5 mm, z_start = -820.0 mm, direction = +z
```

## 5. Produce Muon and Laser Samples

The 100-event comparison samples use the same trajectory and active ERAM row:

```bash
source scripts/setup_env.sh
./build/tpcmc config/muon_horizontal_100_anode.json
./build/tpcmc config/laser_2photon_horizontal_zR50cm_100_anode.json
```

The laser configuration uses 266 nm, 1 mJ, 5 ns, a 0.205755 mm waist radius,
`M2=1`, and a waist position chosen for a Rayleigh length near 50 cm. Its
two-photon coefficient is an effective calibration parameter, not a tabulated
first-principles cross section.

The corresponding 1000-event files are:

```bash
./build/tpcmc config/muon_horizontal_1000_anode.json
./build/tpcmc config/laser_2photon_horizontal_zR50cm_1000_anode_5mJ.json
```

Always change `run.output_file` and `run.random_seed` for independent samples.
Do not compare samples produced with identical output names or accidental seed
reuse.

## 6. Inspect ROOT Output

```bash
rootls -t output_muon_horizontal_100_anode.root
python3 analysis/analyze_output.py output_muon_horizontal_100_anode.root \
  -o analysis_plots/muon_quicklook
```

Interactive or static event display:

```bash
python3 analysis/event_display.py output_muon_horizontal_100_anode.root
python3 analysis/event_display.py output_laser_2photon_horizontal_zR50cm_100_anode.root \
  --event 0 --save analysis_plots/laser_event_000.png
```

Waveform plots require a file generated with `run.output_level: 3`.

## 7. Comparison Analysis

Average pad maps:

```bash
python3 analysis/compare_average_pad_maps.py \
  --muon output_muon_horizontal_100_anode.root \
  --laser output_laser_2photon_horizontal_zR50cm_100_anode.root \
  --out analysis_plots/muon_vs_laser_avg_pad_maps.png
```

Drifted endpoint density per centimetre, with a Landau fit for muons and a
Gaussian fit for the laser in a selected longitudinal interval:

```bash
python3 analysis/track_unit_density.py \
  --muon output_muon_horizontal_100_anode.root \
  --laser output_laser_2photon_horizontal_zR50cm_100_anode.root \
  --out-prefix analysis_plots/track_unit_density \
  --bin-width-cm 1 \
  --muon-z-min -250 --muon-z-max 250 \
  --laser-z-min -250 --laser-z-max 250 \
  --muon-fit-type landau --laser-fit-type gaussian
```

Source-level longitudinal and transverse profiles:

```bash
python3 analysis/source_track_profiles.py \
  --muon output_muon_horizontal_100_anode.root \
  --laser output_laser_2photon_horizontal_zR50cm_100_anode.root \
  --config config/laser_2photon_horizontal_zR50cm_100_anode.json \
  --out-prefix analysis_plots/source_track_profiles
```

Laser source/drift summary:

```bash
python3 analysis/laser_track_metrics.py \
  output_laser_2photon_horizontal_zR50cm_100_anode.root \
  --config config/laser_2photon_horizontal_zR50cm_100_anode.json
```

The Gaussian laser-density interpretation is valid only over a z interval in
which the optical intensity and acceptance are approximately stationary. Over
the full Rayleigh profile, the event density is generally a mixture of local
Poisson distributions and need not be Gaussian.

## 8. Reproducibility Record

For every production sample retain:

```bash
git rev-parse HEAD
root-config --version
geant4-config --version
git -C "$GARFIELD_HOME" describe --tags --always
cp config/the_used_config.json results/run_name.json
```

Also record the host/compiler, environment, random seed, output level, and the
exact analysis command. ROOT outputs and plots are intentionally excluded from
Git; store large production data in the collaboration data area.
