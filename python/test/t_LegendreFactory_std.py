#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

legendre = ot.LegendreFactory()
print("legendre=", legendre)
for i in range(10):
    print("legendre(", i, ")=", legendre.build(i))
roots = legendre.getRoots(10)
print("legendre(10) roots=", repr(roots))
nodes, weights = legendre.getNodesAndWeights(10)
print("legendre(10) nodes=", nodes, "and weights=", weights)
