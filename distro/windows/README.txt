How to use OpenTURNS on Windows.


1. users

Users interact with OpenTURNS through python script.

  - use
To launch OpenTURNS, use the script pyot.bat. OpenTURNS cannot be launched directly from Python
executable because it needs to have some environment variables set.
How to use pyot.bat :
. from DOS : C:\OpenTURNS\bin\pyot.bat ot_example.py
. from explorer : right click on ot_example.py, choose "open from...", "Browse...", select
  pyot.bat (in C:\OpenTURNS\bin directory).

  - examples
Python examples can be found in C:\OpenTURNS\share\openturns\examples directory.

An example of OpenTURNS Python script that wrap an external app in parallel can be found here: 
C:\OpenTURNS\share\openturns\WrapperTemplates\wrapper_python_distributed .

  - install custom python modules
The following modules are already present in OpenTURNS installation: matplotlib, scipy, numpy, ipython.
To install your own python modules (e.g. python-pp) in the python packaged by OpenTURNS (C:\OpenTURNS\opt\Python27),
the module installer will search the python installation path in the registry. By default,
OpenTURNS do not put this information in registry in order to not overwrite existing python 2.7
installation.
The following registry script permit to add/overwrite this information to the registry. To execute
it, double click on it: C:\OpenTURNS\share\openturns\tools\add-python27-toregistry.reg
If you are not administrator of your PC replace in add-python27-toregistry.reg every occurence of
HKEY_LOCAL_MACHINE by HKEY_CURRENT_USER. This script has been only tested on Windows XP.


2. developers

Developpers interact with OpenTURNS by linking c++ program or wrapper against OpenTURNS libs.

  - compile
If you are ready to compile against OpenTURNS library, you can use openturns-developer installer.
It provides the supported MinGW development environment,
an example of wrapper compilable on Windows (C:\OpenTURNS\share\openturns\WrapperTemplates\mingw_wrapper_calling_shell_command)
and an example of application using OpenTURNS (C:\OpenTURNS\share\openturns\examples\simple_cxx_example).
The OpenTURNS_WindowsPortGuide explains how to compile this. This documentation can be found on
OpenTURNS website.

  - launch
To see which env var OpenTURNS needs, have a look to otprog.bat or pyot.bat in C:\OpenTURNS\bin directory.


