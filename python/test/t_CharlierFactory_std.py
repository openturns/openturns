#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

charlier = ot.CharlierFactory(2.5)
print("charlier=", charlier)
for i in range(10):
    print("charlier(", i, ")=", charlier.build(i))
roots = charlier.getRoots(10)
print("charlier(10) roots=", repr(roots))
nodes, weights = charlier.getNodesAndWeights(10)
print("charlier(10) nodes=", nodes, "and weights=", weights)
