@echo off
python --version 3>NUL
if errorlevel 1 goto errorNoPython

echo installing requests
python3 -m pip install requests
echo Trying installing vulkan
setupVulkan.py
echo Trying to generate Project Files for Visual Studio 2019
generateProjects.bat
pause
close

goto:eof

:errorNoPython
echo Trying to install python from Microsoft Store
python
echo After installing python execute this file again
pause