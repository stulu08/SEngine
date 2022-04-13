FOR /F "tokens=*" %%g IN ('"%1\tools\vswhere.exe" -latest -prerelease -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe') do (SET VAR=%%g)
%VAR% %1/Assembly.sln /p:Configuration=Release

