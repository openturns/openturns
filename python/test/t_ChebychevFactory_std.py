#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


chebychev = ot.ChebychevFactory()
print("chebychev=", chebychev)
for i in range(10):
    print("chebychev(", i, ")=", chebychev.build(i))
roots = chebychev.getRoots(10)
print("chebychev(10) roots=", repr(roots))
nodes, weights = chebychev.getNodesAndWeights(10)
print("chebychev(10) nodes=", nodes, "and weights=", weights)
