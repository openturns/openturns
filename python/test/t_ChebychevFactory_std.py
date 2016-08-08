#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    chebychev = ChebychevFactory()
    print("chebychev=", chebychev)
    for i in range(10):
        print("chebychev(", i, ")=", chebychev.build(i))
    roots = chebychev.getRoots(10)
    print("chebychev(10) roots=", repr(roots))
    nodes, weights = chebychev.getNodesAndWeights(10)
    print("chebychev(10) nodes=", nodes, "and weights=", weights)
except:
    import sys
    print("t_ChebychevFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
