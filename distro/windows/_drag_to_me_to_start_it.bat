@echo off
rem start the script
set PYTHON_INSTALL_PATH=C:\Python27
set PATH=%PATH%;%PYTHON_INSTALL_PATH%
rem echo %*
rem C:\OpenTURNS\bin\pyot.bat %1 %2 %3
python.exe  %* 

pause
