SET mypath=%~dp0
cd %mypath:~0,-1%

pause
python python/clean.py
PAUSE
exit