#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    charlier = CharlierFactory(2.5)
    print("charlier=", charlier)
    for i in range(10):
        print("charlier(", i, ")=", charlier.build(i))
    roots = charlier.getRoots(10)
    print("charlier(10) roots=", repr(roots))
    nodes, weights = charlier.getNodesAndWeights(10)
    print("charlier(10) nodes=", nodes, "and weights=", weights)
except:
    import sys
    print("t_CharlierFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
