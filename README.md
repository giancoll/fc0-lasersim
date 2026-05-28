# FC0-laserSim



## Requirements

Garfield++: https://gitlab.cern.ch/garfield/garfieldpp
Tags: 2025.12

ROOT

This AlmaLinux 9 WSL2 setup was verified with:

```text
ROOT:      /opt/software/root-6.38.00/build
Garfield++ /local/simulazioni/garfieldpp/install
Geant4:    /local/simulazioni/geant4/geant4-v11.4.1-install
```

`nlohmann_json` is expected to be available as a local CMake package. On this
machine it is installed under `/usr/local`, so CMake can configure without
downloading from GitHub.

## Compilation

Activate the runtime environments:

```bash
source /opt/software/root-6.38.00/build/bin/thisroot.sh
source /local/simulazioni/geant4/geant4-v11.4.1-install/bin/geant4.sh
source /local/simulazioni/garfieldpp/install/share/Garfield/setupGarfield.sh
```

Configure and build:

```bash
cmake -S /local/simulazioni/fc0-lasersim \
  -B /local/simulazioni/fc0-lasersim/build-almalinux9-gpt \
  -DCMAKE_PREFIX_PATH="/opt/software/root-6.38.00/build;/local/simulazioni/garfieldpp/install;/local/simulazioni/geant4/geant4-v11.4.1-install;/usr/local"

cmake --build /local/simulazioni/fc0-lasersim/build-almalinux9-gpt -j"$(nproc)"
```

The build produces:

```text
build-almalinux9-gpt/tpcmc
build-almalinux9-gpt/viz_hat_geometry
```

Run the main simulation with:

```bash
cd /local/simulazioni/fc0-lasersim
./build-almalinux9-gpt/tpcmc config/simulation.json
```

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
