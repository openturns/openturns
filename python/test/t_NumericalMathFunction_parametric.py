#! /usr/bin/env python

from __future__ import print_function
from openturns import *
import math as m

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

f = NumericalMathFunction(
    Description.BuildDefault(4, 'x'), Description.BuildDefault(3, 'y'),
                          ['sin(x0) + x1 * cos(x2) / exp(x3)', '-2.0 * x0 + x1 * x2^2 + cos(x3)', 'x0 / (abs(x1) * x2^2 + x3 + 1.0)'])
pset = [3, 1]
parametersSet = True
g = NumericalMathFunction(f, pset, parametersSet)
print('g=', g)
x = [1.0, 2.0]
print('g(x)=', g(x))
