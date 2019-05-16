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
    algo = Fejer1()
    # High-level interface
    value = algo.integrate(f, Interval(a, b))[0]
    ref = math.cos(a) - math.cos(b)
    # Low-level interface
    algo = Fejer1([20])
    value, adaptedNodes = algo.integrateWithNodes(f, Interval(a, b))
    ott.assert_almost_equal(value,ref,1e-4,1e-3)

except:
    import sys
    print("t_Fejer1_std.py", sys.exc_info()[0], sys.exc_info()[1])
