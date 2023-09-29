@echo off
if not "%1"=="am_admin" (
    powershell -Command "Start-Process -Verb RunAs -FilePath '%0' -ArgumentList 'am_admin', '%~dp0'"

    exit /b
)

pushd %2

python --version 3>NUL
if errorlevel 1 goto errorNoPython

python python/setup.py
PAUSE
exit


:errorNoPython
echo Trying to install python from Microsoft Store
python
echo After installing python execute this file again
pause
exit