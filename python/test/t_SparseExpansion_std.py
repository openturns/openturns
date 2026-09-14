#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott
import os
from math import pi

ot.TESTPREAMBLE()

# A simple 1D problem: sin(x) on [-2, 2]
g = ot.SymbolicFunction("x", "sin(x)")
distribution = ot.Uniform(-2.0, 2.0)
dimension = 1

inputSample = distribution.getSample(20)
outputSample = g(inputSample)

# Create the orthogonal basis
polynomialCollection = [ot.LegendreFactory()]
enumerateFunction = ot.LinearEnumerateFunction(dimension)
productBasis = ot.OrthogonalProductPolynomialFactory(
    polynomialCollection, enumerateFunction
)

basisSize = 10
methodName = "QR"

# Full constructor with weights
weights = ot.Point(20, 1.0 / 20.0)
algo = otexp.SparseExpansion(
    inputSample,
    weights,
    outputSample,
    distribution,
    productBasis,
    basisSize,
    methodName,
)
algo.run()
result = algo.getResult()
assert result.isLeastSquares()
assert result.involvesModelSelection()
print("Full constructor with weights: OK")

# Check that the result coefficients are not all zero
coeffs = result.getCoefficients().asPoint()
for i in range(coeffs.getSize()):
    ott.assert_almost_equal(abs(coeffs[i]) > 0.0, True)

# Full constructor without weights (uniform weights)
algo = otexp.SparseExpansion(
    inputSample, outputSample, distribution,
    productBasis, basisSize, methodName
)
algo.run()
result = algo.getResult()
assert result.isLeastSquares()
assert result.involvesModelSelection()
print("Full constructor without weights: OK")

# Simplified constructor with weights (no basis/methodName specified)
algo = otexp.SparseExpansion(inputSample, weights, outputSample, distribution)
algo.run()
result = algo.getResult()
assert result.isLeastSquares()
assert result.involvesModelSelection()
print("Simplified constructor with weights: OK")

# Simplified constructor without weights
algo = otexp.SparseExpansion(inputSample, outputSample, distribution)
algo.run()
result = algo.getResult()
assert result.isLeastSquares()
assert result.involvesModelSelection()
print("Simplified constructor without weights: OK")

# Check setActiveFunctions / getActiveFunctions
algo = otexp.SparseExpansion(
    inputSample, outputSample, distribution,
    productBasis, basisSize, methodName
)
algo.setActiveFunctions(ot.Indices(range(3)))
ott.assert_almost_equal(algo.getActiveFunctions(), ot.Indices([0, 1, 2]))
print("setActiveFunctions/getActiveFunctions: OK")

# Check setActiveFunctions ensures index 0 is always present
algo.setActiveFunctions(ot.Indices([1, 2, 3]))
ott.assert_almost_equal(algo.getActiveFunctions(), ot.Indices([1, 2, 3, 0]))
print("setActiveFunctions ensures index 0: OK")

# Check constructor with active functions
algo = otexp.SparseExpansion(
    inputSample, outputSample, distribution,
    productBasis, basisSize, methodName
)
algo.setActiveFunctions(ot.Indices([0, 1, 2]))
algo.run()
result = algo.getResult()
assert result.isLeastSquares()
assert result.involvesModelSelection()
print("Constructor with active functions: OK")

# Check getFittingAlgorithm / setFittingAlgorithm
fitting = algo.getFittingAlgorithm()
print("Default fitting algorithm: ", fitting)
assert fitting.getImplementation().getClassName() == "CorrectedLeaveOneOut"
newFitting = ot.KFold()
algo.setFittingAlgorithm(newFitting)
fitting2 = algo.getFittingAlgorithm()
assert fitting2.getImplementation().getClassName() == "KFold"
print("setFittingAlgorithm/getFittingAlgorithm: OK")

algo.run()
# Check the selection history
result = algo.getResult()
indicesHistory = result.getIndicesHistory()
print("Indices history size: ", indicesHistory.getSize())
if indicesHistory.getSize() > 0:
    print("First history entry indices: ", indicesHistory[0])

coeffsHistory = result.getCoefficientsHistory()
print("Coefficients history size: ", coeffsHistory.getSize())

errorHistory = result.getErrorHistory()
print("Error history size: ", errorHistory.getSize())

# History sizes must be consistent
ott.assert_almost_equal(indicesHistory.getSize(), coeffsHistory.getSize())
ott.assert_almost_equal(errorHistory.getSize(), coeffsHistory.getSize())
print("History consistency: OK")

# Check __str__ and __repr__
print("algo.__str__: ", algo.__str__())
print("algo.__repr__: ", algo.__repr__())

# Check different decomposition methods
for methodName in ["SVD", "QR", "Cholesky"]:
    algo = otexp.SparseExpansion(
        inputSample, outputSample, distribution,
        productBasis, basisSize, methodName
    )
    algo.run()
    result = algo.getResult()
    assert result.isLeastSquares()
    print("Method " + methodName + ": OK")

# Check serialization (save/load)
if ot.PlatformInfo.HasFeature("libxml2"):
    algo = otexp.SparseExpansion(
        inputSample, outputSample, distribution,
        productBasis, basisSize, methodName
    )
    algo.setFittingAlgorithm(ot.KFold())
    algo.run()
    result = algo.getResult()
    fname = "test_omp_saveload.xml"
    try:
        study = ot.Study()
        study.setStorageManager(ot.XMLStorageManager(fname))
        study.add("algo", algo)
        study.save()
        study2 = ot.Study()
        study2.setStorageManager(ot.XMLStorageManager(fname))
        study2.load()
        algoLoaded = otexp.SparseExpansion()
        study2.fillObject("algo", algoLoaded)
        resultLoaded = algoLoaded.getResult()
        ott.assert_almost_equal(
            resultLoaded.getCoefficients(), result.getCoefficients()
        )
        assert (
            algoLoaded.getFittingAlgorithm().getImplementation()
            .getClassName() == "KFold"
        )
        print("Serialization: OK")
    finally:
        if os.path.exists(fname):
            os.unlink(fname)
else:
    print("Serialization: SKIPPED (libxml2 not available)")

# Check exception: basis dimension mismatch
wrongBasis = ot.OrthogonalProductPolynomialFactory(
    [ot.LegendreFactory()] * 2, ot.LinearEnumerateFunction(2)
)
with ott.assert_raises(TypeError):
    otexp.SparseExpansion(
        inputSample, outputSample, distribution,
        wrongBasis, basisSize, methodName
    )
print("Exception basis dimension: OK")

# Check exception: basis size zero
with ott.assert_raises(TypeError):
    otexp.SparseExpansion(
        inputSample, outputSample, distribution, productBasis, 0, methodName
    )
print("Exception basis size zero: OK")

# Check exception: active functions index out of range
algo = otexp.SparseExpansion(
    inputSample, outputSample, distribution,
    productBasis, basisSize, methodName
)
with ott.assert_raises(TypeError):
    algo.setActiveFunctions(ot.Indices([basisSize + 1]))
print("Exception active functions out of range: OK")

# Check useDomination
algo = otexp.SparseExpansion(
    inputSample, outputSample, distribution,
    productBasis, basisSize, methodName
)
algo.setUseDomination(True)
assert algo.getUseDomination()
algo.setUseDomination(False)
assert not algo.getUseDomination()
print("useDomination accessor: OK")

# Check SparseExpansion-ConsecutiveIncreases
# k=1: stop on first increase (more aggressive)
ot.ResourceMap.SetAsUnsignedInteger("SparseExpansion-ConsecutiveIncreases", 1)
algo_k1 = otexp.SparseExpansion(
    inputSample, outputSample, distribution,
    productBasis, basisSize, methodName
)
algo_k1.run()
result_k1 = algo_k1.getResult()
n_active_k1 = result_k1.getIndices().getSize()

# k=5: allow 5 consecutive increases (more tolerant)
ot.ResourceMap.SetAsUnsignedInteger("SparseExpansion-ConsecutiveIncreases", 5)
algo_k5 = otexp.SparseExpansion(
    inputSample, outputSample, distribution,
    productBasis, basisSize, methodName
)
algo_k5.run()
result_k5 = algo_k5.getResult()
n_active_k5 = result_k5.getIndices().getSize()

# Restore default
ot.ResourceMap.SetAsUnsignedInteger("SparseExpansion-ConsecutiveIncreases", 2)
algo_default = otexp.SparseExpansion(
    inputSample, outputSample, distribution,
    productBasis, basisSize, methodName
)
algo_default.run()
result_default = algo_default.getResult()
n_active_default = result_default.getIndices().getSize()

print(f"ConsecutiveIncreases k=1: {n_active_k1} active functions")
print(f"ConsecutiveIncreases k=2 (default): {n_active_default} active functions")
print(f"ConsecutiveIncreases k=5: {n_active_k5} active functions")
ott.assert_almost_equal(n_active_k1 <= n_active_default <= n_active_k5, True)
print("SparseExpansion-ConsecutiveIncreases: OK")

# Check LARS selection method
algo = otexp.SparseExpansion(
    inputSample, outputSample, distribution,
    productBasis, basisSize, methodName
)
algo.setSelectionMethod("LARS")
assert algo.getSelectionMethod() == "LARS"
algo.run()
result = algo.getResult()
assert result.isLeastSquares()
assert result.involvesModelSelection()
print("LARS selection method: OK")

# Check exception for invalid selection method
with ott.assert_raises(TypeError):
    algo.setSelectionMethod("INVALID")
print("Exception invalid selection method: OK")

# Check LARS with different decomposition methods
for methodName in ["SVD", "QR", "Cholesky"]:
    algo = otexp.SparseExpansion(
        inputSample, outputSample, distribution,
        productBasis, basisSize, methodName
    )
    algo.setSelectionMethod("LARS")
    algo.run()
    result = algo.getResult()
    assert result.isLeastSquares()
    print("LARS with method " + methodName + ": OK")

# Multi-output test
g2 = ot.SymbolicFunction(["x", "y"], ["sin(x) + y", "x * y"])
distribution2 = ot.JointDistribution([ot.Uniform(-pi, pi)] * 2)
inputSample2 = distribution2.getSample(30)
outputSample2 = g2(inputSample2)
productBasis2 = ot.OrthogonalProductPolynomialFactory(
    [ot.LegendreFactory()] * 2, ot.LinearEnumerateFunction(2)
)
algo = otexp.SparseExpansion(
    inputSample2, outputSample2, distribution2, productBasis2, 15, "QR"
)
algo.run()
result = algo.getResult()
assert result.isLeastSquares()
assert result.involvesModelSelection()
coeffs = result.getCoefficients()
assert coeffs.getDimension() == 2
print("Multi-output: OK")
