rem How to use this script that install python + OT + ... automatically:
rem 1. comment out any already installed program.
rem 2. fetch every .msi or .exe needed:
rem http://www.python.org/ftp/python/2.7.3/python-2.7.3.msi
set PYTHON_SETUP=python-2.7.3.msi
rem http://peak.telecommunity.com/dist/ez_setup.py
set SETUPTOOLS_SETUP=ez_setup.py
rem http://pypi.python.org/packages/any/p/pyreadline/pyreadline-1.7.1.win32.exe
set PYREADLINE_SETUP=pyreadline-1.7.1.win32.exe
rem http://pypi.python.org/packages/2.7/i/ipython/ipython-0.13.1-py2.7.egg
set IPYTHON_SETUP=ipython-0.13.1-py2.7.egg
rem http://pypi.python.org/packages/2.7/n/numpy/numpy-1.6.2.win32-py2.7.exe
set NUMPY_SETUP=numpy-1.6.2.win32-py2.7.exe
rem http://sourceforge.net/projects/scipy/files/scipy/0.11.0/scipy-0.11.0-win32-superpack-python2.7.exe/download
set SCIPY_SETUP=scipy-0.11.0-win32-superpack-python2.7.exe
rem https://github.com/downloads/matplotlib/matplotlib/matplotlib-1.2.0.win32-py2.7.exe
set MATPLOTLIB_SETUP=matplotlib-1.2.0.win32-py2.7.exe
rem http://sourceforge.net/projects/openturns/files/openturns/openturns-1.0-based-devel/openturns-1.0-based-devel-py27.exe/download
set OT_SETUP=openturns-1.0-based-devel-py27.exe
rem 3. start this .bat script

rem install dir
set PYTHON_DIR=C:\Python27

rem python
rem doc at http://www.python.org/download/releases/2.5/msi/
rem ALLUSERS=1
rem TARGETDIR=c:\Python273
msiexec /i %PYTHON_SETUP% /qn TARGETDIR="%PYTHON_DIR%"

rem setuptools
rem download ez_setup.py from http://peak.telecommunity.com/dist/ez_setup.py
rem Run "ez_setup.py -U setuptools" to reinstall or upgrade
"%PYTHON_DIR%\python.exe" %SETUPTOOLS_SETUP%
set EASY_INSTALL="%PYTHON_DIR%\Scripts\easy_install.exe"

rem ipython
%EASY_INSTALL% %PYREADLINE_SETUP%
rem %EASY_INSTALL% pyreadline
%EASY_INSTALL% %IPYTHON_SETUP%
rem %EASY_INSTALL% ipython

rem numpy, scipy
%EASY_INSTALL% %NUMPY_SETUP%
%EASY_INSTALL% %SCIPY_SETUP%

rem matplotlib
%EASY_INSTALL% %MATPLOTLIB_SETUP%


rem finally openturns :-)
%OT_SETUP% /S


pause


rem uninstall previous install:
rem msiexec /x python-2.7.3.msi /qn
rem C:\Python27\Lib\site-packages\openturns\uninst.exe /S
rem rmdir C:\Python27\ /S /Q
