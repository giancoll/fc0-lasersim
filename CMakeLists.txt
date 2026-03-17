cmake_minimum_required(VERSION 3.15)
project(TPC-sim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------
find_package(ROOT REQUIRED COMPONENTS Core RIO Tree Hist MathCore Physics Geom)
find_package(GSL  REQUIRED)
find_package(Garfield REQUIRED HINTS $ENV{GARFIELD_HOME}/install)

include(${ROOT_USE_FILE})

# oaEvent — bundled nd280 oaEvent source (G4 truth, hits, digits, TND280Event)
add_subdirectory(external/oaEvent)

# nlohmann/json — header-only, fetched automatically
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

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
message(STATUS "  Debug    : ${DEBUG_MODE}")
message(STATUS "  Output   : ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "")
message(STATUS "Targets:")
message(STATUS "  tpcmc                  - Main TPC simulation")
message(STATUS "  viz_hat_geometry       - HAT geometry visualization (requires ROOT)")
