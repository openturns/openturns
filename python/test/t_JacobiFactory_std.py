#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    jacobi = JacobiFactory(2.5, 3.5, JacobiFactory.PROBABILITY)
    print("jacobi=", jacobi)
    jacobi = JacobiFactory(2.5, 3.5, JacobiFactory.ANALYSIS)
    print("jacobi=", jacobi)
    jacobi = JacobiFactory(2.5, 3.5)
    print("jacobi=", jacobi)
    for i in range(10):
        print("jacobi(", i, ")=", jacobi.build(i))
    roots = jacobi.getRoots(10)
    print("jacobi(10) roots=", repr(roots))
    nodes, weights = jacobi.getNodesAndWeights(10)
    print("jacobi(10) nodes=", nodes, "and weights=", weights)
except:
    import sys
    print("t_JacobiFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
