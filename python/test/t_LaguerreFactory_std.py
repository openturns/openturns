#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    laguerre = LaguerreFactory(2.5, LaguerreFactory.PROBABILITY)
    print("laguerre=", laguerre)
    laguerre = LaguerreFactory(2.5, LaguerreFactory.ANALYSIS)
    print("laguerre=", laguerre)
    laguerre = LaguerreFactory(2.5)
    print("laguerre=", laguerre)
    for i in range(10):
        print("laguerre(", i, ")=", laguerre.build(i))
    roots = laguerre.getRoots(10)
    print("laguerre(10) roots=", repr(roots))
    nodes, weights = laguerre.getNodesAndWeights(10)
    print("laguerre(10) nodes=", nodes, "and weights=", weights)
except:
    import sys
    print("t_LaguerreFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
