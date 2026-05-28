# Note about Laser Simulation in FC0 TPC

Date: 2026-05-07

This note documents the first AlmaLinux 9.7 WSL2 build pass for:

```text
/local/simulazioni/fc0-lasersim
```

The package simulates track generation and detection in a Time Projection
Chamber. From the source inspection, it uses Garfield++ for gas properties,
primary ionisation, electron drift/transport, and detailed avalanche/drift
tools. It also contains a custom fast readout/output chain and a bundled
`oaEvent` copy for ND280-style ROOT event objects.

## Installed Software Used

The build was patched and verified against the locally installed software:

```text
ROOT:      /opt/software/root-6.38.00/build
Garfield++ /local/simulazioni/garfieldpp/install
Geant4:    /local/simulazioni/geant4/geant4-v11.4.1-install
nlohmann:  /usr/local
```

Important runtime setup:

```bash
source /opt/software/root-6.38.00/build/bin/thisroot.sh
source /local/simulazioni/geant4/geant4-v11.4.1-install/bin/geant4.sh
source /local/simulazioni/garfieldpp/install/share/Garfield/setupGarfield.sh
```

## Source Structure Observed

Main executable:

```text
tpcmc
```

Main source areas:

```text
src/config/          JSON configuration loader
src/detector/        gas model, field manager, drift engine, HAT geometry
src/generator/       laser and track generators
src/io/              ROOT and ND280/oaEvent output
external/oaEvent/    bundled ND280 oaEvent classes/dictionaries
examples/            HAT geometry visualization example
```

Important Garfield++ usage:

```text
src/detector/GasModel.cc       Garfield::MediumMagboltz
src/detector/FieldManager.cc   Garfield::Sensor, ComponentUser, ComponentConstant
src/detector/DriftEngine.cc    AvalancheMC, AvalancheMicroscopic, DriftLineRKF
src/generator/TrackGenerator.cc Garfield::TrackHeed
```

The code includes Garfield++ headers with the modern installed-header style:

```cpp
#include <Garfield/TrackHeed.hh>
#include <Garfield/Sensor.hh>
#include <Garfield/MediumMagboltz.hh>
```

This matches the installed Garfield++ package at
`/local/simulazioni/garfieldpp/install`.

## CMake Problem Found

The original top-level `CMakeLists.txt` had:

```cmake
find_package(ROOT REQUIRED COMPONENTS Core RIO Tree Hist MathCore Physics Geom)
find_package(GSL  REQUIRED)
find_package(Garfield REQUIRED HINTS $ENV{GARFIELD_HOME}/install)
```

and fetched `nlohmann/json` from GitHub with `FetchContent`.

Problems:

- `$GARFIELD_HOME` was not the reliable way to point to the installed Garfield++
  prefix used on this machine.
- Geant4 was not explicitly found, even though the package contains bundled
  `oaEvent` classes representing G4 truth/hit objects.
- The first configure failed because CMake tried to clone `nlohmann/json` from
  GitHub, but the WSL2 build environment had no network access from CMake.
- A local `nlohmann_json` CMake package already existed under `/usr/local`.

## CMake Patch Applied

The top-level `CMakeLists.txt` was patched to define cacheable local prefixes:

```cmake
set(FC0_ROOT_PREFIX "/opt/software/root-6.38.00/build" CACHE PATH
    "ROOT installation prefix")
set(FC0_GARFIELD_PREFIX "/local/simulazioni/garfieldpp/install" CACHE PATH
    "Garfield++ installation prefix")
set(FC0_GEANT4_PREFIX "/local/simulazioni/geant4/geant4-v11.4.1-install" CACHE PATH
    "Geant4 installation prefix")
option(FC0_WITH_GEANT4 "Find and link the installed Geant4 package" ON)
```

Then these prefixes are prepended to `CMAKE_PREFIX_PATH`:

```cmake
list(PREPEND CMAKE_PREFIX_PATH
    "${FC0_ROOT_PREFIX}"
    "${FC0_GARFIELD_PREFIX}"
    "${FC0_GEANT4_PREFIX}"
    "/usr/local"
)
```

The package lookup was changed to:

```cmake
find_package(ROOT REQUIRED CONFIG
    COMPONENTS Core RIO Tree Hist MathCore Physics Geom Gpad Graf Gui EG)
find_package(GSL REQUIRED)
find_package(Garfield REQUIRED CONFIG)

if(FC0_WITH_GEANT4)
    find_package(Geant4 REQUIRED CONFIG)
    include(${Geant4_USE_FILE})
endif()
```

For `nlohmann_json`, the build now tries the local CMake package first:

```cmake
find_package(nlohmann_json 3.11.3 QUIET CONFIG)
if(NOT nlohmann_json_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()
```

The main executable still links Garfield++ normally:

```cmake
target_link_libraries(tpcmc PRIVATE
    ROOT::Core ROOT::RIO ROOT::Tree ROOT::Hist ROOT::MathCore ROOT::Physics ROOT::Geom
    Garfield::Garfield
    GSL::gsl
    nlohmann_json::nlohmann_json
    oaEvent
)

if(FC0_WITH_GEANT4)
    target_link_libraries(tpcmc PRIVATE ${Geant4_LIBRARIES})
endif()
```

The repository `.gitignore` was also updated to ignore build directories:

```text
build*/
```

## Config Path Patch

The first runtime test failed before the event loop with:

```text
terminate called after throwing an instance of 'Garfield::Exception'
what():  Cannot open file ../gas_files/T2K_ExBxA_6x6x6_293K_760Torr.gas
```

The gas file was not actually missing. It exists here:

```text
/local/simulazioni/fc0-lasersim/gas_files/T2K_ExBxA_6x6x6_293K_760Torr.gas
```

The problem was path resolution. In `config/simulation.json`, the gas file is
specified as:

```json
"gas_file": "../gas_files/T2K_ExBxA_6x6x6_293K_760Torr.gas"
```

That path is correct relative to the config file directory:

```text
/local/simulazioni/fc0-lasersim/config/
```

but the old code interpreted it relative to the process working directory:

```text
/local/simulazioni/fc0-lasersim/
```

I patched `src/config/Config.cc` so `gas_file` and `save_gas_file` are resolved
relative to the JSON config file location:

```cpp
#include <filesystem>

namespace fs = std::filesystem;

const fs::path configDir = fs::absolute(fs::path(path)).parent_path();
const auto resolveConfigPath = [&configDir](const std::string& value) {
    if (value.empty()) return value;
    const fs::path p(value);
    if (p.is_absolute()) return p.lexically_normal().string();
    return (configDir / p).lexically_normal().string();
};
```

and:

```cpp
cfg.gas.gasFile =
    resolveConfigPath(g.value("gas_file", cfg.gas.gasFile));
cfg.gas.saveGasFile =
    resolveConfigPath(g.value("save_gas_file", cfg.gas.saveGasFile));
```

After this patch, the existing config path resolves to:

```text
/local/simulazioni/fc0-lasersim/gas_files/T2K_ExBxA_6x6x6_293K_760Torr.gas
```

## Verified Build Commands

From a fresh shell:

```bash
source /opt/software/root-6.38.00/build/bin/thisroot.sh
source /local/simulazioni/geant4/geant4-v11.4.1-install/bin/geant4.sh
source /local/simulazioni/garfieldpp/install/share/Garfield/setupGarfield.sh
```

Configure:

```bash
cmake -S /local/simulazioni/fc0-lasersim \
  -B /local/simulazioni/fc0-lasersim/build-almalinux9-gpt \
  -DCMAKE_PREFIX_PATH="/opt/software/root-6.38.00/build;/local/simulazioni/garfieldpp/install;/local/simulazioni/geant4/geant4-v11.4.1-install;/usr/local"
```

The configure step reported:

```text
ROOT     : 6.38.00
GSL      : 2.6
Garfield : /local/simulazioni/garfieldpp/install
Geant4   : 11.4.1
nlohmann : 3.11.3
Debug    : OFF
```

Build:

```bash
cmake --build /local/simulazioni/fc0-lasersim/build-almalinux9-gpt -j"$(nproc)"
```

Built targets:

```text
/local/simulazioni/fc0-lasersim/build-almalinux9-gpt/tpcmc
/local/simulazioni/fc0-lasersim/build-almalinux9-gpt/viz_hat_geometry
```

The build completed successfully.

## Runtime Link Check

The main executable was checked with `ldd`.

Important resolved libraries:

```text
libGarfield.so.0.3 => /local/simulazioni/garfieldpp/install/lib64/libGarfield.so.0.3
liboaEvent.so => /local/simulazioni/fc0-lasersim/build-almalinux9-gpt/external/oaEvent/liboaEvent.so
Geant4 libraries => /local/simulazioni/geant4/geant4-v11.4.1-install/lib64
ROOT libraries => /opt/software/root/build/lib
```

No `not found` entries were reported for either:

```text
tpcmc
viz_hat_geometry
```

## Run Command

To run the main simulation:

```bash
cd /local/simulazioni/fc0-lasersim
source /opt/software/root-6.38.00/build/bin/thisroot.sh
source /local/simulazioni/geant4/geant4-v11.4.1-install/bin/geant4.sh
source /local/simulazioni/garfieldpp/install/share/Garfield/setupGarfield.sh

./build-almalinux9-gpt/tpcmc config/simulation.json
```

## One-Event Runtime Test

After the config path patch, the code was rebuilt:

```bash
cmake --build /local/simulazioni/fc0-lasersim/build-almalinux9-gpt -j"$(nproc)"
```

Then the requested one-event run was executed:

```bash
cd /local/simulazioni/fc0-lasersim
source /opt/software/root-6.38.00/build/bin/thisroot.sh
source /local/simulazioni/geant4/geant4-v11.4.1-install/bin/geant4.sh
source /local/simulazioni/garfieldpp/install/share/Garfield/setupGarfield.sh

./build-almalinux9-gpt/tpcmc config/simulation.json
```

The run succeeded. Important output:

```text
[tpcmc] Loading config: config/simulation.json
[tpcmc] Output level: 2
MediumMagboltz::LoadGasFile:
    Reading file /local/simulazioni/fc0-lasersim/gas_files/T2K_ExBxA_6x6x6_293K_760Torr.gas.
    Version 12.
    Gas composition set to CF4/Ar/iC4H10 (3/95/2).
[GasModel] Loaded gas file: /local/simulazioni/fc0-lasersim/gas_files/T2K_ExBxA_6x6x6_293K_760Torr.gas
[GasModel] Initialized: Ar 95% CF4 3% iC4H10 2%
[FieldManager] Sensor area: x=[0,974.6] mm, y=[-370.6,370.6] mm, z=[-890,890] mm
[FieldManager] Initialized with field type: user
[DriftEngine] Initialized with method: MC
[TrackGenerator] Initialized: muon (Heed: mu-) @ 1 GeV/c
[ROOTOutput] Output level 2 (cumulative) -> output.root
[tpcmc] Starting event loop: 1 events
[TrackGenerator] Generated 7223 primary electrons
[tpcmc] Done. Writing output to output.root
[ROOTOutput] File closed.
```

The output file was produced here:

```text
/local/simulazioni/fc0-lasersim/output.root
```

Observed size at the time of this one-event check:

```text
116K
```

At the time of this one-event check, `config/simulation.json` was locally
modified to use:

```text
n_events: 1
output_level: 2
output_format: root
generator.type: track
track particle: muon
```

The config was later changed back to a larger test. On 2026-05-07 the current
`config/simulation.json` used:

```text
n_events: 100
output_file: output.root
output_level: 2
output_format: root
```

The corresponding `output.root` was:

```text
/local/simulazioni/fc0-lasersim/output.root
size: 11819872 bytes
timestamp: 2026-05-07 14:18:38 +0900
```

## Python/PyROOT Analysis Script

A first Python analysis script was added here:

```text
/local/simulazioni/fc0-lasersim/analysis/analyze_output.py
```

The script uses PyROOT for reading the ROOT file and standard Python modules
plus matplotlib for plotting. It avoids the C++ ROOT analysis framework.

Run it from the repository directory:

```bash
cd /local/simulazioni/fc0-lasersim
source /opt/software/root-6.38.00/build/bin/thisroot.sh

python3 analysis/analyze_output.py output.root -o analysis_plots
```

The script reads the two TTrees written by `ROOTOutput`:

```text
clusters:
  eventId
  nElectrons
  cl_x
  cl_y
  cl_z
  cl_t
  cl_ekin

anode:
  eventId
  nPrimaries
  nAnodeHits
  anode_z
  anode_y
  anode_t
  anode_status
  anode_padZ
  anode_padY
```

It prints a compact numerical summary and creates PNG plots in:

```text
/local/simulazioni/fc0-lasersim/analysis_plots
```

Generated plots:

```text
clusters_yz.png
clusters_x_hist.png
anode_hits_yz.png
anode_time_hist.png
pad_occupancy.png
```

The script was tested on the current 100-event `output.root`. The summary was:

```text
=== FC0 laser/TPC ROOT output summary ===

[clusters]
  events:          100
  saved entries:   415570
  branch total:    768051
  electrons/event: mean=7680.51, rms=1627.80
  saved/event:     mean=4155.70, rms=1038.05
  note: nElectrons and saved vector entries differ; use saved entries for plotted cluster positions.
  x [mm]:          min=119.817, max=232.021, mean=187.489
  y [mm]:          min=31.597, max=177.537, mean=99.213
  z [mm]:          min=0.000, max=889.996, mean=292.074
  ekin [eV]:       mean=0.000, rms=0.000

[anode]
  events:          100
  endpoints total: 768051
  primaries total: 768051
  good hits total: 0
  hit efficiency:  0.000000
  status counts:   {-5: 768051}
  arrival t [ns]:  min=3.619, max=4851.400, mean=2346.820
[plots] Wrote plots to analysis_plots
```

Important interpretation notes:

- `clusters.nElectrons` and the saved `cl_*` vector lengths can differ. In the
  tested file, `nElectrons` sums to 768051 while only 415570 cluster positions
  are saved. The script reports both and uses the saved vector entries for
  plotted cluster positions.
- `anode.nAnodeHits` was zero in the tested file, and all endpoint statuses were
  `-5`. Therefore the anode position/time plots currently represent drift
  endpoints, not accepted/good anode hits.
- The script sets `MPLCONFIGDIR` to `/tmp/fc0-lasersim-matplotlib` before
  importing matplotlib. This avoids matplotlib cache warnings when the default
  home config directory is not writable from WSL.

## Parallel Event Generation Strategy

The generation of events is conceptually easy to parallelize because events are
independent. However, the safest first implementation for this codebase is not
thread-level parallelism inside one `tpcmc` process. The safer approach is
event-level multi-process parallelism.

The current event loop is in:

```text
/local/simulazioni/fc0-lasersim/src/main.cc
```

The loop currently runs one event after another and uses shared objects created
before the loop:

```text
TRandom3
GasModel
FieldManager
Garfield::Sensor
TrackGenerator / Garfield::TrackHeed
DriftEngine / Garfield::AvalancheMC
ROOTOutput / TFile / TTree
ND280Output / oaEvent objects
```

These objects should not be casually shared across threads. The most delicate
parts are Garfield++ state, `TRandom3`, and ROOT output writing.

Recommended first strategy:

1. Split the total number of events into independent jobs.
2. Give each job its own JSON config.
3. Use a different `random_seed` for each job.
4. Use a different `output_file` for each job.
5. Run several `tpcmc` processes in parallel.
6. Merge ROOT files afterward with `hadd`, if the output trees are compatible.

Example:

```bash
cd /local/simulazioni/fc0-lasersim
source /opt/software/root-6.38.00/build/bin/thisroot.sh
source /local/simulazioni/geant4/geant4-v11.4.1-install/bin/geant4.sh
source /local/simulazioni/garfieldpp/install/share/Garfield/setupGarfield.sh

./build-almalinux9-gpt/tpcmc config/job_000.json &
./build-almalinux9-gpt/tpcmc config/job_001.json &
./build-almalinux9-gpt/tpcmc config/job_002.json &
./build-almalinux9-gpt/tpcmc config/job_003.json &
wait
```

Then merge:

```bash
hadd output_merged.root output_job_*.root
```

A future in-process threaded implementation is possible, but each worker thread
should own its own complete simulation chain:

```text
GasModel
FieldManager
DriftEngine
TrackGenerator or LaserGenerator
TRandom3
per-thread ROOT output file or per-thread output buffer
```

The shared writer is the main complication. Writing one shared `TTree` from
many threads should be avoided in the first implementation. Per-process or
per-thread ROOT files are simpler and much safer.

Useful future improvement:

- Add command-line overrides for `n_events`, `random_seed`, and `output_file`.
- Add a small launcher script that creates temporary job configs and runs jobs
  in parallel.
- Keep the core `tpcmc` event loop single-process/single-writer until the output
  model is redesigned.

## Local Git Repository Setup

The directory:

```text
/local/simulazioni/fc0-lasersim
```

was checked for Git management. It was already a Git repository:

```text
/local/simulazioni/fc0-lasersim/.git
```

with the current branch:

```text
main
```

and existing recent commits:

```text
ad923f1 readme
52dbee0 Merge remote-tracking branch 'refs/remotes/origin/main' into first-commit
721524a first commit
2943c91 Initial commit
```

The cernbox note file:

```text
/mnt/c/Users/colla/cernbox/ALL_README/README_fc0-lasersim.md
```

was copied into the project repository as:

```text
/local/simulazioni/fc0-lasersim/README_fc0-lasersim.md
```

and staged for Git.

The useful source/documentation changes staged for the next commit were:

```text
.gitignore
CMakeLists.txt
README.md
README_fc0-lasersim.md
analysis/analyze_output.py
src/config/Config.cc
```

The local runtime configuration file was intentionally left unstaged:

```text
config/simulation.json
```

because it contains local run parameters and had been modified interactively
during testing.

Generated products are intentionally ignored and should not be committed:

```text
build/
build*/
*.root
analysis_plots/
analysis/analysis_plots/
gas_files/*.gas
```

The `.gitignore` was extended to ignore generated build and analysis products:

```text
build*/
analysis_plots/
__pycache__/
```

At the time of setup, Git did not have a local user identity configured in this
WSL environment:

```bash
git config user.name
git config user.email
```

both returned no configured value. Before making the first local commit in this
environment, configure an identity, for example:

```bash
cd /local/simulazioni/fc0-lasersim
git config user.name "Your Name"
git config user.email "your.email@example.com"
```

Then commit the staged snapshot with:

```bash
git commit -m "Configure AlmaLinux Garfield and Geant4 build"
```

The existing `origin` remote was present, but its URL contained an embedded
access token. Before pushing, replace it with a clean remote URL and rely on
SSH keys or Git credential storage instead of storing credentials directly in
the repository remote configuration.

## Notes and Caveats

- The package builds cleanly with the installed Garfield++ and Geant4.
- The one-event runtime check now succeeds and writes `output.root`.
- The current code uses Garfield++ directly for the detector and generator
  parts; Geant4 is linked mainly to make the local environment explicit and to
  support the G4/ND280-style output ecosystem.
- The bundled `oaEvent` build emits some warnings about redefined compile-host
  metadata and deprecated copy operations, but these did not stop compilation.
- `TrackGenerator` emits a harmless member-initialization-order warning.
- `HATGeometryViz` emits a harmless unused-parameter warning.
- ROOT libraries appeared in `ldd` as `/opt/software/root/build/lib`, which is
  consistent with the ROOT setup script on this system.

## 2026-05-27 TPCMC Comparison and Waveform Port

This interaction started from the README/log file:

```text
/mnt/c/Users/colla/cernbox/ALL_README/README_fc0-lasersim.md
```

and the local project copy:

```text
/local/simulazioni/fc0-lasersim/README_fc0-lasersim.md
```

The two files had the same length and content at the start of the analysis.

README files inspected:

```text
/local/simulazioni/fc0-lasersim/README.md
/local/simulazioni/fc0-lasersim/README_fc0-lasersim.md
/local/simulazioni/TPCMC/TPCMC-20220112/README
/local/simulazioni/TPCMC/TPCMC-20220112/README-GC-ilcsoft-note_B_field
/local/simulazioni/TPCMC/TPCMC-20220112/bin/bfield_files/README
/local/simulazioni/TPCMC/TPCMC-20220112/bin/DESY-Bfield-studies/README-GC-ilcsoft-note_B_field
```

Main relationship between the two codebases:

```text
TPCMC-20220112       original Makefile-era Garfield/ROOT simulation
fc0-lasersim         newer CMake project derived from the same simulation ideas
```

Important TPCMC areas:

```text
inc/T2KConstants.h   hardcoded pad, DAQ, mapping, path constants
inc/Mapping.h        detector connector mapping
inc/DAQ.h            connector/DAQ channel mapping
inc/Pads.h           36 x 32 pad plane wrapper
inc/Pixel.h          pad/channel object
src/tpcmc.c          monolithic simulation, drift, signal, ROOT, and AQS writing
src/Mapping/         chip mapping text files
src/DAQ/             detector-to-ARC and ARC-to-DAQ mapping text files
```

Important fc0-lasersim areas:

```text
src/config/          JSON configuration
src/detector/        gas, fields, drift, and HAT geometry
src/generator/       laser and track primary generation
src/io/              ROOT and ND280 output
src/electronics/     new waveform builder added in this interaction
```

Key differences:

- `TPCMC-20220112` is mostly a single executable around `src/tpcmc.c`, with
  compile-time switches, hardcoded paths, and explicit T2K pad/DAQ/AQS logic.
- `fc0-lasersim` is modular C++17 with CMake, JSON runtime configuration,
  modern Garfield++ include style, HAT geometry utilities, ROOT output, and
  ND280/oaEvent support.
- Before this interaction, `fc0-lasersim` stored primary clusters and drift
  endpoints only. It had a placeholder `WaveformEventData`, but no code built
  per-pad waveforms.
- The old waveform logic lives in `TPCMC-20220112/src/tpcmc.c`: electron
  endpoints are grouped into sub-pads, signal is spread to nearby pads with the
  resistive-sheet formula, a front-end response is applied, the waveform is
  digitized, and old AQS zero-suppressed words can be written.

First waveform port implemented in `fc0-lasersim`:

```text
src/electronics/WaveformBuilder.hh
src/electronics/WaveformBuilder.cc
```

The new builder:

- consumes `AnodeEventData` after the drift step;
- maps global HAT `(Y,Z)` endpoint positions to ERAM-local pad coordinates;
- skips the quartz-window ERAM for waveform creation;
- groups endpoints into configurable sub-pads;
- merges charge in the same sub-pad within a configurable time window;
- spreads charge to neighbouring pads using the old TPCMC resistive-sheet
  expression;
- applies the old convolution response function;
- digitizes to ADC samples;
- writes only pads with non-zero post-threshold ADC content.

New electronics defaults adapted from the old TPCMC constants:

```text
waveform_internal_step_ns:    10
waveform_internal_samples:    1500
subpads_x/subpads_y:          5 / 5
spread_pads_x/spread_pads_y:  3 / 3
subpad_merge_time_ns:         38
resistive_rc_ns_per_mm2:      70
response_time_ns:             206
avalanche_gain:               1000
normalize_waveforms:          true
```

These can be overridden in the JSON `electronics` block. The existing
`config/simulation.json` was not modified during this implementation.

ROOT output level 3 is now enabled. With `output_level: 3`, `ROOTOutput` writes
the existing trees plus:

```text
waveforms
```

Branches in the new waveform tree:

```text
eventId
nPrimaries
nActivePads
nSamples
samplePeriodNs
wf_eram
wf_padZ
wf_padY
wf_peakSample
wf_peakAdc
wf_adcStart
wf_adcLength
wf_adc
```

`wf_adc` is a flat vector. For pad index `i`, its waveform is:

```text
wf_adc[ wf_adcStart[i] ... wf_adcStart[i] + wf_adcLength[i] - 1 ]
```

The flat representation was chosen after a runtime test showed that this ROOT
build did not have a compiled collection proxy for `vector<vector<int>>`.

Files changed for the waveform port:

```text
CMakeLists.txt
src/config/Config.hh
src/config/Config.cc
src/electronics/WaveformBuilder.hh
src/electronics/WaveformBuilder.cc
src/io/EventData.hh
src/io/EventWriter.hh
src/io/ROOTOutput.hh
src/io/ROOTOutput.cc
src/io/ND280Output.hh
src/io/ND280Output.cc
src/main.cc
```

Verification:

```bash
cmake --build build-almalinux9-gpt -j2
```

completed successfully.

A small temporary runtime check was done with:

```text
/tmp/fc0_waveform_test.json
```

and output:

```text
/tmp/fc0_waveform_test.root
```

The temporary config used one low-density laser event and `output_level: 3` to
exercise the waveform path without changing the project config.

`rootls -t /tmp/fc0_waveform_test.root` showed three trees:

```text
clusters
anode
waveforms
```

A ROOT scan of the waveform tree reported:

```text
nActivePads: 51
nSamples:    511
samplePeriodNs: 25
Max peak ADC: 4093
Min peak ADC: 14
```

Caveats after the first waveform-port step:

- The first waveform port used configurable `electronics.avalanche_gain`
  instead of the old TPCMC `MC_ResMM::eGain()` per-electron amplification.
  This was addressed later by adding `electronics.gain_model = "polya"` while
  keeping the constant gain as the default.
- The old DAQ/AQS writer and exact card/chip/channel mapping were not ported
  yet. The new output is ROOT waveform data only.
- The waveform builder currently uses all endpoints that fall in an active pad
  area, even if Garfield status is non-zero. This matches the practical old
  behavior more closely and avoids producing no waveforms while the current
  drift status handling still needs review.
- The waveform generation cost scales with active sub-pads, neighbouring pads,
  and internal samples. For production statistics, the next optimization should
  focus on sparse waveform ranges and/or configurable pad spread.

README maintenance note:

- For this development session, this file should be updated after each
  substantive analysis or code-change interaction.
- The `/mnt/c/.../README_fc0-lasersim.md` cernbox copy should be kept in sync
  with this local project copy whenever filesystem permissions allow it.

## 2026-05-27 Clarification: Pre-Port Waveform State

Before the waveform port in this session, `fc0-lasersim` did not actually build
per-pad waveforms.

The pre-port code had a placeholder data structure:

```text
src/io/EventData.hh
WaveformEventData:
  eventId
  nPrimaries
  nActivePads
  padX
  padY
  adcWaveforms
```

but there was no waveform-builder class, no waveform call in `src/main.cc`, and
no `EventWriter::WriteEvent(WaveformEventData)` interface.

The event loop stopped after:

```text
1. primary cluster generation
2. electron drift to endpoint positions
3. anode endpoint ROOT writing
```

The only pad-related operation before the port was a direct pad-index estimate
for each drift endpoint:

```cpp
padZ = floor(ep.z / cfg.detector.padPitchXMm)
padY = floor(ep.y / cfg.detector.padPitchYMm)
```

That was an endpoint-to-pad label, not a waveform. It did not include charge
spreading, sub-pad clustering, electronics response, ADC sampling, thresholding,
or per-pad waveform output.

There was also an inconsistency in the old configuration/API:

```text
Config.hh default: outputLevel = 3  // documented as waveform
ROOTOutput.cc: accepted only output levels 1 and 2
```

So level 3 was documented as an intended future waveform level, but was not
implemented before this session.

## 2026-05-27 Git Checkpoint

After the first waveform-port implementation and verification, a local Git
checkpoint commit was created:

```text
529309d Checkpoint waveform port
```

This commit preserves the current reversible development state for:

```text
.gitignore
CMakeLists.txt
README.md
README_fc0-lasersim.md
analysis/analyze_output.py
src/config/Config.cc
src/config/Config.hh
src/electronics/WaveformBuilder.cc
src/electronics/WaveformBuilder.hh
src/io/EventData.hh
src/io/EventWriter.hh
src/io/ND280Output.cc
src/io/ND280Output.hh
src/io/ROOTOutput.cc
src/io/ROOTOutput.hh
src/main.cc
```

The local runtime file was intentionally left outside that checkpoint:

```text
config/simulation.json
```

At the time of the checkpoint, `git status --short` showed only:

```text
 M config/simulation.json
```

Useful commands:

```bash
git show --stat 529309d
git diff 529309d
```

The commit was made with per-command identity:

```text
Codex <codex@local>
```

because no local repository `user.name` or `user.email` was configured.

## 2026-05-27 Event Display Tool

A reusable event display was added:

```text
analysis/event_display.py
```

Purpose:

- display one event at a time;
- show primary electrons projected on the pad plane in `(Z,Y)`;
- show drift endpoints on the same pad-plane view;
- show active waveform pads coloured by peak ADC;
- update the waveform panel when the cursor is over an active pad or when a pad
  is clicked;
- provide a path to overlay reconstructed tracks later.

Interactive use:

```bash
cd /local/simulazioni/fc0-lasersim
source /opt/software/root-6.38.00/build/bin/thisroot.sh
python3 analysis/event_display.py output.root
```

Useful controls:

```text
mouse move over active pad   update waveform panel
mouse click on pad           select that pad waveform
Previous/Next buttons        change event
left/right keys              previous/next event
p/n keys                     previous/next event
```

Snapshot mode, useful for tests or documentation:

```bash
python3 analysis/event_display.py /tmp/fc0_waveform_test.root \
  --save /tmp/fc0_event_display_test.png
```

The script also works on older ROOT files without a `waveforms` tree. In that
case it still displays the projected primary electrons and anode endpoints, and
the waveform panel reports that no waveform is available for the event.

Optional reconstructed-track overlay:

```bash
python3 analysis/event_display.py output.root --reco-csv reconstructed_tracks.csv
```

Expected CSV columns:

```text
event,track,z_mm,y_mm
```

The display groups points by `event` and `track`, then overlays each track as a
polyline on the same `(Z,Y)` event projection. This is intended for later
comparison of reconstructed tracks with simulated electron positions.

Verification performed:

```bash
python3 analysis/event_display.py /tmp/fc0_waveform_test.root \
  --save /tmp/fc0_event_display_test.png
python3 -m py_compile analysis/event_display.py
python3 analysis/event_display.py output.root \
  --save /tmp/fc0_event_display_no_waveform.png
```

The waveform-file snapshot was produced successfully and showed:

```text
left panel:  ERAM layout, projected primary electrons, drift endpoints, active pads
right panel: selected pad waveform versus time
```

The older `output.root` check confirmed graceful behaviour when no waveform tree
is present.

## 2026-05-28 Run Simulation and Event Display

Current local run configuration observed on 2026-05-28:

```text
config/simulation.json
n_events:      100
generator:     track / muon
drift_method:  MC
output_file:   output.root
output_level:  2
output_format: root
```

Important point:

```text
output_level 2  -> clusters + anode drift endpoints
output_level 3  -> clusters + anode drift endpoints + pad waveforms
```

For using the event display with pad waveforms, edit `config/simulation.json`
before the run:

```json
"n_events": 1,
"output_level": 3
```

For larger production runs, increase `n_events` again after validating the
waveform/event-display workflow.

Standard setup and run commands:

```bash
cd /local/simulazioni/fc0-lasersim

source /opt/software/root-6.38.00/build/bin/thisroot.sh
source /local/simulazioni/geant4/geant4-v11.4.1-install/bin/geant4.sh
source /local/simulazioni/garfieldpp/install/share/Garfield/setupGarfield.sh

cmake --build build-almalinux9-gpt -j"$(nproc)"

./build-almalinux9-gpt/tpcmc config/simulation.json
```

The simulation writes:

```text
/local/simulazioni/fc0-lasersim/output.root
```

Start the event display:

```bash
python3 analysis/event_display.py output.root
```

Useful event-display commands:

```bash
python3 analysis/event_display.py output.root --event 0
python3 analysis/event_display.py output.root --event 0 --save analysis_plots/event_000.png
```

Interactive controls:

```text
mouse over active pad  -> show that pad waveform
click pad              -> select that waveform
right / n              -> next event
left / p               -> previous event
```

If the ROOT file was produced with `output_level: 2`, the event display still
shows the electron projections and anode endpoints, but no pad waveform panel is
available. Use `output_level: 3` to produce the `waveforms` tree.

## 2026-05-28 Waveform Display Troubleshooting

Observed issue:

```text
The event display opened output.root, but no waveforms were visible.
```

Cause:

```text
config/simulation.json had output_level: 2
```

The current `output.root` was checked with:

```bash
rootls -t output.root
```

and contained only:

```text
clusters
anode
```

Therefore the event display could show the projected electrons and drift
endpoints, but it had no waveform tree to read.

The previously verified waveform test file was:

```text
/tmp/fc0_waveform_test.root
```

and `rootls -t /tmp/fc0_waveform_test.root` showed:

```text
clusters
anode
waveforms
```

To produce a new displayable waveform file in the project directory:

1. Edit `config/simulation.json`.

For a quick test, use:

```json
"n_events": 1,
"output_level": 3
```

2. Run the simulation:

```bash
cd /local/simulazioni/fc0-lasersim
source /opt/software/root-6.38.00/build/bin/thisroot.sh
source /local/simulazioni/geant4/geant4-v11.4.1-install/bin/geant4.sh
source /local/simulazioni/garfieldpp/install/share/Garfield/setupGarfield.sh

./build-almalinux9-gpt/tpcmc config/simulation.json
```

3. Verify that waveforms were written:

```bash
rootls -t output.root
```

The output must include:

```text
waveforms
```

4. Open the event display:

```bash
python3 analysis/event_display.py output.root
```

Summary:

```text
output_level: 2  -> no waveform tree, no pad waveforms in display
output_level: 3  -> writes waveforms tree, pad waveforms visible in display
```

## 2026-05-28 ERAM 2 Waveform Dominance

Observation:

```text
In the event display, almost all visible waveform pads appear in ERAM 2.
```

The current `output.root` was checked directly, not only through the display.
The `waveforms` tree contains:

```text
entries: 100 events
total waveform pads by ERAM:
  ERAM 1: 32
  ERAM 2: 24281
```

Example events:

```text
event 0:  nActivePads = 251, ERAM counts = {2: 251}
event 26: nActivePads = 269, ERAM counts = {1: 32, 2: 237}
```

Therefore the event display is not hiding large numbers of waveform pads in
other ERAMs; the waveform tree itself is dominated by ERAM 2.

Current track configuration:

```json
"track": {
  "particle": "muon",
  "momentum_GeV": 1.0,
  "position": { "x_mm": 187.0, "y_mm": 100, "z_mm": 0.0 },
  "direction": { "dx": 0.0, "dy": 0.0, "dz": 1.0 }
}
```

This starts a track on the top-row side of the HAT layout near `z = 0` and moves
toward positive `z`.

Relevant top-row ERAM Z coverage:

```text
ERAM 0: z = [-834.48, -428.52] mm
ERAM 1: z = [-413.48,   -7.52] mm
ERAM 2: z = [   7.52,  413.48] mm
ERAM 3: z = [ 428.52,  834.48] mm, quartz-window position
```

Because the track starts near `z = 0` and goes in the positive `z` direction, it
naturally populates ERAM 2 first. The next top-row region in positive `z` is
ERAM 3, but in this geometry ERAM 3 is the quartz-window position and waveform
creation intentionally skips it. This explains why the output is almost all
ERAM 2 for the current run.

For a test intended to populate multiple top-row pad ERAMs, use a track that
starts before ERAM 0 and crosses through the row, for example:

```json
"position": { "x_mm": 187.0, "y_mm": 100.0, "z_mm": -850.0 },
"direction": { "dx": 0.0, "dy": 0.0, "dz": 1.0 }
```

This should cross ERAM 0, ERAM 1, ERAM 2, and then the ERAM 3 quartz-window
region. For the bottom row, use a negative `y_mm`, for example `y_mm = -100.0`.

Current conclusion:

```text
No display bug was found for this observation.
The current run geometry mostly creates waveform pads in ERAM 2.
ERAM 3 is skipped by design because it is the quartz-window ERAM.
```

## 2026-05-29 Configurable Avalanche Gain Models

Implemented configurable gain selection for waveform generation.

Default behaviour remains a constant per-electron avalanche gain:

```json
"electronics": {
  "gain_model": "constant",
  "avalanche_gain": 1000.0,
  "normalize_waveforms": true
}
```

The old TPCMC `MC_ResMM::eGain()` Polya-style option is now available:

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

Meaning of the Polya parameters:

```text
mean_gain:        old MC_ResMM MeanGain, default 1000
polya_parameter:  old MC_ResMM PolyPara, default 2.3
max_gain_ratio:   old eGain integration upper ratio, default 5
n_bins:           old eGain CDF bin count, default 1000
random_seed:      -1 derives the gain RNG seed from run.random_seed
```

Implementation notes:

- `WaveformBuilder` now samples the endpoint charge through `SampleAvalancheGain()`.
- `gain_model = "constant"` uses `electronics.avalanche_gain`.
- `gain_model = "polya"` or `"mc_resmm_polya"` builds the same discrete inverse
  CDF used by `MC_ResMM::eGain()`.
- The old strict CDF-bin selection is preserved; sampled Polya gains are integer
  values derived from `gain_ratio_center * mean_gain`.
- `electronics.gain` is still kept for the ADC conversion scale used only when
  `normalize_waveforms` is false. It is not the per-electron avalanche gain.
- With `normalize_waveforms: true`, a uniform scale change is removed by
  event-level normalization, but Polya still changes relative charge sharing
  because each electron receives an independent gain.

Files changed:

```text
src/config/Config.hh
src/config/Config.cc
src/electronics/WaveformBuilder.hh
src/electronics/WaveformBuilder.cc
config/simulation.json
README.md
README_fc0-lasersim.md
```

Verification:

```bash
cmake --build build-almalinux9-gpt -j2
```

completed successfully.

Two one-event runtime smoke tests were also run with temporary configs:

```text
/tmp/fc0_gain_constant.json -> /tmp/fc0_gain_constant.root
/tmp/fc0_gain_polya.json    -> /tmp/fc0_gain_polya.root
```

Both outputs contained the expected ROOT trees:

```text
clusters
anode
waveforms
```

A PyROOT check of the waveform tree reported:

```text
constant entries 1 nActivePads 502
polya    entries 1 nActivePads 456
```

Git/checkpoint decision:

- `config/simulation.json` is intentionally included in the checkpoint.
- Its current run block is treated as the repository default waveform/event-display
  test configuration, not as an accidental local runtime edit.
- The committed default uses 10 MC track events, output level 3, ROOT output,
  constant avalanche gain, and the old MC_ResMM Polya parameters exposed for
  switching with `electronics.gain_model = "polya"`.
