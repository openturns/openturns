#! /usr/bin/env python

from __future__ import print_function
from openturns import *
import math
import openturns.testing as ott

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    f = SymbolicFunction(["x"], ["sin(x)"])
    a = -2.5
    b = 4.5
    # Default parameters
    algo = Fejer2()
    print("Algo=", algo)
    # High-level interface
    value = algo.integrate(f, Interval(a, b))[0]
    ref = math.cos(a) - math.cos(b)
    print("value=%.6f" % value, ", ref=%.6f" % ref)
    # Low-level interface
    algo = Fejer2([100])
    value, adaptedNodes = algo.integrateWithNodes(f, Interval(a, b))
    print("value=%.6f" %
          value[0], ", ref=%.6f" % ref, ", adaptedNodes=", adaptedNodes)


    ott.assert_almost_equal(ref,value[0],1e-5,1e-5)

except:
    import sys
    print("t_Fejer2_std.py", sys.exc_info()[0], sys.exc_info()[1])
