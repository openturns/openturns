#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

# ot.Log.Show(ot.Log.ALL)

basisSize = 3
sampleSize = 5

X = ot.Sample(sampleSize, 1)
for i in range(sampleSize):
    X[i, 0] = i + 1.0

phis = []
for j in range(basisSize):
    phis.append(ot.SymbolicFunction(["x"], ["x^" + str(j + 1)]))
basis = ot.Basis(phis)
for i in range(basisSize):
    print(ot.FunctionCollection(basis)[i](X))

proxy = ot.DesignProxy(X, phis)
full = range(basisSize)

design = ot.Matrix(proxy.computeDesign(full))
print("design=", design)

methods = [
    ot.SVDMethod(proxy, full),
    ot.CholeskyMethod(proxy, full),
    ot.QRMethod(proxy, full),
    ot.SparseMethod(ot.QRMethod(proxy, full)),
]

y = ot.Normal(
    [1.0] * sampleSize, [0.1] * sampleSize, ot.CorrelationMatrix(sampleSize)
).getRealization()
yAt = design.transpose() * y

for method in methods:
    name = method.__class__.__name__
    print("-- method:", name)

    x = method.solve(y)
    print("solve:", x)

    residual = m.sqrt((design * x - y).normSquare()) / sampleSize
    print("residual: %.6g" % residual)

    try:
        x = method.solveNormal(yAt)
        print("solveNormal:", x)
    except RuntimeError:
        print("-no solveNormal-")

    d = method.getHDiag()
    print("getHDiag:", d)

    d = method.getGramInverse()
    print("getGramInverse:", d)

    d = method.getGramInverseDiag()
    print("getGramInverseDiag:", d)

    d = method.getGramInverseTrace()
    print("getGramInverseTrace: %.5g" % d)

proxy = ot.DesignProxy(design)

for method in methods:
    name = method.__class__.__name__
    print("-- method:", name)

    x = method.solve(y)
    print("solve:", x)

    residual = m.sqrt((design * x - y).normSquare()) / sampleSize
    print("residual: %.6g" % residual)

    try:
        x = method.solveNormal(yAt)
        print("solveNormal:", x)
    except RuntimeError:
        print("-no solveNormal-")

    d = method.getHDiag()
    print("getHDiag:", d)

    d = method.getGramInverse()
    print("getGramInverse:", d)

    d = method.getGramInverseDiag()
    print("getGramInverseDiag:", d)

    d = method.getGramInverseTrace()
    print("getGramInverseTrace: %.5g" % d)

# Regression test: solveNormal/solveNormalGram on an underdetermined design
# (more basis functions than sample points) must return the minimum-norm
# solution instead of reading past the singular values
smallSampleSize = 3
largeBasisSize = 5
Xsmall = ot.Sample(smallSampleSize, 1)
for i in range(smallSampleSize):
    Xsmall[i, 0] = i + 1.0
phisLarge = []
for j in range(largeBasisSize):
    phisLarge.append(ot.SymbolicFunction(["x"], ["x^" + str(j)]))
proxyUnder = ot.DesignProxy(Xsmall, phisLarge)
fullUnder = range(largeBasisSize)
designUnder = ot.Matrix(proxyUnder.computeDesign(fullUnder))
ySmall = ot.Sample([[1.0], [2.0], [4.0]])
gramRhs = designUnder.transpose() * ySmall
methodUnder = ot.SVDMethod(proxyUnder, fullUnder)
xGram = methodUnder.solveNormalGram(gramRhs)
print("underdetermined solveNormalGram:", xGram)
ott.assert_almost_equal(xGram.getSize(), largeBasisSize)
assert all(m.isfinite(v) for v in xGram)
xNormal = methodUnder.solveNormal(gramRhs)
ott.assert_almost_equal(xNormal, xGram)
with ott.assert_raises(TypeError):
    methodUnder.solveNormalGram(ot.Point(largeBasisSize + 1))
print("underdetermined solveNormalGram: OK")
