@echo off
rem Start a python shell that will be able to use OpenTURNS.
rem First argument of pyot.bat will be given to python (e.g. a python file).


rem Set env var for openturns
set OPENTURNS_HOME=C:\OpenTURNS
set PYTHON_HOME=%OPENTURNS_HOME%\opt\Python27
set PATH=%PYTHON_HOME%;%PATH%
set PATH=%OPENTURNS_HOME%\opt\gs8.64\bin;%PATH%
set PATH=%OPENTURNS_HOME%\opt\R-2.12.0\bin;%PATH%
set PATH=%OPENTURNS_HOME%\bin;%PATH%
set PATH=%OPENTURNS_HOME%\lib\bin;%PATH%
set PATH=%OPENTURNS_HOME%\lib\openturns\module\lib\bin;%PATH%
set PYTHONPATH=%PYTHON_HOME%\Lib;%PYTHONPATH%
set PYTHONPATH=%PYTHON_HOME%\Lib\site-packages;%PYTHONPATH%
set PYTHONPATH=%OPENTURNS_HOME%\lib\python27\site-packages;%PYTHONPATH%
set OPENTURNS_CONFIG_PATH=%OPENTURNS_HOME%\etc\openturns
rem Set OpenTURNS verbosity:
rem set OPENTURNS_LOG_SEVERITY=ALL


rem Start python
rem "%PYTHON_HOME%"\python.exe" %1
"%PYTHON_HOME%\Scripts\ipython.exe" %1 %2 %3

pause

