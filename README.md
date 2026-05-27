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

Useful CMake options:

```bash
-DFC0_ROOT_PREFIX=/opt/software/root-6.38.00/build
-DFC0_GARFIELD_PREFIX=/local/simulazioni/garfieldpp/install
-DFC0_GEANT4_PREFIX=/local/simulazioni/geant4/geant4-v11.4.1-install
-DFC0_WITH_GEANT4=ON
-DDEBUG_MODE=ON
```

