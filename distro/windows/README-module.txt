How to use OpenTURNS Python module on Windows.


1. users

Users interact with OpenTURNS through Python script.

  - use
To launch OpenTURNS, type "import openturns" in a Python console. 
No environment variable need to be set.

  - examples
Python examples can be found in C:\Python27\Lib\site-packages\openturns\examples directory.
Start them by dragging .py to _drag_to_me_to_start_it.bat batch file.

An example of OpenTURNS Python script that wrap an external app in parallel can be found here: 
C:\Python27\Lib\site-packages\openturns\examples\WrapperTemplates\wrapper_python_distributed .
Start it using start_it.bat file.


2. developers

Developpers interact with OpenTURNS by linking c++ program or wrapper against OpenTURNS libs.

  - compile
If you are ready to compile against OpenTURNS library, you can use openturns-developer installer.
It provides the supported MinGW development environment, an example of wrapper compilable on 
Windows :
  C:\Python27\Lib\site-packages\openturns\examples\WrapperTemplates\mingw_wrapper_calling_shell_command
and an example of application using OpenTURNS :
  C:\Python27\Lib\site-packages\openturns\examples\simple_cxx_example
The OpenTURNS_WindowsPortGuide explains how to compile this. This documentation can be found on
OpenTURNS website.
