rem Simple example that use an OT wrapper.

set OT_PREFIX=C:\Python27\Lib\site-packages\openturns
set PYTHON_PREFIX=%OT_PREFIX%\..\..\..
set OPENTURNS_HOME=%OT_PREFIX%

set OPENTURNS_WRAPPER_PATH=.
PATH=%PATH%;%OT_PREFIX%;%PYTHON_PREFIX%

@rem launch the program
python.exe test.py


@rem sleep for a few seconds
CHOICE /T 10 /C ync /CS /D y
@rem pause
@exit
