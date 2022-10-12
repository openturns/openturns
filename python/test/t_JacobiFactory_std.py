#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


jacobi = ot.JacobiFactory(2.5, 3.5, ot.JacobiFactory.PROBABILITY)
print("jacobi=", jacobi)
jacobi = ot.JacobiFactory(2.5, 3.5, ot.JacobiFactory.ANALYSIS)
print("jacobi=", jacobi)
jacobi = ot.JacobiFactory(2.5, 3.5)
print("jacobi=", jacobi)
for i in range(10):
    print("jacobi(", i, ")=", jacobi.build(i))
roots = jacobi.getRoots(10)
print("jacobi(10) roots=", repr(roots))
nodes, weights = jacobi.getNodesAndWeights(10)
print("jacobi(10) nodes=", nodes, "and weights=", weights)
