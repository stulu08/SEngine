@echo off
pushd %~dp0\..\
call Engine\Stulu\vendor\premake\premake5.exe gmake2
popd
PAUSE