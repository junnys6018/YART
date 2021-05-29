pushd %~dp0..
call vendor\windows\premake5\premake5.exe --os=windows vs2019
popd