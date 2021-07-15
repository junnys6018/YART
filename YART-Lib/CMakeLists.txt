
file(GLOB_RECURSE SOURCES src/*)
file(GLOB_RECURSE HEADERS include/*)

set(GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/templatedeclaration.h" "${CMAKE_CURRENT_BINARY_DIR}/templatebuild.cpp")

# Generate configuration file
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/yart.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/include/yart.h")

# Auto generate template declaration and instantiation code
message(STATUS "Generating template code")
execute_process(
    COMMAND python GenerateTemplates.py templates.json -o ${CMAKE_CURRENT_BINARY_DIR}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/scripts"
)

add_library(YART-Lib STATIC ${SOURCES} ${HEADERS} ${GENERATED_FILES})

source_group(" " REGULAR_EXPRESSION "(src|include)/[^/]+\.(h|cpp)")
source_group(accelerators REGULAR_EXPRESSION "(src|include)/accelerators/.*")
source_group(cameras REGULAR_EXPRESSION "(src|include)/cameras/.*")
source_group(core REGULAR_EXPRESSION "(src|include)/core/.*")
source_group(filters REGULAR_EXPRESSION "(src|include)/filters/.*")
source_group(geometry REGULAR_EXPRESSION "(src|include)/geometry/.*")
source_group(math REGULAR_EXPRESSION "(src|include)/math/.*")
source_group(samplers REGULAR_EXPRESSION "(src|include)/samplers/.*")
source_group(materials REGULAR_EXPRESSION "(src|include)/materials/.*")
source_group(integrators REGULAR_EXPRESSION "(src|include)/integrators/.*")
source_group(textures REGULAR_EXPRESSION "(src|include)/textures/.*")

target_include_directories(YART-Lib PUBLIC 
    src
    include
    vendor/spdlog/include
    vendor/openexr/src/lib/OpenEXR
    vendor/openexr/src/lib/Iex
    vendor/Imath/src/Imath
    ${CMAKE_CURRENT_BINARY_DIR}/vendor/openexr/cmake
    ${CMAKE_CURRENT_BINARY_DIR}/vendor/Imath/config
)

target_precompile_headers(YART-Lib 
    PRIVATE
        <algorithm>
        <cassert>
        <chrono>
        <functional>
        <iostream>
        <memory>
        <utility>
        <cstdlib>
        <array>
        <map>
        <set>
        <sstream>
        <string>
        <unordered_map>
        <unordered_set>
        <vector>
        [["core/assert.h"]]
        [["core/log.h"]]
        [["core/yart.h"]]
        [["math/boundingbox.h"]]
        [["math/matrix.h"]]
        [["math/ray.h"]]
        [["math/transform.h"]]
        [["math/util.h"]]
        [["math/vector.h"]]
)