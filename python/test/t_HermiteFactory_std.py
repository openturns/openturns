#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


hermite = ot.HermiteFactory()
print("hermite=", hermite)
for i in range(10):
    print("hermite(", i, ")=", hermite.build(i))
roots = hermite.getRoots(10)
print("hermite(10) roots=", repr(roots))
nodes, weights = hermite.getNodesAndWeights(10)
print("hermite(10) nodes=", nodes, "and weights=", weights)
