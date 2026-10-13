#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

mesh = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
cov1D = ot.AbsoluteExponential([1.0])
algo = ot.KarhunenLoeveP1Algorithm(mesh, cov1D, 0.0)
algo.setNbModes(8)
algo.run()
result = algo.getResult()
lambd = result.getEigenvalues()
KLModes = result.getModesAsProcessSample()
print("KL modes=", KLModes)
print("KL eigenvalues=", lambd)
process = ot.GaussianProcess(cov1D, KLModes.getMesh())
coefficients = result.project(process.getSample(10))
print("KL coefficients=", coefficients)
KLFunctions = result.getModes()
print("KL functions=", KLFunctions)
print("KL lift=", result.lift(coefficients[0]))
print("KL lift as field=", result.liftAsField(coefficients[0]))

# check spectra variant
if ot.PlatformInfo.HasFeature("spectra"):
    oldSolver = ot.ResourceMap.GetAsString("KarhunenLoeveP1Algorithm-EigenvaluesSolver")
    ot.ResourceMap.SetAsString("KarhunenLoeveP1Algorithm-EigenvaluesSolver", "SPECTRA")
    try:
        algo.run()
        result2 = algo.getResult()
        lambd2 = result2.getEigenvalues()
        ott.assert_almost_equal(lambd2, lambd)
    finally:
        ot.ResourceMap.SetAsString("KarhunenLoeveP1Algorithm-EigenvaluesSolver", oldSolver)

# check the HODLR storage variant against the DENSE reference
if ot.PlatformInfo.HasFeature("spectra"):
    oldSolver = ot.ResourceMap.GetAsString("KarhunenLoeveP1Algorithm-EigenvaluesSolver")
    oldStorage = ot.ResourceMap.GetAsString("KarhunenLoeveP1Algorithm-CovarianceMatrixStorage")
    ot.ResourceMap.SetAsString("KarhunenLoeveP1Algorithm-EigenvaluesSolver", "SPECTRA")
    ot.ResourceMap.SetAsString("KarhunenLoeveP1Algorithm-CovarianceMatrixStorage", "HODLR")
    try:
        algo.run()
        resultHODLR = algo.getResult()
        lambdHODLR = resultHODLR.getEigenvalues()
        ott.assert_almost_equal(lambdHODLR, lambd2)
    finally:
        ot.ResourceMap.SetAsString("KarhunenLoeveP1Algorithm-EigenvaluesSolver", oldSolver)
        ot.ResourceMap.SetAsString("KarhunenLoeveP1Algorithm-CovarianceMatrixStorage", oldStorage)

R = ot.CorrelationMatrix(2)
R[0, 1] = 0.5
scale = [1.0]
amplitude = [1.0, 2.0]
cov2D = ot.ExponentialModel(scale, amplitude, R)
algo = ot.KarhunenLoeveP1Algorithm(mesh, cov2D, 0.0)
algo.setNbModes(18)
algo.run()
result = algo.getResult()
lambd = result.getEigenvalues()
KLModes = result.getModesAsProcessSample()
print("KL modes=", KLModes)
print("KL eigenvalues=", lambd)
process = ot.GaussianProcess(cov2D, KLModes.getMesh())
coefficients = result.project(process.getSample(10))
print("KL coefficients=", coefficients)
KLFunctions = result.getModes()
print("KL functions=", KLFunctions)
print("KL lift=", result.lift(coefficients[0]))
print("KL lift as field=", result.liftAsField(coefficients[0]))

# 2d input, to check that issue #1660 is solved
mesh = ot.IntervalMesher([4] * 2).build(ot.Interval([-1.2] * 2, [1.0] * 2))
cov2D = ot.AbsoluteExponential([1.0] * 2)
algo = ot.KarhunenLoeveP1Algorithm(mesh, cov2D, 1e-3)
algo.run()
result = algo.getResult()
lambd = result.getEigenvalues()
KLModesPS = result.getModesAsProcessSample()
# The output is hidden due to near-zero nonreproducible values
# print("KL modes (process sample)=", KLModesPS)
KLScaledModesPS = result.getScaledModesAsProcessSample()
# The output is hidden due to near-zero nonreproducible values
# print("KL scaled modes (process sample)=", KLScaledModesPS)
KLModes = result.getModes()
# The output is hidden due to near-zero nonreproducible values
# print("KL modes (functions)=", KLModes)
KLScaledModes = result.getScaledModes()
# The output is hidden due to near-zero nonreproducible values
# print("KL scaled modes (functions)=", KLScaledModes)

# Extra coverage: constructors, accessors, converters, error branches

defaultAlgo = ot.KarhunenLoeveP1Algorithm()
assert defaultAlgo.getMesh().getVerticesNumber() == ot.Mesh().getVerticesNumber()
assert len(repr(defaultAlgo)) > 0
assert len(str(defaultAlgo)) > 0
mesh1D = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
cov1DCheck = ot.AbsoluteExponential([1.0])
algoCheck = ot.KarhunenLoeveP1Algorithm(mesh1D, cov1DCheck, 0.0)
assert algoCheck.getMesh().getVerticesNumber() == mesh1D.getVerticesNumber()
copyAlgo = ot.KarhunenLoeveP1Algorithm(algoCheck)
assert copyAlgo.getMesh().getVerticesNumber() == mesh1D.getVerticesNumber()
assert len(repr(algoCheck)) > 0
assert len(str(algoCheck)) > 0
# Mesh without simplices exercises the empty Gram matrix branch
emptyMesh = ot.Mesh(ot.Sample([[0.0], [1.0]]))
emptyAlgo = ot.KarhunenLoeveP1Algorithm(emptyMesh, cov1DCheck, 0.0)
with ott.assert_raises(Exception):
    emptyAlgo.run()
# Unknown storage / solver configuration branches
# Storage key has enum validation, SetAsString itself must raise
with ott.assert_raises(Exception):
    ot.ResourceMap.SetAsString(
        "KarhunenLoeveP1Algorithm-CovarianceMatrixStorage", "BOGUS"
    )
oldSolverCheck = ot.ResourceMap.GetAsString("KarhunenLoeveP1Algorithm-EigenvaluesSolver")
ot.ResourceMap.SetAsString("KarhunenLoeveP1Algorithm-EigenvaluesSolver", "BOGUS")
try:
    with ott.assert_raises(Exception):
        algoCheck.run()
finally:
    ot.ResourceMap.SetAsString(
        "KarhunenLoeveP1Algorithm-EigenvaluesSolver", oldSolverCheck
    )
oldStorageCheck = ot.ResourceMap.GetAsString("KarhunenLoeveP1Algorithm-CovarianceMatrixStorage")
ot.ResourceMap.SetAsString(
    "KarhunenLoeveP1Algorithm-CovarianceMatrixStorage", "HMAT"
)
try:
    with ott.assert_raises(Exception):
        algoCheck.run()
finally:
    ot.ResourceMap.SetAsString(
        "KarhunenLoeveP1Algorithm-CovarianceMatrixStorage", oldStorageCheck
    )
# Persistence roundtrip
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("t_KarhunenLoeveP1Algorithm_std_tmp.xml"))
study.add("algo", algoCheck)
study.save()
try:
    loadedStudy = ot.Study()
    loadedStudy.setStorageManager(
        ot.XMLStorageManager("t_KarhunenLoeveP1Algorithm_std_tmp.xml")
    )
    loadedStudy.load()
    loadedAlgo = ot.KarhunenLoeveP1Algorithm()
    loadedStudy.fillObject("algo", loadedAlgo)
    assert loadedAlgo.getMesh().getVerticesNumber() == mesh1D.getVerticesNumber()
finally:
    os.remove("t_KarhunenLoeveP1Algorithm_std_tmp.xml")
