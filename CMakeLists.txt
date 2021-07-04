cmake_minimum_required(VERSION 3.16)
project(YART)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release") 
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ---------- Check for submodules ----------

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/YART-Lib/vendor/spdlog")
    message(FATAL_ERROR "The spdlog submodule directory is missing! "
        "You probably did not clone the project with --recursive. It is possible to recover "
        "by running \"git submodule update --init --recursive\"")
endif()

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/YART-Lib/vendor/openexr")
    message(FATAL_ERROR "The openexr submodule directory is missing! "
        "You probably did not clone the project with --recursive. It is possible to recover "
        "by running \"git submodule update --init --recursive\"")
endif()

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/YART-Lib/vendor/Imath")
    message(FATAL_ERROR "The Imath submodule directory is missing! "
        "You probably did not clone the project with --recursive. It is possible to recover "
        "by running \"git submodule update --init --recursive\"")
endif()

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/YART-Lib/vendor/zlib")
    message(FATAL_ERROR "The zlib submodule directory is missing! "
        "You probably did not clone the project with --recursive. It is possible to recover "
        "by running \"git submodule update --init --recursive\"")
endif()

# ---------- specify the C++ standard ----------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ---------- Global Configuration Settings ----------

add_compile_definitions($<$<CONFIG:Debug>:CONFIGURATION_DEBUG>)
add_compile_definitions($<$<CONFIG:Release>:CONFIGURATION_RELEASE>)

add_compile_definitions($<$<PLATFORM_ID:Windows>:PLATFORM_WINDOWS>)
add_compile_definitions($<$<PLATFORM_ID:Linux>:PLATFORM_LINUX>)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    message(FATAL_ERROR "Unknown Compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# ---------- Add Third Party Libraries ----------

# zlib
set(ZLIB_BUILD_STATIC_LIBS ON CACHE BOOL " " FORCE)
set(ZLIB_BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
add_subdirectory(YART-Lib/vendor/zlib)

add_library(ZLIB::ZLIB ALIAS zlibstatic)
include_directories(YART-Lib/vendor/zlib ${CMAKE_CURRENT_BINARY_DIR}/YART-Lib/vendor/zlib) # yuck, but so openexr/ptex can find zlib.h...

# Imath
set(BUILD_SHARED_LIBS            OFF CACHE BOOL " " FORCE)

add_subdirectory (YART-Lib/vendor/Imath)

# openexr
set(ILMBASE_NAMESPACE_VERSIONING OFF CACHE BOOL " " FORCE)
set(OPENEXR_NAMESPACE_VERSIONING OFF CACHE BOOL " " FORCE)
set(OPENEXR_BUILD_SHARED_LIBS    OFF CACHE BOOL " " FORCE)
set(ILMBASE_BUILD_SHARED_LIBS    OFF CACHE BOOL " " FORCE)
set(PYILMBASE_ENABLE             OFF CACHE BOOL " " FORCE)
set(OPENEXR_BUILD_UTILS          OFF CACHE BOOL " " FORCE)
set(OPENEXR_INSTALL_EXAMPLES     OFF CACHE BOOL " " FORCE)
set(OPENEXR_INSTALL_TOOLS        OFF CACHE BOOL " " FORCE)
set(OPENEXR_INSTALL              OFF CACHE BOOL " " FORCE)

add_subdirectory (YART-Lib/vendor/openexr)

# Move projects to its own folder
set_property(TARGET Imath Iex IlmThread OpenEXR OpenEXRUtil zlibstatic PROPERTY FOLDER "vendor")

# ---------- Add Subdirectories ----------

add_subdirectory(YART-Lib)
add_subdirectory(YART-Cli)
add_subdirectory(YART-Test)
