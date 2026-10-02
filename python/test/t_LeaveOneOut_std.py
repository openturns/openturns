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

fittingAlgo = ot.LeaveOneOut()

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

# Uniform weights: CorrectedLeaveOneOut is LeaveOneOut times its correction,
# up to the legacy variance convention: CLOO normalizes by the unbiased
# variance (n - 1) while LeaveOneOut normalizes by the weight mass (n)
basisSize = len(indices)
phi = proxy.computeDesign(indices)
gram = phi.transpose() * phi
invGram = gram.solveLinearSystem(ot.IdentityMatrix(basisSize))
traceInverse = sum(invGram[d, d] for d in range(basisSize))
cloo = ot.CorrectedLeaveOneOut()
resultCLOO = cloo.run(method, y)
ott.assert_almost_equal(
    resultCLOO,
    size / (size - basisSize) * (1.0 + traceInverse) * result * (size - 1) / size,
    1e-8,
    1e-10,
)

# Non-uniform weights: independent recomputation through the public API
wNonUniform = ot.Point([0.5 + (i % 4) * 0.25 for i in range(size)])
proxyW = ot.DesignProxy(x, basis)
methodW = ot.QRMethod(proxyW, wNonUniform, indices)
resultW = fittingAlgo.run(methodW, y)
yFlat = ot.Point([y[s, 0] for s in range(size)])
coefficients = methodW.solve(yFlat)
yHat = phi * coefficients
hDiag = methodW.getHDiag()
wSum = sum(wNonUniform[s] for s in range(size))
wMean = sum(wNonUniform[s] * y[s, 0] for s in range(size)) / wSum
varianceW = sum(wNonUniform[s] * (y[s, 0] - wMean) ** 2 for s in range(size)) / wSum
# Leave-one-out residuals through the diagonal of the weighted hat matrix
expected = (
    sum(
        wNonUniform[s] * ((y[s, 0] - yHat[s]) / (1.0 - hDiag[s])) ** 2
        for s in range(size)
    )
    / wSum
    / varianceW
)
ott.assert_almost_equal(resultW, expected, 1e-8, 1e-10)

# Non-uniform weights: brute-force refit check on a small problem
smallSize = 12
xs = ot.Uniform(0.9, 1.1).getSample(smallSize)
ys = f(xs)
ws = ot.Point([0.5 + 0.25 * (i % 4) for i in range(smallSize)])
proxyS = ot.DesignProxy(xs, basis)
methodS = ot.QRMethod(proxyS, ws, indices)
resultS = fittingAlgo.run(methodS, ys)
phiS = proxyS.computeDesign(indices)
wSumS = sum(ws)
press = 0.0
for i in range(smallSize):
    train = ot.Indices([j for j in range(smallSize) if j != i])
    proxyT = ot.DesignProxy(xs[train], basis)
    methodT = ot.QRMethod(proxyT, ot.Point([ws[j] for j in train]), indices)
    coeffT = methodT.solve(ot.Point([ys[j, 0] for j in train]))
    pred = sum(phiS[i, j] * coeffT[j] for j in range(len(indices)))
    press += ws[i] * (ys[i, 0] - pred) ** 2
press /= wSumS
wMeanS = sum(ws[i] * ys[i, 0] for i in range(smallSize)) / wSumS
varianceS = sum(ws[i] * (ys[i, 0] - wMeanS) ** 2 for i in range(smallSize)) / wSumS
ott.assert_almost_equal(resultS, press / varianceS, 1e-8, 1e-10)

# Invalid inputs raise
with ott.assert_raises(TypeError):
    fittingAlgo.run(methodS, ot.Sample(smallSize - 1, 1))
with ott.assert_raises(TypeError):
    badMethod = ot.QRMethod(proxyS, ot.Point(smallSize - 1, 1.0), indices)
    fittingAlgo.run(badMethod, ys)
