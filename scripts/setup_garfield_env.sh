# Source this file before running charged-particle TrackHeed jobs.
# Example:
#   source scripts/setup_garfield_env.sh
#   ./build-almalinux9-gpt/tpcmc config/muon_horizontal_100_anode.json

: "${GARFIELD_HOME:=/local/simulazioni/garfieldpp}"
: "${GARFIELD_INSTALL:=${GARFIELD_HOME}/install}"
export HEED_DATABASE="${GARFIELD_INSTALL}/share/Heed/database"
export CMAKE_PREFIX_PATH="${GARFIELD_INSTALL}:${CMAKE_PREFIX_PATH:-}"
export LD_LIBRARY_PATH="${GARFIELD_INSTALL}/lib64:${LD_LIBRARY_PATH:-}"
export GARFIELD_HOME GARFIELD_INSTALL HEED_DATABASE CMAKE_PREFIX_PATH LD_LIBRARY_PATH

echo "[fc0-lasersim] GARFIELD_INSTALL=${GARFIELD_INSTALL}"
echo "[fc0-lasersim] HEED_DATABASE=${HEED_DATABASE}"
