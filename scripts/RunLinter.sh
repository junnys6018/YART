#!/bin/bash
pushd "$(dirname "$0")/.."
vendor/linux/premake5/premake5 /systemscript=vendor/premake_modules/export-compile-commands.lua export-compile-commands
cp compile_commands/release.json compile_commands.json
for project in YART-Cli/src YART-Lib/src YART-Lib/include YART-Test/src
do
	clang-tidy --quiet -p . --checks=-clang-diagnostic-*,cppcoreguidelines-*,clang-analyzer-*,-cppcoreguidelines-avoid-magic-numbers,-cppcoreguidelines-pro-bounds-* $(find $project -name *.cpp) $(find $project -name *.h)
done

# clean up
rm -rf compile_commands compile_commands.json

popd