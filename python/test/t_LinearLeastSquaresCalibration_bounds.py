#! /usr/bin/env python

import os

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Linear model y = a + b*x, exact observations generated with (a, b) = (2, 3)
g = ot.SymbolicFunction(["a", "b", "x"], ["a + b*x"])
model = ot.ParametricFunction(g, [0, 1], [2.0, 3.0])
x = ot.Sample([[0.0], [1.0], [2.0], [3.0], [4.0]])
y = model(x)
candidate = [0.0, 0.0]

# unconstrained reference
algoRef = ot.LinearLeastSquaresCalibration(model, x, y, candidate, "SVD")
algoRef.run()
ott.assert_almost_equal(algoRef.getResult().getParameterMAP(), [2.0, 3.0])

algo = ot.LinearLeastSquaresCalibration(model, x, y, candidate, "SVD")
assert algo.getBounds().getDimension() == 0
with ott.assert_raises(TypeError):
    algo.setBounds(ot.Interval([0.0], [1.0]))

# a <= 0.5, b free: the unconstrained solution is [2, 3]
bounds = ot.Interval([-10.0, -10.0], [0.5, 10.0])
algo.setBounds(bounds)
assert algo.getBounds() == bounds
algo.run()
result = algo.getResult()
ott.assert_almost_equal(result.getParameterMAP(), [0.5, 3.5])
posterior = result.getParameterPosterior()
ott.assert_almost_equal(posterior.getMean(), [0.5, 3.5])
covariance = posterior.getCovariance()
# residual r = (1.5, 1.0, 0.5, 0.0, -0.5), dof = 1 free parameter, variance = 0.9375
# free parameter b: sum x^2 = 30, covariance = 0.9375 / 30 = 0.03125
assert abs(covariance[0, 0]) < 1e-10
ott.assert_almost_equal([covariance[1, 1]], [0.03125], 1e-9)
ott.assert_almost_equal(
    result.getObservationsError().getStandardDeviation(), [0.9682458365518543]
)

# regularization must not give a nonzero variance to a parameter fixed at a bound
regularizationKey = "LinearLeastSquaresCalibration-Regularization"
regularizationDefault = ot.ResourceMap.GetAsScalar(regularizationKey)
try:
    ot.ResourceMap.SetAsScalar(regularizationKey, 0.1)
    algoRegularized = ot.LinearLeastSquaresCalibration(model, x, y, candidate, "SVD")
    algoRegularized.setBounds(bounds)
    algoRegularized.run()
    covarianceRegularized = (
        algoRegularized.getResult().getParameterPosterior().getCovariance()
    )
    # a is active at its upper bound and stays fixed, b is free and regularized
    assert abs(covarianceRegularized[0, 0]) < 1e-10
    assert covarianceRegularized[1, 1] > 0.03125
finally:
    ot.ResourceMap.SetAsScalar(regularizationKey, regularizationDefault)

# bounds inactive: same solution as the unconstrained calibration
for method in ["SVD", "QR", "Cholesky"]:
    algoInactive = ot.LinearLeastSquaresCalibration(model, x, y, candidate, method)
    algoInactive.setBounds(ot.Interval([-10.0, -10.0], [10.0, 10.0]))
    algoInactive.run()
    ott.assert_almost_equal(algoInactive.getResult().getParameterMAP(), [2.0, 3.0])

# lower bound active on a: b = 85 / 30
for method in ["SVD", "QR", "Cholesky"]:
    algoLower = ot.LinearLeastSquaresCalibration(model, x, y, candidate, method)
    algoLower.setBounds(ot.Interval([2.5, -10.0], [10.0, 10.0]))
    algoLower.run()
    ott.assert_almost_equal(
        algoLower.getResult().getParameterMAP(), [2.5, 85.0 / 30.0]
    )

# upper bound active on b only: a = 3
algoMixed = ot.LinearLeastSquaresCalibration(model, x, y, candidate, "SVD")
algoMixed.setBounds(ot.Interval([-10.0, -10.0], [10.0, 2.5]))
algoMixed.run()
ott.assert_almost_equal(algoMixed.getResult().getParameterMAP(), [3.0, 2.5])

# infinite bounds on the free side are supported
algoInfinite = ot.LinearLeastSquaresCalibration(model, x, y, candidate, "SVD")
algoInfinite.setBounds(
    ot.Interval([2.5, -1e30], [1e30, 1e30], [True, False], [False, False])
)
algoInfinite.run()
ott.assert_almost_equal(
    algoInfinite.getResult().getParameterMAP(), [2.5, 85.0 / 30.0]
)

# all parameters active at a bound
algoAll = ot.LinearLeastSquaresCalibration(model, x, y, candidate, "SVD")
algoAll.setBounds(ot.Interval([-10.0, -10.0], [0.5, 1.5]))
algoAll.run()
resultAll = algoAll.getResult()
ott.assert_almost_equal(resultAll.getParameterMAP(), [0.5, 1.5])
# residual r = (1.5, 3.0, 4.5, 6.0, 7.5), no free parameter, variance = 24.75
ott.assert_almost_equal(
    resultAll.getObservationsError().getStandardDeviation(), [4.9749371855331]
)
covarianceAll = resultAll.getParameterPosterior().getCovariance()
assert abs(covarianceAll[0, 0]) < 1e-10
assert abs(covarianceAll[1, 1]) < 1e-10

# empty bounds restore the unconstrained path
algoEmpty = ot.LinearLeastSquaresCalibration(model, x, y, candidate, "SVD")
algoEmpty.setBounds(bounds)
algoEmpty.setBounds(ot.Interval())
algoEmpty.run()
ott.assert_almost_equal(algoEmpty.getResult().getParameterMAP(), [2.0, 3.0])

# multivariate output: y = (a + b*x, a - b*x)
gVector = ot.SymbolicFunction(["a", "b", "x"], ["a + b*x", "a - b*x"])
modelVector = ot.ParametricFunction(gVector, [0, 1], [2.0, 3.0])
yVector = modelVector(x)
algoVector = ot.LinearLeastSquaresCalibration(
    modelVector, x, yVector, candidate, "SVD"
)
algoVector.setBounds(ot.Interval([-10.0, -10.0], [0.5, 10.0]))
algoVector.run()
resultVector = algoVector.getResult()
ott.assert_almost_equal(resultVector.getParameterMAP(), [0.5, 3.0])
# residual sum of squares = 22.5 over 10 observations and 1 free parameter:
# variance = 22.5 / 9 and free parameter b has sum of squares 60
ott.assert_almost_equal(
    resultVector.getObservationsError().getStandardDeviation(),
    [1.5811388300841898, 1.5811388300841898],
)
covarianceVector = resultVector.getParameterPosterior().getCovariance()
assert abs(covarianceVector[0, 0]) < 1e-10
ott.assert_almost_equal([covarianceVector[1, 1]], [0.041666666666666664], 1e-9)

# a rank-deficient free block must fail instead of returning a NaN covariance
xDegenerate = ot.Sample([[0.0]] * 5)
yDegenerate = model(xDegenerate)
for method in ["SVD", "QR", "Cholesky"]:
    algoDegenerate = ot.LinearLeastSquaresCalibration(
        model, xDegenerate, yDegenerate, candidate, method
    )
    algoDegenerate.setBounds(ot.Interval([-10.0, -10.0], [10.0, 10.0]))
    with ott.assert_raises(TypeError):
        algoDegenerate.run()

# identical nonzero free columns are rank deficient too: the numerical rank
# check must catch the small positive singular value of the free block
gDependent = ot.SymbolicFunction(["x", "a", "b"], ["(a + b) * x"])
modelDependent = ot.ParametricFunction(gDependent, [1, 2], [0.5, 0.5])
xDependent = ot.Sample([[1.0], [2.0], [3.0], [4.0]])
yDependent = ot.Sample([[3.0], [6.0], [9.0], [12.0]])
for method in ["SVD", "QR", "Cholesky"]:
    algoDependent = ot.LinearLeastSquaresCalibration(
        modelDependent, xDependent, yDependent, [0.5, 0.5], method
    )
    algoDependent.setBounds(ot.Interval([-10.0, -10.0], [10.0, 10.0]))
    with ott.assert_raises(TypeError):
        algoDependent.run()

# a parameter lying inside the feasible interval of a large bound is not
# fixed: the activity tolerance is absolute, not scaled by the bound magnitude
gLarge = ot.SymbolicFunction(["x", "a"], ["a * x"])
aLarge = 1.0e6 + 0.5
modelLarge = ot.ParametricFunction(gLarge, [1], [aLarge])
xLarge = ot.Sample([[1.0], [2.0], [3.0]])
yLarge = ot.Sample([[aLarge], [2.0 * aLarge], [3.0 * aLarge + 1.0]])
algoLarge = ot.LinearLeastSquaresCalibration(modelLarge, xLarge, yLarge, [aLarge], "SVD")
algoLarge.setBounds(ot.Interval([1.0e6], [2.0e6]))
algoLarge.run()
resultLarge = algoLarge.getResult()
thetaLarge = resultLarge.getParameterPosterior().getMean()[0]
# least-squares solution a* = aLarge + 3 / 14, i.e. 3 / 14 above the lower bound.
# The QP solver is only accurate to a relative tolerance at this magnitude, so
# the comparisons are relative: an active bound would return exactly 1.0e6.
ott.assert_almost_equal([thetaLarge], [aLarge + 3.0 / 14.0], 1e-7)
assert thetaLarge - 1.0e6 > 0.5
covarianceLarge = resultLarge.getParameterPosterior().getCovariance()
# residual r = (3, 6, -5) / 14, variance error = (5 / 14) / 2, gram inverse = 1 / 14
# The fallback solver used when HiGHS is not available is less accurate on
# this large-magnitude problem, so the covariance is checked loosely.
assert covarianceLarge[0, 0] > 0.0
ott.assert_almost_equal([covarianceLarge[0, 0]], [5.0 / 392.0], 5e-2)

# invalid active-bound tolerances are rejected instead of silently changing
# the covariance and the residual degrees of freedom
toleranceKey = "LinearLeastSquaresCalibration-ActiveBoundTolerance"
toleranceDefault = ot.ResourceMap.GetAsScalar(toleranceKey)
try:
    for invalidTolerance in [0.0, -1.0, float("inf"), float("nan")]:
        ot.ResourceMap.SetAsScalar(toleranceKey, invalidTolerance)
        algoInvalid = ot.LinearLeastSquaresCalibration(model, x, y, candidate, "SVD")
        algoInvalid.setBounds(bounds)
        with ott.assert_raises(TypeError):
            algoInvalid.run()
finally:
    ot.ResourceMap.SetAsScalar(toleranceKey, toleranceDefault)

# save/load round trip
algoSave = ot.LinearLeastSquaresCalibration(model, x, y, candidate, "SVD")
algoSave.setBounds(bounds)
studyFile = "linear_least_squares_calibration_bounds.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(studyFile))
study.add("algo", algoSave)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager(studyFile))
loadedStudy.load()
loadedAlgo = ot.LinearLeastSquaresCalibration()
loadedStudy.fillObject("algo", loadedAlgo)
assert loadedAlgo.getBounds() == bounds
loadedAlgo.run()
ott.assert_almost_equal(loadedAlgo.getResult().getParameterMAP(), [0.5, 3.5])
os.remove(studyFile)
