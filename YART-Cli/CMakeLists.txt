file(GLOB_RECURSE SOURCES src/*)

add_executable(YART-Cli ${SOURCES})

target_include_directories(YART-Cli PUBLIC ../YART-Lib/include)
target_link_libraries(YART-Cli PUBLIC YART-Lib)

target_precompile_headers(YART-Cli 
    PRIVATE
        <algorithm>
        <cassert>
        <chrono>
        <functional>
        <iostream>
        <memory>
        <utility>
        <cmath>
        <array>
        <map>
        <set>
        <sstream>
        <string>
        <unordered_map>
        <unordered_set>
        <vector>
)