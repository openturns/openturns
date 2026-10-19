#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m
import os as _os

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
ySmall = ot.Point([1.0, 2.0, 4.0])
gramRhs = designUnder.transpose() * ySmall
methodUnder = ot.SVDMethod(proxyUnder, fullUnder)
xGram = methodUnder.solveNormalGram(gramRhs)
ott.assert_almost_equal(xGram.getSize(), largeBasisSize)
assert all(m.isfinite(v) for v in xGram)
xNormal = methodUnder.solveNormal(gramRhs)
ott.assert_almost_equal(xNormal, xGram)
with ott.assert_raises(TypeError):
    methodUnder.solveNormalGram(ot.Point(largeBasisSize + 1))
print("underdetermined solveNormalGram: OK")

# Extra CholeskyMethod coverage (no output to preserve expout)
Xc = ot.Sample(sampleSize, 1)
for i in range(sampleSize):
    Xc[i, 0] = i + 1.0
proxyC = ot.DesignProxy(Xc, phis)
chol = ot.CholeskyMethod(proxyC, list(full))
assert len(repr(chol)) > 0
# constructor with explicit weights and with a design matrix
cholW = ot.CholeskyMethod(proxyC, [1.0] * sampleSize, list(full))
cholM = ot.CholeskyMethod(ot.Matrix(proxyC.computeDesign(list(full))))
ott.assert_almost_equal(cholW.solve(y), chol.solve(y), 1e-12, 1e-12)
ott.assert_almost_equal(cholM.solve(y), chol.solve(y), 1e-12, 1e-12)
# solveNormal / solveNormalGram error paths
with ott.assert_raises(Exception):
    chol.solveNormal(ot.Point([1.0, 2.0]))
with ott.assert_raises(Exception):
    chol.solveNormalGram(ot.Point([1.0]))
cholNonUniform = ot.CholeskyMethod(proxyC, [1.0, 2.0, 3.0, 4.0, 5.0], list(full))
with ott.assert_raises(Exception):
    cholNonUniform.solveNormal(ot.Point([1.0, 2.0, 3.0]))
with ott.assert_raises(Exception):
    cholNonUniform.solve([1.0, 2.0])
# Gram matrix and hat matrix accessors
assert chol.getH().getNbRows() == sampleSize
assert chol.getGramInverse().getDimension() == basisSize
# row update with conserved rows then trash/recompute
chol.update([0], list(full), [1], True)
ott.assert_almost_equal(chol.solve(y).getSize(), basisSize)
chol.trashDecomposition()
ott.assert_almost_equal(chol.solve(y).getSize(), basisSize)
# column update with removed indices is not implemented
with ott.assert_raises(Exception):
    chol.update([0], [1, 2], [0], False)
# incremental large-case column update
old_large_case = ot.ResourceMap.GetAsUnsignedInteger("CholeskyMethod-LargeCase")
ot.ResourceMap.SetAsUnsignedInteger("CholeskyMethod-LargeCase", 2)
try:
    chol.update([2], [0, 1], [], False)
    ott.assert_almost_equal(chol.solve(y).getSize(), basisSize)
finally:
    ot.ResourceMap.SetAsUnsignedInteger("CholeskyMethod-LargeCase", old_large_case)
# save / load roundtrip
study_chol = ot.Study()
study_chol.setStorageManager(ot.XMLStorageManager("chol_test.xml"))
study_chol.add("chol", chol)
study_chol.save()
study_chol2 = ot.Study()
study_chol2.setStorageManager(ot.XMLStorageManager("chol_test.xml"))
try:
    study_chol2.load()
    chol_loaded = ot.CholeskyMethod()
    study_chol2.fillObject("chol", chol_loaded)
    ott.assert_almost_equal(chol_loaded.solve(y), chol.solve(y), 1e-12, 1e-12)
finally:
    _os.remove("chol_test.xml")
