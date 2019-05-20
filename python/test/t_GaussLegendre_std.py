#! /usr/bin/env python

from __future__ import print_function
from openturns import *
import math

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    f = SymbolicFunction(["x"], ["sin(x)"])
    a = -2.5
    b = 4.5
    # Default parameters
    algo = GaussLegendre()
    print("Algo=", algo)
    # High-level interface
    value, adaptedNodes = algo.integrateWithNodes(f, Interval(a, b))
    ref = math.cos(a) - math.cos(b)
    print("value=%.6f" %
          value[0], ", ref=%.6f" % ref, ", adaptedNodes=", adaptedNodes)
    # Low-level interface
    algo = GaussLegendre([20])
    value, adaptedNodes = algo.integrateWithNodes(f, Interval(a, b))
    print("value=%.6f" %
          value[0], ", ref=%.6f" % ref, ", adaptedNodes=", adaptedNodes)

except:
    import sys
    print("t_GaussKronrod_std.py", sys.exc_info()[0], sys.exc_info()[1])
