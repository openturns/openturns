#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    resourceMap = ResourceMap.GetInstance()
    print(resourceMap)
    print("Extract from ResourceMap : R-executable-command -> ",
          resourceMap.get("R-executable-command"))
except:
    import sys
    print("t_ResourceMap_std.py", sys.exc_info()[0], sys.exc_info()[1])
