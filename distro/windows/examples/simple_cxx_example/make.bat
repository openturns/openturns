rem Simple example to compile a cxx program that use openturns.

set OT_PREFIX="c:\openturns"
set MINGW_PREFIX="%OT_PREFIX%\MinGW"

%MINGW_PREFIX%\bin\g++ -Wl,--enable-auto-import main.cxx  -I%OT_PREFIX%\include  -I%OT_PREFIX%\include\openturns -I%MINGW_PREFIX%\include   -L%OT_PREFIX%  -DHAVE_PTHREAD_H -DPACKAGE_NAME -DPACKAGE_VERSION  -lOT -o simple_example.exe

@rem launch the program
PATH=..\..;%PATH%
simple_example.exe

@rem pause
@rem sleep for a few seconds
@CHOICE /T 5 /C ync /CS /D y
@exit
