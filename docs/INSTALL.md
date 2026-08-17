# Installation

This document describes the AlmaLinux 9 setup used to build `fc0-lasersim`.
Install all packages with one compiler toolchain: ROOT, Garfield++, Geant4, and
this project exchange C++ objects and must be ABI-compatible.

## Known-Good Versions

The current workstation build was verified with:

| Package | Version |
|---|---:|
| AlmaLinux | 9.7, x86-64 |
| GCC | 11.x system compiler |
| ROOT | 6.38.00 |
| Garfield++ | tag `2025.12` plus later local commits |
| Geant4 | 11.4.1 |
| GSL | 2.6 |
| nlohmann/json | 3.11.3 |
| CMake | 3.15 or newer |

The exact matrix is the reproducibility baseline, not a requirement that newer
compatible releases will fail. Record all versions when producing comparison
samples.

## 1. System Packages

On AlmaLinux 9, install the compiler and common development dependencies:

```bash
sudo dnf install -y epel-release
sudo dnf groupinstall -y "Development Tools"
sudo dnf install -y \
  cmake git gcc-c++ gcc-gfortran gsl-devel \
  libX11-devel libXext-devel libXft-devel libXpm-devel \
  mesa-libGL-devel mesa-libGLU-devel openssl-devel \
  python3 python3-pip
```

The analysis scripts additionally need NumPy and Matplotlib. PyROOT is supplied
by ROOT and must match the Python ABI used by the ROOT build.

```bash
python3 -m pip install --user numpy matplotlib
```

Use a common installation root:

```bash
export FC0_DEPS="$HOME/opt/fc0"
mkdir -p "$FC0_DEPS/src"
```

## 2. ROOT

Install ROOT before Garfield++. The simplest route is an official precompiled
AlmaLinux 9 binary whose compiler and Python versions match the host. Download
it from <https://root.cern/install/>, unpack it as `$FC0_DEPS/root`, then run:

```bash
export ROOT_PREFIX="$FC0_DEPS/root"
source "$ROOT_PREFIX/bin/thisroot.sh"
root-config --version
```

The local reference installation was built from source and is located at
`/opt/software/root-6.38.00/build`. A source build is also valid; follow the
official ROOT build instructions and make sure all components required in
`CMakeLists.txt` are enabled.

## 3. Geant4

Download the Geant4 11.4.1 source release from the official download page and
unpack it under `$FC0_DEPS/src/geant4-v11.4.1`. Configure an out-of-source
release build:

```bash
export GEANT4_SOURCE="$FC0_DEPS/src/geant4-v11.4.1"
export GEANT4_BUILD="$FC0_DEPS/build/geant4-v11.4.1"
export GEANT4_INSTALL="$FC0_DEPS/geant4-v11.4.1-install"

cmake -S "$GEANT4_SOURCE" -B "$GEANT4_BUILD" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$GEANT4_INSTALL" \
  -DGEANT4_INSTALL_DATA=ON \
  -DGEANT4_BUILD_MULTITHREADED=ON
cmake --build "$GEANT4_BUILD" -j"$(nproc)"
cmake --install "$GEANT4_BUILD"
source "$GEANT4_INSTALL/bin/geant4.sh"
geant4-config --version
```

`GEANT4_INSTALL_DATA=ON` downloads the physics datasets during configuration,
so that step requires network access. Geant4 is currently linked for the
bundled ND280/oaEvent data model; the principal gas ionisation and electron
drift path uses Garfield++, Heed, and Magboltz.

## 4. Garfield++

Garfield++ requires ROOT, GSL, CMake, a C++ compiler compatible with ROOT, and
a Fortran compiler for Magboltz. Start from a shell where `thisroot.sh` has
already been sourced:

```bash
export GARFIELD_HOME="$FC0_DEPS/src/garfieldpp"
export GARFIELD_INSTALL="$FC0_DEPS/garfieldpp-install"

git clone https://gitlab.cern.ch/garfield/garfieldpp.git "$GARFIELD_HOME"
git -C "$GARFIELD_HOME" checkout 2025.12

cmake -S "$GARFIELD_HOME" -B "$GARFIELD_HOME/build" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$GARFIELD_INSTALL"
cmake --build "$GARFIELD_HOME/build" -j"$(nproc)"
cmake --install "$GARFIELD_HOME/build"
source "$GARFIELD_INSTALL/share/Garfield/setupGarfield.sh"
```

Verify the installed CMake package and Heed database:

```bash
test -f "$GARFIELD_INSTALL/lib64/cmake/Garfield/GarfieldConfig.cmake" || \
  test -f "$GARFIELD_INSTALL/lib/cmake/Garfield/GarfieldConfig.cmake"
test -d "$GARFIELD_INSTALL/share/Heed/database"
export HEED_DATABASE="$GARFIELD_INSTALL/share/Heed/database"
```

The explicit `HEED_DATABASE` export prevents `TrackHeed::Initialise: Cannot
retrieve database path` when generating muons.

## 5. Environment Check

After cloning this repository, configure the paths once per shell and source
the helper:

```bash
export FC0_DEPS="$HOME/opt/fc0"
source scripts/setup_env.sh

root-config --version
geant4-config --version
test -d "$HEED_DATABASE"
```

For the paths on the original workstation use:

```bash
export ROOT_PREFIX=/opt/software/root-6.38.00/build
export GEANT4_INSTALL=/local/simulazioni/geant4/geant4-v11.4.1-install
export GARFIELD_HOME=/local/simulazioni/garfieldpp
export GARFIELD_INSTALL=/local/simulazioni/garfieldpp/install
source scripts/setup_env.sh
```

Official references:

- ROOT installation: <https://root.cern/install/>
- Garfield++ guide: <https://garfieldpp.web.cern.ch/doxygen/UserGuide.pdf>
- Geant4 installation guide: <https://geant4.web.cern.ch/documentation/dev/ig_html/InstallationGuide/>
