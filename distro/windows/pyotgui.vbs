' Start a python shell that will be able to use OpenTURNS.


' OpenTURNS prefix
dim OPENTURNS_HOME
' adapt the following line if OpenTURNS is not installed in C:\OpenTURNS
OPENTURNS_HOME = "C:\OpenTURNS"

' Use Process vars : Applies to current process and might be passed to child processes
Set wshShell = CreateObject("WScript.Shell")
Set wshEnv = wshShell.Environment("Process")

' Set required env vars
dim PYTHON_HOME
PYTHON_HOME = OPENTURNS_HOME & "\opt\Python27"
wshEnv("PATH") = PYTHON_HOME & ";" & wshEnv("PATH")
wshEnv("PATH") = OPENTURNS_HOME & "\opt\gs8.64\bin;" & wshEnv("PATH")
wshEnv("PATH") = OPENTURNS_HOME & "\opt\R-2.12.0\bin;" & wshEnv("PATH")
wshEnv("PATH") = OPENTURNS_HOME & "\bin;" & wshEnv("PATH")
wshEnv("PATH") = OPENTURNS_HOME & "\lib\bin;" & wshEnv("PATH")
wshEnv("PATH") = OPENTURNS_HOME & "\lib\openturns\module\lib\bin;" & wshEnv("PATH")
wshEnv("PYTHONPATH") = PYTHON_HOME & "\Lib;" & wshEnv("PYTHONPATH")
wshEnv("PYTHONPATH") = PYTHON_HOME & "\Lib\site-packages;" & wshEnv("PYTHONPATH")
wshEnv("PYTHONPATH") = OPENTURNS_HOME & "\lib\python27\site-packages;" & wshEnv("PYTHONPATH")
wshEnv("OPENTURNS_CONFIG_PATH") = OPENTURNS_HOME & "\etc\openturns"
wshEnv("OPENTURNS_MODULE_PATH") = OPENTURNS_HOME & "\lib\openturns\module;" & wshEnv("OPENTURNS_MODULE_PATH")
'WScript.Echo "PATH=" & wshEnv("PATH")
' Debug OpenTURNS
'wshEnv("OPENTURNS_LOG_SEVERITY") = "ALL"

' Launch OpenTURNS
wshShell.run """" & PYTHON_HOME & "\Scripts\ipython.exe"""


