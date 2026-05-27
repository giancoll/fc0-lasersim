cmake_minimum_required(VERSION 3.15)
project(TPC-sim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------
set(FC0_ROOT_PREFIX "/opt/software/root-6.38.00/build" CACHE PATH
    "ROOT installation prefix")
set(FC0_GARFIELD_PREFIX "/local/simulazioni/garfieldpp/install" CACHE PATH
    "Garfield++ installation prefix")
set(FC0_GEANT4_PREFIX "/local/simulazioni/geant4/geant4-v11.4.1-install" CACHE PATH
    "Geant4 installation prefix")
option(FC0_WITH_GEANT4 "Find and link the installed Geant4 package" ON)

list(PREPEND CMAKE_PREFIX_PATH
    "${FC0_ROOT_PREFIX}"
    "${FC0_GARFIELD_PREFIX}"
    "${FC0_GEANT4_PREFIX}"
    "/usr/local"
)

find_package(ROOT REQUIRED CONFIG
    COMPONENTS Core RIO Tree Hist MathCore Physics Geom Gpad Graf Gui EG)
find_package(GSL REQUIRED)
find_package(Garfield REQUIRED CONFIG)

if(FC0_WITH_GEANT4)
    find_package(Geant4 REQUIRED CONFIG)
    include(${Geant4_USE_FILE})
endif()

include(${ROOT_USE_FILE})

# oaEvent — bundled nd280 oaEvent source (G4 truth, hits, digits, TND280Event)
add_subdirectory(external/oaEvent)

# nlohmann/json — prefer the local CMake package, with FetchContent as fallback.
# The AlmaLinux WSL2 environment used for this build already has it in
# /usr/local, so normal offline builds do not need network access.
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

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
set(SOURCES
    src/main.cc
    src/config/Config.cc
    src/detector/GasModel.cc
    src/detector/FieldManager.cc
    src/detector/DriftEngine.cc
    src/detector/HATGeometry.cc
    src/detector/HATGeometryViz.cc
    src/electronics/WaveformBuilder.cc
    src/generator/LaserGenerator.cc
    src/generator/TrackGenerator.cc
    src/io/ROOTOutput.cc
    src/io/ND280Output.cc
)

add_executable(tpcmc ${SOURCES})

target_include_directories(tpcmc PRIVATE src)

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

target_compile_options(tpcmc PRIVATE
    -O2 -Wall -Wextra -Wno-long-long -Wno-unused-parameter
)

# Debug build: cmake -DDEBUG_MODE=ON ..
option(DEBUG_MODE "Enable verbose debug output" OFF)
target_compile_definitions(tpcmc PRIVATE
    $<$<BOOL:${DEBUG_MODE}>:TPC_DEBUG>
)

# ---------------------------------------------------------------------------
# HAT Geometry Visualization Example
# ---------------------------------------------------------------------------
add_executable(viz_hat_geometry
    examples/VisualizeHATGeometry.cc
    src/config/Config.cc
    src/detector/HATGeometry.cc
    src/detector/HATGeometryViz.cc
)

target_include_directories(viz_hat_geometry PRIVATE src)

target_link_libraries(viz_hat_geometry PRIVATE
    ROOT::Core ROOT::RIO ROOT::Hist ROOT::Gpad ROOT::Tree
    nlohmann_json::nlohmann_json
)

target_compile_options(viz_hat_geometry PRIVATE
    -O2 -Wall -Wextra
)

# ---------------------------------------------------------------------------
# Build info
# ---------------------------------------------------------------------------
message(STATUS "TPC-sim build configuration:")
message(STATUS "  ROOT     : ${ROOT_VERSION}")
message(STATUS "  GSL      : ${GSL_VERSION}")
message(STATUS "  Garfield : ${FC0_GARFIELD_PREFIX}")
if(FC0_WITH_GEANT4)
message(STATUS "  Geant4   : ${Geant4_VERSION}")
endif()
message(STATUS "  nlohmann : ${nlohmann_json_VERSION}")
message(STATUS "  Debug    : ${DEBUG_MODE}")
message(STATUS "  Output   : ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "")
message(STATUS "Targets:")
message(STATUS "  tpcmc                  - Main TPC simulation")
message(STATUS "  viz_hat_geometry       - HAT geometry visualization (requires ROOT)")
