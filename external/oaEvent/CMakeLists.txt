cmake_minimum_required(VERSION 3.15)
project(oaEvent CXX)

find_package(ROOT REQUIRED COMPONENTS Core RIO Tree Hist MathCore Physics
                                       EG Geom Gui Graf Gpad)
include(${ROOT_USE_FILE})

# Generate oaEvent_make_date.hxx (normally produced by the nd280 build system)
string(TIMESTAMP _COMPILE_DATE)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/oaEvent_make_date.hxx"
     "#define oaEvent_COMPILE_DATE \"${_COMPILE_DATE}\"\n"
     "#define oaEvent_COMPILE_HOST \"${CMAKE_HOST_SYSTEM_NAME}\"\n"
     "#define oaEvent_COMPILE_DIR  \"${CMAKE_CURRENT_SOURCE_DIR}\"\n"
     "#define oaEvent_COMPILE_UNAME \"${CMAKE_HOST_SYSTEM}\"\n"
)

set(OAE_INC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
set(OAE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB OAE_SOURCES      "${OAE_SRC}/*.cxx")
file(GLOB OAE_DICT_HEADERS "${OAE_INC}/*.hxx")  # *.hxx only — excludes *_LinkDef.h

# Auto-generate combined LinkDef by concatenating all *_LinkDef.h files.
# Each individual file already has its own #ifdef __CINT__ ... #endif guard,
# so multiple guarded blocks in one file is valid — no regex stripping needed.
file(GLOB _LINKDEF_FILES "${OAE_INC}/*_LinkDef.h")
list(SORT _LINKDEF_FILES)
set(_COMBINED_LINKDEF "${CMAKE_CURRENT_BINARY_DIR}/oaEventCombinedLinkDef.h")
file(WRITE "${_COMBINED_LINKDEF}" "")
foreach(_ldf IN LISTS _LINKDEF_FILES)
    file(READ "${_ldf}" _ldf_content)
    file(APPEND "${_COMBINED_LINKDEF}" "${_ldf_content}\n")
endforeach()

ROOT_GENERATE_DICTIONARY(oaEventDict
    ${OAE_DICT_HEADERS}
    LINKDEF "${_COMBINED_LINKDEF}"
    OPTIONS -I${OAE_INC} -I${CMAKE_CURRENT_BINARY_DIR}
)

add_library(oaEvent SHARED ${OAE_SOURCES} oaEventDict.cxx)

target_include_directories(oaEvent
    PUBLIC  ${OAE_INC}
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(oaEvent PUBLIC
    ROOT::Core ROOT::RIO ROOT::Tree ROOT::Hist ROOT::MathCore ROOT::Physics
    ROOT::EG ROOT::Geom ROOT::Gui ROOT::Graf ROOT::Gpad
)

target_compile_options(oaEvent PRIVATE -O2 -Wall -Wno-long-long -Wno-unused-parameter)
