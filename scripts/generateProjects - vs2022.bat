@echo off
pushd %~dp0\..\
call Engine\Stulu\vendor\premake\premake5.exe vs2022
popd
PAUSE