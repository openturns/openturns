#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # As the result of these methods is installation dependent, don't check
    # the output
    print("Installation directory=", Path.GetInstallationDirectory())
    wrapperDirectoryList = Path.GetWrapperDirectoryList()
    for i in range(len(wrapperDirectoryList)):
        print("wrapperDirectoryList[", i, "]=", wrapperDirectoryList[i])
    configDirectoryList = Path.GetConfigDirectoryList()
    for i in range(len(configDirectoryList)):
        print("configDirectoryList[", i, "]=", configDirectoryList[i])
    list = DirectoryList(2)
    directory1 = Path.CreateTemporaryDirectory("testDirectory1")
    print("Directory 1=", directory1)
    list[0] = directory1
    directory2 = Path.CreateTemporaryDirectory("testDirectory2")
    print("Directory 2=", directory2)
    list[1] = directory2
    # Create a file in dir2
    testFile = open(directory2 + "/testFile", "w")
    testFile.write("test")
    testFile.close()
    findName = Path.FindFileByNameInDirectoryList("testFile", list)
    print("Find file=", findName)
    fileName = Path.BuildTemporaryFileName("testFile")
    print("Temporary file name=", fileName)
    Path.DeleteTemporaryDirectory(directory1)
    Path.DeleteTemporaryDirectory(directory2)

except:
    import sys
    print("t_Path_std.py", sys.exc_info()[0], sys.exc_info()[1])
