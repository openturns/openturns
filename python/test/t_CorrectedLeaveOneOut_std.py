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
coll = []
coll.append(f)
basis = ot.Basis(coll)
indices = list(range(len(coll)))

fittingAlgo = ot.CorrectedLeaveOneOut()

print("algo =", fittingAlgo)

result = fittingAlgo.run(x, y, w, basis, indices)

print("result = %g" % result)

# illustrate the other usages
proxy = ot.DesignProxy(x, basis)
method = ot.QRMethod(proxy, indices)
# use the other run methods to cover them
result2 = fittingAlgo.run(y, w, indices, proxy)
ott.assert_almost_equal(result, result2, 1e-5, 1e-5)
result3 = fittingAlgo.run(y, indices, proxy)
ott.assert_almost_equal(result, result3, 1e-5, 1e-5)
result4 = fittingAlgo.run(method, y)
ott.assert_almost_equal(result, result4, 1e-5, 1e-5)

# Non-uniform weights: the correction uses the unweighted Gram inverse trace
wNonUniform = ot.Point([0.5 + (i % 4) * 0.25 for i in range(size)])
proxyW = ot.DesignProxy(x, basis)
methodW = ot.QRMethod(proxyW, wNonUniform, indices)
resultW = fittingAlgo.run(methodW, y)
# Independent recomputation through the public API
basisSize = len(indices)
phi = proxyW.computeDesign(indices)
gram = phi.transpose() * phi
invGram = gram.solveLinearSystem(ot.IdentityMatrix(basisSize))
traceInverse = sum(invGram[d, d] for d in range(basisSize))
yFlat = ot.Point([y[s, 0] for s in range(size)])
coefficients = methodW.solve(yFlat)
yHat = phi * coefficients
hDiag = methodW.getHDiag()
wSum = sum(wNonUniform[s] for s in range(size))
wMean = sum(wNonUniform[s] * y[s, 0] for s in range(size)) / wSum
varianceW = sum(wNonUniform[s] * (y[s, 0] - wMean) ** 2 for s in range(size)) / wSum
empiricalError = (
    sum(
        wNonUniform[s] * ((y[s, 0] - yHat[s]) / (1.0 - hDiag[s])) ** 2
        for s in range(size)
    )
    / wSum
)
expected = (
    size / (size - basisSize) * (1.0 + traceInverse) * empiricalError / varianceW
)
ott.assert_almost_equal(resultW, expected, 1e-8, 1e-10)
