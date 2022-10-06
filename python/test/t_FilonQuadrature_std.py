#! /usr/bin/env python

import openturns as ot
import math

ot.TESTPREAMBLE()

f = ot.SymbolicFunction(['t'], ['log(1+t)'])
a = 0.5
b = 0.5 + 8.0 * math.pi
n = 100
omega = 1000.0
kind = 0
algoF = ot.FilonQuadrature(n, omega, kind)
value = algoF.integrate(f, ot.Interval(a, b))
print(value)
kind = 1
algoF = ot.FilonQuadrature(n, omega, kind)
value = algoF.integrate(f, ot.Interval(a, b))
print(value)
kind = 2
algoF = ot.FilonQuadrature(n, omega, kind)
value = algoF.integrate(f, ot.Interval(a, b))
print(value)
