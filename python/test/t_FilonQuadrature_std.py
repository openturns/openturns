#! /usr/bin/env python

from __future__ import print_function
from openturns import *
import math

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    f = SymbolicFunction(['t'], ['log(1+t)'])
    a = 0.5
    b = 0.5 + 8.0 * math.pi
    n = 100
    omega = 1000.0
    kind = 0
    algoF = FilonQuadrature(n, omega, kind)
    value = algoF.integrate(f, Interval(a, b))
    print(value)
    kind = 1
    algoF = FilonQuadrature(n, omega, kind)
    value = algoF.integrate(f, Interval(a, b))
    print(value)
    kind = 2
    algoF = FilonQuadrature(n, omega, kind)
    value = algoF.integrate(f, Interval(a, b))
    print(value)

except:
    import sys
    print("t_FilonQuadrature_std.py", sys.exc_info()[0], sys.exc_info()[1])
