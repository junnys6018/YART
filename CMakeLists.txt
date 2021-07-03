cmake_minimum_required(VERSION 3.16)
project(YART)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release") 

# ---------- Check for submodules ----------

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/YART-Lib/vendor/spdlog")
    message(FATAL_ERROR "The spdlog submodule directory is missing! "
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

# ---------- Add Subdirectories ----------

add_subdirectory(YART-Lib)
add_subdirectory(YART-Cli)
add_subdirectory(YART-Test)
