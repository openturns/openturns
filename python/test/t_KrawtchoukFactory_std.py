#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    krawtchouk = KrawtchoukFactory(10, 0.3)
    for i in range(10):
        print("krawtchouk(", i, ")=", krawtchouk.build(i))
    roots = krawtchouk.getRoots(10)
    print("krawtchouk(10) roots=", repr(roots))
    nodes, weights = krawtchouk.getNodesAndWeights(10)
    print("krawtchouk(10) nodes=", nodes, "and weights=", weights)
except:
    import sys
    print("t_KrawtchoukFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
