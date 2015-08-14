@echo off
rem Launch a program that use openturns lib.
rem First argument of otprog.bat will be executed.


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
set OPENTURNS_MODULE_PATH=%OPENTURNS_HOME%\lib\openturns\module;%OPENTURNS_MODULE_PATH%
rem set OPENTURNS_WRAPPER_PATH=C:\openturns\wrappers
rem Set OpenTURNS verbosity:
rem set OPENTURNS_LOG_SEVERITY=ALL


rem Start prog
%1


