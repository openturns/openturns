#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    meixner = MeixnerFactory(10, 0.3)
    for i in range(10):
        print("meixner(", i, ")=", meixner.build(i))
    roots = meixner.getRoots(10)
    print("meixner(10) roots=", repr(roots))
    nodes, weights = meixner.getNodesAndWeights(10)
    print("meixner(10) nodes=", nodes, "and weights=", weights)
except:
    import sys
    print("t_MeixnerFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
