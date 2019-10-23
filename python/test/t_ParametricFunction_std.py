#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import math as m

f = ot.SymbolicFunction(ot.Description.BuildDefault(5, 'x'),
                        ['sin(x0) + x1 * cos(x2) / exp(x3)',
                         '-2.0 * x0 + x1 * x2^2 + cos(x3)',
                         'x0 / (abs(x1) * x2^2 + x3 + 1.0)'])
pset = [3, 1]  # x3, x1
parametersSet = True
x = [1.0, 2.0, 0.0]  # x0, x2, x4

# with reference point
referencePoint = [0.85] * 2  # x3, x1
g2 = ot.ParametricFunction(f, pset, referencePoint, parametersSet)
print('g2=', g2)
print('g2(x)=', g2(x))

# with complementary parameter set
referencePoint = [1.0, 2.0, 0.0]  # x0, x2, x4
x = [0.85] * 2  # x3, x1
parametersSet = False
g3 = ot.ParametricFunction(f, pset, referencePoint, parametersSet)
print('g3=', g3)
print('g3(x)=', g3(x))

# From ticket #1092
modelFunc = ot.SymbolicFunction(["s", "r", "c"], ["r + c"])
s = [1.]
r = 1e9
c = 1e8
theta = ot.Point([r, c])
calibratedIndices = [1, 2]
mycf = ot.ParametricFunction(modelFunc, calibratedIndices, theta)

print(modelFunc.gradient([s[0], r, c]).transpose())
print(mycf.parameterGradient(s).transpose())
