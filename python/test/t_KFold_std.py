#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

size = 100
xuniform = ot.Uniform(0.9, 1.1)
x = xuniform.getSample(size)
yuniform = ot.Uniform(1.9, 2.1)
y = yuniform.getSample(size)
w = [1.0] * size
f = ot.SymbolicFunction(["x"], ["2.0*x"])
basis = [f]
indices = list(range(len(basis)))

fittingAlgo = ot.KFold()

print("algo =", fittingAlgo)

result = fittingAlgo.run(x, y, w, basis, indices)

print("result = %g" % result)
# illustrate the other usages
proxy = ot.DesignProxy(x, basis)
method = ot.SVDMethod(proxy, indices)
# use the other run methods to cover them
result2 = fittingAlgo.run(y, w, indices, proxy)
ott.assert_almost_equal(result, result2, 1e-5, 1e-5)
result3 = fittingAlgo.run(y, indices, proxy)
ott.assert_almost_equal(result, result3, 1e-5, 1e-5)
result4 = fittingAlgo.run(method, y)
ott.assert_almost_equal(result, result4, 1e-5, 1e-5)
