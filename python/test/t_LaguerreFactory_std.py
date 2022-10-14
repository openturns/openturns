#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


laguerre = ot.LaguerreFactory(2.5, ot.LaguerreFactory.PROBABILITY)
print("laguerre=", laguerre)
laguerre = ot.LaguerreFactory(2.5, ot.LaguerreFactory.ANALYSIS)
print("laguerre=", laguerre)
laguerre = ot.LaguerreFactory(2.5)
print("laguerre=", laguerre)
for i in range(10):
    print("laguerre(", i, ")=", laguerre.build(i))
roots = laguerre.getRoots(10)
print("laguerre(10) roots=", repr(roots))
nodes, weights = laguerre.getNodesAndWeights(10)
print("laguerre(10) nodes=", nodes, "and weights=", weights)
