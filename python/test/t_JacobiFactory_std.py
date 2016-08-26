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
    iMax = 10
    for i in range(iMax):
        print("jacobi(", i, ")=", jacobi.build(i))
    distribution = jacobi.getMeasure()
    M = SymmetricMatrix(iMax)
    for i in range(iMax):
        pI = jacobi.build(i)
        for j in range(i + 1):
            pJ = jacobi.build(j)

            def kernel(x):
                return [pI(x[0]) * pJ(x[0]) * distribution.computePDF(x)]

            M[i, j] = GaussKronrod().integrate(
                PythonFunction(1, 1, kernel), distribution.getRange())[0]
    print("M=\n", M.clean(1.0e-6))
    roots = jacobi.getRoots(iMax)
    print("jacobi(10) roots=", repr(roots))
    nodes, weights = jacobi.getNodesAndWeights(iMax)
    print("jacobi(10) nodes=", nodes, "and weights=", weights)
except:
    import sys
    print("t_JacobiFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
