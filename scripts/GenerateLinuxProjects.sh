pushd "$(dirname "$0")/.."
vendor/linux/premake5/premake5 --os=linux gmake2
popd