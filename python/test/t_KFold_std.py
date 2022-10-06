#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

size = 100
xuniform = ot.Uniform(0.9, 1.1)
x = xuniform.getSample(size)
yuniform = ot.Uniform(1.9, 2.1)
y = yuniform.getSample(size)
w = [1.0] * size
f = ot.SymbolicFunction(['x'], ['2.0*x'])
basis = [f]
indices = list(range(len(basis)))

fittingAlgo = ot.KFold()

print("algo =", fittingAlgo)

result = fittingAlgo.run(x, y, w, basis, indices)

print("result = %g" % result)

