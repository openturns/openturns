#! /usr/bin/env python

import openturns as ot
import math

ot.TESTPREAMBLE()

f = ot.SymbolicFunction(["x"], ["sin(x)"])
a = -2.5
b = 4.5
# Default parameters
algo = ot.GaussLegendre()
print("Algo=", algo)
# High-level interface
value, adaptedNodes = algo.integrateWithNodes(f, ot.Interval(a, b))
ref = math.cos(a) - math.cos(b)
print("value=%.6f" % value[0], ", ref=%.6f" % ref, ", adaptedNodes=", adaptedNodes)
# Low-level interface
algo = ot.GaussLegendre([20])
value, adaptedNodes = algo.integrateWithNodes(f, ot.Interval(a, b))
print("value=%.6f" % value[0], ", ref=%.6f" % ref, ", adaptedNodes=", adaptedNodes)
