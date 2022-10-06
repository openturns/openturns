#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

krawtchouk = ot.KrawtchoukFactory(10, 0.3)
for i in range(10):
    print("krawtchouk(", i, ")=", krawtchouk.build(i))
roots = krawtchouk.getRoots(10)
print("krawtchouk(10) roots=", repr(roots))
nodes, weights = krawtchouk.getNodesAndWeights(10)
print("krawtchouk(10) nodes=", nodes, "and weights=", weights)
