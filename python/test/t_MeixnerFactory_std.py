#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


meixner = ot.MeixnerFactory(10, 0.3)
for i in range(10):
    print("meixner(", i, ")=", meixner.build(i))
roots = meixner.getRoots(10)
print("meixner(10) roots=", repr(roots))
nodes, weights = meixner.getNodesAndWeights(10)
print("meixner(10) nodes=", nodes, "and weights=", weights)
