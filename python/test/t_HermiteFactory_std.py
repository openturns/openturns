#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    hermite = HermiteFactory()
    print("hermite=", hermite)
    for i in range(10):
        print("hermite(", i, ")=", hermite.build(i))
    roots = hermite.getRoots(10)
    print("hermite(10) roots=", repr(roots))
    nodes, weights = hermite.getNodesAndWeights(10)
    print("hermite(10) nodes=", nodes, "and weights=", weights)
except:
    import sys
    print("t_HermiteFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
