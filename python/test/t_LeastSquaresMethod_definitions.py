#! /usr/bin/env python

# Verification of the weighted least squares solvers.
#
# Two kinds of check are used, and they play very different roles.
#
# 1. Conditions that follow from the definition of the quantities, hence
#    decide on their own whether the library is right. The weighted
#    least squares solution beta* is characterized by
#    Phi^T W (y - Phi beta*) = 0, and this is checked against the weights
#    the *caller* asked for, whatever the method was handed. A method that
#    minimises a different objective, for instance because it drops or
#    misreads the weights, still satisfies the normal equations of the
#    objective it actually minimised, so only this version of the check
#    has teeth: the observed separation is 1e-15 against 1e-1. Likewise
#    the Gram inverse is checked as an inverse, G G^-1 = I, and the
#    leverages as a projection, their sum being the number of active
#    functions whatever the weights.
#
# 2. A comparison with a direct matrix algebra computation of the same
#    quantities. Agreement is consistency evidence, not proof: a defect
#    shared by the two computations, or a mistake in this file, would pass.
#    That is why it comes second, and why the deciding instrument is the
#    set of conditions above.
#
# The C++ test t_LeastSquaresMethod_weighted only checks that the three
# solvers agree with each other, which is the weaker form of item 2.

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

# the objective the caller asked to minimise, and its normal equations
weightSqrt = ot.Point(sampleSize)
for i in range(sampleSize):
    weightSqrt[i] = weight[i] ** 0.5


def predict(coefficients):
    prediction = ot.Point(sampleSize)
    for i in range(sampleSize):
        prediction[i] = sum(
            design[i, j] * coefficients[j] for j in range(inputDimension)
        )
    return prediction


def objective(coefficients):
    """Weighted sum of squares, the quantity the caller wants to minimize."""
    prediction = predict(coefficients)
    total = 0.0
    for i in range(sampleSize):
        residual = target[i] - prediction[i]
        total += weight[i] * residual * residual
    return total


def normalEquationsResidual(coefficients):
    """Phi^T W (y - Phi beta), which vanishes at the minimizer."""
    prediction = predict(coefficients)
    worst = 0.0
    for j in range(inputDimension):
        accumulator = 0.0
        for i in range(sampleSize):
            accumulator += (
                design[i, j] * weight[i] * (target[i] - prediction[i])
            )
        worst = max(worst, abs(accumulator))
    return worst


# quantities computed directly from their definition, in plain matrix algebra
weightedDesign = ot.Matrix(sampleSize, inputDimension)
weightedTarget = ot.Point(sampleSize)
for i in range(sampleSize):
    root = weight[i] ** 0.5
    weightedTarget[i] = root * target[i]
    for j in range(inputDimension):
        weightedDesign[i, j] = root * design[i, j]
gram = weightedDesign.transpose() * weightedDesign
gramInverse = gram.solveLinearSystem(ot.IdentityMatrix(inputDimension))
directSolve = ot.Point(gramInverse * (weightedDesign.transpose() * weightedTarget))
directGramSolve = ot.Point(
    gramInverse * ot.Point([1.0 + i for i in range(inputDimension)])
)
gramTrace = 0.0
for i in range(inputDimension):
    gramTrace += gramInverse[i, i]
gramInverseDiag = ot.Point(inputDimension)
for i in range(inputDimension):
    gramInverseDiag[i] = gramInverse[i, i]
# leverages: diag(Phi (Phi^T W Phi)^-1 Phi^T W)
weightedByRow = ot.Matrix(sampleSize, inputDimension)
for i in range(sampleSize):
    for j in range(inputDimension):
        weightedByRow[i, j] = weight[i] * design[i, j]
hatDiagonal = ot.Matrix(design * (gramInverse * weightedByRow.transpose()))
directHDiag = ot.Point(sampleSize)
for i in range(sampleSize):
    directHDiag[i] = hatDiagonal[i, i]

proxy = ot.DesignProxy(inputSample, functionCollection)
indices = ot.Indices(range(inputDimension))
rhs = ot.Point([1.0 + i for i in range(inputDimension)])

for methodName in ("SVD", "QR", "Cholesky"):
    method = ot.LeastSquaresMethod.Build(methodName, proxy, weight, indices)
    method.update([], indices, [])

    # --- conditions that decide on their own ---
    beta = method.solve(target)
    # the normal equations of the caller's problem, in the units of the
    # weighted residual, which is the scale the condition is stated in
    residualScale = 0.0
    prediction = predict(beta)
    for i in range(sampleSize):
        residualScale = max(
            residualScale, abs(weight[i] * (target[i] - prediction[i]))
        )
    residualScale *= inputDimension
    print(
        f"{methodName}: normal equations residual = "
        f"{normalEquationsResidual(beta) / residualScale:.3e}"
    )
    ott.assert_almost_equal(
        normalEquationsResidual(beta), 0.0, 1.0e-12, 1.0e-12 * residualScale
    )

    # the objective must be minimal: any perturbation of one coefficient,
    # in either direction, has to increase it
    value = objective(beta)
    for j in range(inputDimension):
        step = 1.0e-3 * max(1.0, abs(beta[j]))
        for direction in (1.0, -1.0):
            perturbed = ot.Point(beta)
            perturbed[j] += direction * step
            assert objective(perturbed) > value, (
                f"{methodName}: the objective decreases when coefficient {j} is "
                f"moved by {direction * step}, so the solution is not a minimizer"
            )

    # the Gram inverse is an inverse, and the leverages sum to the number of
    # active functions whatever the weights, since trace(Phi G^-1 Phi^T W)
    # is trace(G^-1 G)
    computedGramInverse = ot.Matrix(method.getGramInverse())
    identity = ot.IdentityMatrix(inputDimension)
    ott.assert_almost_equal(
        ot.Matrix(gram * computedGramInverse),
        ot.Matrix(identity),
        1.0e-12,
        1.0e-12,
    )
    ott.assert_almost_equal(
        sum(method.getHDiag()), float(inputDimension), 1.0e-12, 1.0e-12
    )

    # --- consistency with a direct computation of the same quantities ---
    ott.assert_almost_equal(beta, directSolve, 1.0e-9, 1.0e-9)
    # solveNormalGram: the Gram system, with no weight on the rhs
    ott.assert_almost_equal(
        method.solveNormalGram(rhs), directGramSolve, 1.0e-9, 1.0e-9
    )
    ott.assert_almost_equal(
        computedGramInverse, gramInverse, 1.0e-9, 1.0e-9
    )
    ott.assert_almost_equal(
        method.getGramInverseTrace(), gramTrace, 1.0e-9, 1.0e-9
    )
    ott.assert_almost_equal(
        method.getGramInverseDiag(), gramInverseDiag, 1.0e-9, 1.0e-9
    )
    ott.assert_almost_equal(method.getHDiag(), directHDiag, 1.0e-9, 1.0e-9)

    print(f"{methodName}: minimizer, inverse and projection hold")

print("Weighted least squares: OK")
