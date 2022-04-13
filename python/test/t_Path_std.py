#! /usr/bin/env python

import openturns as ot

# As the result of these methods is installation dependent, don't check
# the output
print('Installation directory=', ot.Path.GetInstallationDirectory())
configDirectoryList = ot.Path.GetConfigDirectoryList()
for i in range(len(configDirectoryList)):
    print('configDirectoryList[', i, ']=', configDirectoryList[i])
