pushd "$(dirname "$0")/.."
find YART-Lib/src/ -name "*.cpp" -exec clang-format -i  {} \+
find YART-Lib/include/ -name "*.h" -exec clang-format -i  {} \+
find YART-Cli/src/ -name "*.h" -exec clang-format -i  {} \+
find YART-Cli/src/ -name "*.cpp" -exec clang-format -i  {} \+
find YART-Test/src/ -name "*.h" -exec clang-format -i  {} \+
find YART-Test/src/ -name "*.cpp" -exec clang-format -i  {} \+
popd