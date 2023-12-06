@echo off
FOR /F "tokens=*" %%g IN ('"tools\vswhere.exe" -latest -prerelease -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe') do (SET VAR=%%g)
"%VAR%" %1 /p:Configuration=Release -verbosity:minimal -maxcpucount:2 -nologo