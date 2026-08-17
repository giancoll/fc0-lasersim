# Source this file from a shell; do not execute it.
# Override FC0_DEPS, ROOT_PREFIX, GEANT4_INSTALL, GARFIELD_HOME, or
# GARFIELD_INSTALL before sourcing when dependencies are installed elsewhere.

: "${FC0_DEPS:=${HOME}/opt/fc0}"
: "${ROOT_PREFIX:=${FC0_DEPS}/root}"
: "${GEANT4_INSTALL:=${FC0_DEPS}/geant4-v11.4.1-install}"
: "${GARFIELD_HOME:=${FC0_DEPS}/src/garfieldpp}"
: "${GARFIELD_INSTALL:=${FC0_DEPS}/garfieldpp-install}"

fc0_source_if_present() {
    if [ -f "$1" ]; then
        # shellcheck disable=SC1090
        source "$1"
    else
        printf '[fc0-lasersim] Missing setup file: %s\n' "$1" >&2
        return 1
    fi
}

fc0_source_if_present "${ROOT_PREFIX}/bin/thisroot.sh" || return 1
fc0_source_if_present "${GEANT4_INSTALL}/bin/geant4.sh" || return 1
fc0_source_if_present "${GARFIELD_INSTALL}/share/Garfield/setupGarfield.sh" || return 1

export FC0_DEPS ROOT_PREFIX GEANT4_INSTALL GARFIELD_HOME GARFIELD_INSTALL
export HEED_DATABASE="${GARFIELD_INSTALL}/share/Heed/database"
export CMAKE_PREFIX_PATH="${ROOT_PREFIX}:${GARFIELD_INSTALL}:${GEANT4_INSTALL}:${CMAKE_PREFIX_PATH:-}"

printf '[fc0-lasersim] ROOT:      %s\n' "${ROOT_PREFIX}"
printf '[fc0-lasersim] Geant4:    %s\n' "${GEANT4_INSTALL}"
printf '[fc0-lasersim] Garfield:  %s\n' "${GARFIELD_INSTALL}"
printf '[fc0-lasersim] Heed data: %s\n' "${HEED_DATABASE}"
