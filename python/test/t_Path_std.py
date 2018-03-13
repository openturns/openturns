#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

try:
    # As the result of these methods is installation dependent, don't check
    # the output
    print('Installation directory=', ot.Path.GetInstallationDirectory())
    configDirectoryList = ot.Path.GetConfigDirectoryList()
    for i in range(len(configDirectoryList)):
        print('configDirectoryList[', i, ']=', configDirectoryList[i])
    # Check that openturns.conf had been installed at the expected location
    ot.Path_FindFileByNameInDirectoryList(
        "openturns.conf", ot.Path_GetConfigDirectoryList())
    list = ot.DirectoryList(2)
    directory1 = ot.Path.CreateTemporaryDirectory('testDirectory1')
    print('Directory 1=', directory1)
    list[0] = directory1
    directory2 = ot.Path.CreateTemporaryDirectory('testDirectory2')
    print('Directory 2=', directory2)
    list[1] = directory2
    # Create a file in dir2
    testFile = open(directory2 + '/testFile', 'w')
    testFile.write('test')
    testFile.close()
    findName = ot.Path.FindFileByNameInDirectoryList('testFile', list)
    print('Find file=', findName)
    fileName = ot.Path.BuildTemporaryFileName('testFile')
    print('Temporary file name=', fileName)
    ot.Path.DeleteTemporaryDirectory(directory1)
    ot.Path.DeleteTemporaryDirectory(directory2)
    print('Library dir=', ot.Path.GetLibraryDirectory())
except:
    import os
    import traceback
    traceback.print_exc()
    os._exit(1)
