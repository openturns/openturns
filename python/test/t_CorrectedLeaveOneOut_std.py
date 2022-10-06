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
coll = []
coll.append(f)
basis = ot.Basis(coll)
indices = list(range(len(coll)))

fittingAlgo = ot.CorrectedLeaveOneOut()

print("algo =", fittingAlgo)

result = fittingAlgo.run(x, y, w, basis, indices)

print("result = %g" % result)

