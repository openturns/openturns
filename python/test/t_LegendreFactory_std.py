#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    legendre = LegendreFactory()
    print("legendre=", legendre)
    for i in range(10):
        print("legendre(", i, ")=", legendre.build(i))
    roots = legendre.getRoots(10)
    print("legendre(10) roots=", repr(roots))
    weights = Point()
    nodes, weights = legendre.getNodesAndWeights(10)
    print("legendre(10) nodes=", nodes, "and weights=", weights)
except:
    import sys
    print("t_LegendreFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
