rem Simple example to compile an OT wrapper.

set OT_PREFIX=C:\Python27\Lib\site-packages\openturns
set MINGW_PREFIX=%OT_PREFIX%\MinGW

@rem compile wrapper
%MINGW_PREFIX%\bin\gcc -Wl,--enable-auto-import wrapper.c  -I%OT_PREFIX%\include  -I%OT_PREFIX%\include\openturns -I%MINGW_PREFIX%\include  -L%OT_PREFIX% -DHAVE_PTHREAD_H -DPACKAGE_NAME -DPACKAGE_VERSION  -shared  -lpthreadGC2 -lOT -lOTbind -o wcode.dll

@rem sleep for a few seconds
@CHOICE /T 5 /C ync /CS /D y
@rem pause
@exit
