#! /usr/bin/env python

# Independent verification of the weighted least squares solvers.
#
# The C++ test t_LeastSquaresMethod_weighted checks that the three solvers
# agree with each other under non-uniform weights. This one checks them
# against a reference computed without the least squares classes at all, so
# that a bias shared by the three of them cannot pass unnoticed. With
# Psi = sqrt(W) Phi and z = sqrt(W) y:
#
#   beta  = (Psi^T Psi)^-1 Psi^T z    for LeastSquaresMethod.solve
#   gamma = (Psi^T Psi)^-1 rhs       for LeastSquaresMethod.solveNormalGram
#
# It also checks the leverages against the definition diag(Phi (Phi^T W Phi)^-1 Phi^T W).

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

ot.RandomGenerator.SetSeed(0)
sampleSize = 24
inputDimension = 3

distribution = ot.Normal(inputDimension)
inputSample = distribution.getSample(sampleSize)
# columns that are neither orthogonal nor diagonal, so the Gram matrix is a
# genuine test of the by-products
formulas = ["x0^2", "x1^2 + 0.5 * x1", "x2^2 - 0.25 * x0"]
inputNames = ["x0", "x1", "x2"]
functionCollection = [
    ot.SymbolicFunction(inputNames, [formulas[i]]) for i in range(inputDimension)
]
design = ot.Matrix(sampleSize, inputDimension)
for j in range(inputDimension):
    values = functionCollection[j](inputSample).asPoint()
    for i in range(sampleSize):
        design[i, j] = values[i]

# a target with a component outside the span, so the residuals are non-zero
target = ot.Point(sampleSize)
for i in range(sampleSize):
    target[i] = 0.5 * design[i, 0] - 0.25 * design[i, 1] + 0.75

# strongly non-uniform weights, as a quadrature rule would give
weight = ot.Point(sampleSize)
for i in range(sampleSize):
    weight[i] = 0.2 + 2.0 * ((3 * i) % sampleSize) / float(sampleSize)
print("weight ratio = ", max(weight) / min(weight))
assert max(weight) / min(weight) > 5.0

# reference quantities, built from plain matrix algebra
weightedDesign = ot.Matrix(sampleSize, inputDimension)
weightedTarget = ot.Point(sampleSize)
for i in range(sampleSize):
    root = weight[i] ** 0.5
    weightedTarget[i] = root * target[i]
    for j in range(inputDimension):
        weightedDesign[i, j] = root * design[i, j]
gram = weightedDesign.transpose() * weightedDesign
identity = ot.IdentityMatrix(inputDimension)
gramInverse = gram.solveLinearSystem(identity)
referenceSolve = ot.Point(gramInverse * (weightedDesign.transpose() * weightedTarget))

proxy = ot.DesignProxy(inputSample, functionCollection)
indices = ot.Indices(range(inputDimension))

gramTrace = 0.0
for i in range(inputDimension):
    gramTrace += gramInverse[i, i]
gramInverseDiag = ot.Point(inputDimension)
for i in range(inputDimension):
    gramInverseDiag[i] = gramInverse[i, i]
print("reference Gram inverse trace = ", gramTrace)

# an unrelated right-hand side for the Gram system
rhs = ot.Point(inputDimension)
for i in range(inputDimension):
    rhs[i] = 1.0 + i
referenceGramSolve = ot.Point(gramInverse * rhs)

# leverages: diag(Phi (Phi^T W Phi)^-1 Phi^T W)
weightedByRow = ot.Matrix(sampleSize, inputDimension)
for i in range(sampleSize):
    for j in range(inputDimension):
        weightedByRow[i, j] = weight[i] * design[i, j]
hatDiagonal = ot.Matrix(design * (gramInverse * weightedByRow.transpose()))
referenceHDiag = ot.Point(sampleSize)
for i in range(sampleSize):
    referenceHDiag[i] = hatDiagonal[i, i]

for methodName in ("SVD", "QR", "Cholesky"):
    method = ot.LeastSquaresMethod.Build(methodName, proxy, weight, indices)
    method.update([], indices, [])

    # solve: the weighted normal equations
    ott.assert_almost_equal(method.solve(target), referenceSolve, 1.0e-9, 1.0e-9)

    # solveNormalGram: the same Gram system, with no weight on the rhs
    ott.assert_almost_equal(
        method.solveNormalGram(rhs), referenceGramSolve, 1.0e-9, 1.0e-9
    )

    # Gram matrix inverse, its trace and its diagonal
    ott.assert_almost_equal(
        ot.Matrix(method.getGramInverse()), gramInverse, 1.0e-9, 1.0e-9
    )
    ott.assert_almost_equal(
        method.getGramInverseTrace(), gramTrace, 1.0e-9, 1.0e-9
    )
    ott.assert_almost_equal(
        method.getGramInverseDiag(), gramInverseDiag, 1.0e-9, 1.0e-9
    )

    # leverages
    ott.assert_almost_equal(method.getHDiag(), referenceHDiag, 1.0e-9, 1.0e-9)

    print(methodName, ": solve, solveNormalGram, Gram inverse, trace and leverages agree")

print("Weighted least squares against an independent reference: OK")
