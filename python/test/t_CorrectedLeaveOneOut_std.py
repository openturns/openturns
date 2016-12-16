#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

size = 100
xuniform = ot.Uniform(0.9, 1.1)
x = xuniform.getSample(size)
yuniform = ot.Uniform(1.9, 2.1)
y = yuniform.getSample(size)
w = [1.0] * size
f = ot.AnalyticalFunction(['x'], ['2.0*x'])
coll = []
coll.append(f)
basis = ot.Basis(coll)
indices = range(len(coll))

fittingAlgo = ot.CorrectedLeaveOneOut()
print('algo =', fittingAlgo)

result = fittingAlgo.run(x, y, w, basis, indices)
print('result = %g' % result)
