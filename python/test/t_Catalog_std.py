#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    catalog = Catalog.GetInstance()
    print(catalog)

except:
    import sys
    print("t_Catalog_std.py", sys.exc_info()[0], sys.exc_info()[1])
