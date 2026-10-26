#! /usr/bin/env python

import os

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

HAVE_LIBXML2 = ot.PlatformInfo.HasFeature("libxml2")

# Linear model y = a + b*x, exact observations generated with (a, b) = (2, 3)
g = ot.SymbolicFunction(["a", "b", "x"], ["a + b*x"])
model = ot.ParametricFunction(g, [0, 1], [2.0, 3.0])
x = ot.Sample([[0.0], [1.0], [2.0], [3.0], [4.0]])
y = model(x)
calibrationModel = ot.ParametricFunction(g, [2], [0.0])
candidate = [0.0, 0.0]

# Without HiGHS, bounded calibration relies on the first generic algorithm
# returned by OptimizationAlgorithm::Build (Cobyla), which is less accurate
# than the HiGHS quadratic solver.
boundedMapRtol = 1e-5 if ot.PlatformInfo.HasFeature("highs") else 1e-3

# unconstrained reference
algoRef = ot.LinearLeastSquaresCalibration(calibrationModel, x, y, candidate, "SVD")
algoRef.run()
ott.assert_almost_equal(algoRef.getResult().getParameter(), [2.0, 3.0])

algo = ot.LinearLeastSquaresCalibration(calibrationModel, x, y, candidate, "SVD")
assert algo.getBounds().getDimension() == 0
with ott.assert_raises(TypeError):
    algo.setBounds(ot.Interval([0.0], [1.0]))

# a <= 0.5, b free: the unconstrained solution is [2, 3]
bounds = ot.Interval([-10.0, -10.0], [0.5, 10.0])
algo.setBounds(bounds)
assert algo.getBounds() == bounds
algo.run()
result = algo.getResult()
ott.assert_almost_equal(result.getParameter(), [0.5, 3.5], boundedMapRtol)
posterior = result.getParameterDistribution()
ott.assert_almost_equal(posterior.getMean(), [0.5, 3.5], boundedMapRtol)
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
    algoRegularized = ot.LinearLeastSquaresCalibration(calibrationModel, x, y, candidate, "SVD")
    algoRegularized.setBounds(bounds)
    algoRegularized.run()
    covarianceRegularized = (
        algoRegularized.getResult().getParameterDistribution().getCovariance()
    )
    # a is active at its upper bound and stays fixed, b is free and regularized
    assert abs(covarianceRegularized[0, 0]) < 1e-10
    assert covarianceRegularized[1, 1] > 0.03125
finally:
    ot.ResourceMap.SetAsScalar(regularizationKey, regularizationDefault)

# bounds inactive: same solution as the unconstrained calibration
for method in ["SVD", "QR", "Cholesky"]:
    algoInactive = ot.LinearLeastSquaresCalibration(calibrationModel, x, y, candidate, method)
    algoInactive.setBounds(ot.Interval([-10.0, -10.0], [10.0, 10.0]))
    algoInactive.run()
    ott.assert_almost_equal(
        algoInactive.getResult().getParameter(), [2.0, 3.0], boundedMapRtol
    )

# lower bound active on a: b = 85 / 30
for method in ["SVD", "QR", "Cholesky"]:
    algoLower = ot.LinearLeastSquaresCalibration(calibrationModel, x, y, candidate, method)
    algoLower.setBounds(ot.Interval([2.5, -10.0], [10.0, 10.0]))
    algoLower.run()
    ott.assert_almost_equal(
        algoLower.getResult().getParameter(), [2.5, 85.0 / 30.0], boundedMapRtol
    )

# upper bound active on b only: a = 3
algoMixed = ot.LinearLeastSquaresCalibration(calibrationModel, x, y, candidate, "SVD")
algoMixed.setBounds(ot.Interval([-10.0, -10.0], [10.0, 2.5]))
algoMixed.run()
ott.assert_almost_equal(
    algoMixed.getResult().getParameter(), [3.0, 2.5], boundedMapRtol
)

# infinite bounds on the free side are supported
algoInfinite = ot.LinearLeastSquaresCalibration(calibrationModel, x, y, candidate, "SVD")
algoInfinite.setBounds(
    ot.Interval([2.5, -1e30], [1e30, 1e30], [True, False], [False, False])
)
algoInfinite.run()
ott.assert_almost_equal(
    algoInfinite.getResult().getParameter(), [2.5, 85.0 / 30.0], boundedMapRtol
)

# all parameters active at a bound
algoAll = ot.LinearLeastSquaresCalibration(calibrationModel, x, y, candidate, "SVD")
algoAll.setBounds(ot.Interval([-10.0, -10.0], [0.5, 1.5]))
algoAll.run()
resultAll = algoAll.getResult()
ott.assert_almost_equal(resultAll.getParameter(), [0.5, 1.5], boundedMapRtol)
# residual r = (1.5, 3.0, 4.5, 6.0, 7.5), no free parameter, variance = 24.75
ott.assert_almost_equal(
    resultAll.getObservationsError().getStandardDeviation(), [4.9749371855331]
)
covarianceAll = resultAll.getParameterDistribution().getCovariance()
assert abs(covarianceAll[0, 0]) < 1e-10
assert abs(covarianceAll[1, 1]) < 1e-10

# empty bounds restore the unconstrained path
algoEmpty = ot.LinearLeastSquaresCalibration(calibrationModel, x, y, candidate, "SVD")
algoEmpty.setBounds(bounds)
algoEmpty.setBounds(ot.Interval())
algoEmpty.run()
ott.assert_almost_equal(algoEmpty.getResult().getParameter(), [2.0, 3.0])

# multivariate output: y = (a + b*x, a - b*x)
gVector = ot.SymbolicFunction(["a", "b", "x"], ["a + b*x", "a - b*x"])
modelVector = ot.ParametricFunction(gVector, [0, 1], [2.0, 3.0])
yVector = modelVector(x)
calibrationModelVector = ot.ParametricFunction(gVector, [2], [0.0])
algoVector = ot.LinearLeastSquaresCalibration(calibrationModelVector, x, yVector, candidate, "SVD")
algoVector.setBounds(ot.Interval([-10.0, -10.0], [0.5, 10.0]))
algoVector.run()
resultVector = algoVector.getResult()
ott.assert_almost_equal(resultVector.getParameter(), [0.5, 3.0], boundedMapRtol)
# residual sum of squares = 22.5 over 10 observations and 1 free parameter:
# variance = 22.5 / 9 and free parameter b has sum of squares 60
ott.assert_almost_equal(
    resultVector.getObservationsError().getStandardDeviation(),
    [1.5811388300841898, 1.5811388300841898],
)
covarianceVector = resultVector.getParameterDistribution().getCovariance()
assert abs(covarianceVector[0, 0]) < 1e-10
ott.assert_almost_equal([covarianceVector[1, 1]], [0.041666666666666664], 1e-9)

# example of the specification draft (section 3.1.6): with the design
# [[1, 1], [1, 2], [1, 3], [1, 4]] and the observations [6, 5, 7, 10], the
# unconstrained solution is [3.5, 1.4]; with beta_1 in [2, 5], the active
# constraint moves the solution to [2, 2], which is not the projection
# [3.5, 2] of the unconstrained solution
gSpec = ot.SymbolicFunction(["a", "b", "x"], ["a + b*x"])
calibrationModelSpec = ot.ParametricFunction(gSpec, [2], [0.0])
xSpec = ot.Sample([[1.0], [2.0], [3.0], [4.0]])
ySpec = ot.Sample([[6.0], [5.0], [7.0], [10.0]])
algoSpec = ot.LinearLeastSquaresCalibration(calibrationModelSpec, xSpec, ySpec, [0.0, 0.0], "SVD")
algoSpec.setBounds(ot.Interval([0.0, 2.0], [0.0, 5.0], [False, True], [False, True]))
algoSpec.run()
ott.assert_almost_equal(
    algoSpec.getResult().getParameter(), [2.0, 2.0], boundedMapRtol
)

# a rank-deficient free block must fail instead of returning a NaN covariance
xDegenerate = ot.Sample([[0.0]] * 5)
yDegenerate = model(xDegenerate)
for method in ["SVD", "QR", "Cholesky"]:
    algoDegenerate = ot.LinearLeastSquaresCalibration(
        calibrationModel, xDegenerate, yDegenerate, candidate, method
    )
    algoDegenerate.setBounds(ot.Interval([-10.0, -10.0], [10.0, 10.0]))
    with ott.assert_raises(TypeError):
        algoDegenerate.run()

# identical nonzero free columns are rank deficient too: the numerical rank
# check must catch the small positive singular value of the free block
gDependent = ot.SymbolicFunction(["x", "a", "b"], ["(a + b) * x"])
calibrationModelDependent = ot.ParametricFunction(gDependent, [0], [0.0])
xDependent = ot.Sample([[1.0], [2.0], [3.0], [4.0]])
yDependent = ot.Sample([[3.0], [6.0], [9.0], [12.0]])
for method in ["SVD", "QR", "Cholesky"]:
    algoDependent = ot.LinearLeastSquaresCalibration(
        calibrationModelDependent, xDependent, yDependent, [0.5, 0.5], method
    )
    algoDependent.setBounds(ot.Interval([-10.0, -10.0], [10.0, 10.0]))
    with ott.assert_raises(TypeError):
        algoDependent.run()

# a parameter lying inside the feasible interval of a large bound is not
# fixed: the activity tolerance is absolute, not scaled by the bound magnitude
gLarge = ot.SymbolicFunction(["x", "a"], ["a * x"])
aLarge = 1.0e6 + 0.5
calibrationModelLarge = ot.ParametricFunction(gLarge, [0], [0.0])
xLarge = ot.Sample([[1.0], [2.0], [3.0]])
yLarge = ot.Sample([[aLarge], [2.0 * aLarge], [3.0 * aLarge + 1.0]])
algoLarge = ot.LinearLeastSquaresCalibration(
    calibrationModelLarge, xLarge, yLarge, [aLarge], "SVD"
)
algoLarge.setBounds(ot.Interval([1.0e6], [2.0e6]))
algoLarge.run()
resultLarge = algoLarge.getResult()
thetaLarge = resultLarge.getParameterDistribution().getMean()[0]
# least-squares solution a* = aLarge + 3 / 14, i.e. 3 / 14 above the lower bound.
# The QP solver is only accurate to a relative tolerance at this magnitude, so
# the comparisons are relative: an active bound would return exactly 1.0e6.
# Without HiGHS the fallback solver is less accurate on this problem.
largeMapRtol = 1e-7 if ot.PlatformInfo.HasFeature("highs") else 1e-5
ott.assert_almost_equal([thetaLarge], [aLarge + 3.0 / 14.0], largeMapRtol)
assert thetaLarge - 1.0e6 > 0.5
covarianceLarge = resultLarge.getParameterDistribution().getCovariance()
# residual r = (3, 6, -5) / 14, variance error = (5 / 14) / 2, gram inverse = 1 / 14
# The fallback solver used when HiGHS is not available is less accurate on
# this large-magnitude problem, so the covariance is checked loosely.
assert covarianceLarge[0, 0] > 0.0
ott.assert_almost_equal([covarianceLarge[0, 0]], [5.0 / 392.0], 5e-2)

# a large increment from the starting point must not exhaust the fallback
# solver budget: the bounded problem is solved in the increment variable, so a
# candidate far from the solution produces a large increment
gShift = ot.SymbolicFunction(["x", "a"], ["a * x"])
calibrationModelShift = ot.ParametricFunction(gShift, [0], [0.0])
xShift = ot.Sample([[1.0], [1.0], [1.0]])
yShift = ot.Sample([[1.0e6], [1.0e6], [1.0e6]])
algoShift = ot.LinearLeastSquaresCalibration(calibrationModelShift, xShift, yShift, [0.0], "SVD")
algoShift.setBounds(ot.Interval([-2.0e6], [2.0e6]))
algoShift.run()
ott.assert_almost_equal(algoShift.getResult().getParameter(), [1.0e6], 1e-6)

# a one-sided bound whose optimum is far from the starting point must not
# exhaust the fallback solver budget: a half-infinite box has no width to
# scale the initial trust-region radius from, so it is derived from the
# unconstrained least-squares increment
algoShiftOneSided = ot.LinearLeastSquaresCalibration(
    calibrationModelShift, xShift, yShift, [0.0], "SVD"
)
algoShiftOneSided.setBounds(ot.Interval([0.0], [1.0e30], [True], [False]))
algoShiftOneSided.run()
ott.assert_almost_equal(algoShiftOneSided.getResult().getParameter(), [1.0e6], 1e-6)

# a design whose smallest singular value is below the square root of the
# machine precision: its normal matrix cannot represent the direction it
# identifies, so the bounded solve works on the design itself and agrees with
# the unconstrained SVD solution when no bound is active
illG = ot.SymbolicFunction(["t", "a", "b"], ["a + b + t", "1e-9 * b", "0.0"])
illCalibrationModel = ot.ParametricFunction(illG, [0], [0.0])
illX = ot.Sample([[0.0]])
for illObservations in ([1.0, 1e-9, 0.1], [1.0, 2e-9, 0.1]):
    illY = ot.Sample([illObservations])
    reference = ot.LinearLeastSquaresCalibration(
        illCalibrationModel, illX, illY, [0.0, 0.0], "SVD"
    )
    reference.run()
    bounded = ot.LinearLeastSquaresCalibration(illCalibrationModel, illX, illY, [0.0, 0.0], "SVD")
    bounded.setBounds(ot.Interval([-3.0, -3.0], [3.0, 3.0]))
    bounded.run()
    ott.assert_almost_equal(
        bounded.getResult().getParameter(),
        reference.getResult().getParameter(),
        1e-6,
        1e-6,
    )
# a bound active on the weakly identified direction is resolved exactly
active = ot.LinearLeastSquaresCalibration(
    illCalibrationModel, illX, ot.Sample([[1.0, 2e-9, 0.1]]), [0.0, 0.0], "SVD"
)
active.setBounds(ot.Interval([-3.0, 0.0], [3.0, 1.5]))
active.run()
ott.assert_almost_equal(active.getResult().getParameter(), [-0.5, 1.5], 1e-6, 1e-6)

# a residual component that no design column can cancel (a constant offset on
# an extra observation) must not scale the release tolerance of the active-set
# solver: with design = [[1, 1], [0, 1e-9], [0, 0]] and right-hand side
# [1, 2e-9, 1e9], a tolerance scaled by the residual norm would hide the
# descent direction of the first parameter and return [0, 0.5] instead of the
# constrained minimizer [0.5, 0.5]
constantOffset = ot.LinearLeastSquaresCalibration(
    illCalibrationModel, illX, ot.Sample([[1.0, 2e-9, 1e9]]), [0.0, 0.0], "SVD"
)
constantOffset.setBounds(ot.Interval([0.0, 0.0], [3.0, 0.5]))
constantOffset.run()
ott.assert_almost_equal(
    constantOffset.getResult().getParameter(), [0.5, 0.5], 1e-6, 1e-6
)

# a rank-deficient free block must not abort the active-set iterations: the
# intermediate free solve is rank revealing and returns the minimum-norm
# least-squares solution, after which the bounds are activated normally
singularG = ot.SymbolicFunction(["t", "a", "b"], ["a + b", "0.0", "0.0"])
singularCalibrationModel = ot.ParametricFunction(singularG, [0], [0.0])
singularX = ot.Sample([[0.0]])
singular = ot.LinearLeastSquaresCalibration(
    singularCalibrationModel, singularX, ot.Sample([[-1.0, 0.1, 0.1]]), [0.0, 0.0], "SVD"
)
singular.setBounds(ot.Interval([0.0, 0.0], [3.0, 3.0]))
singular.run()
ott.assert_almost_equal(singular.getResult().getParameter(), [0.0, 0.0])

# the active-set fallback must honour the finite-bound flags: an interval may
# store an endpoint on a side marked unbounded, which must stay unconstrained
splitG = ot.SymbolicFunction(["t", "a", "b"], ["a + t", "1e-9 * b", "0.0"])
splitCalibrationModel = ot.ParametricFunction(splitG, [0], [0.0])
splitY = ot.Sample([[-1.0, 1e-9, 0.1]])
unboundedLower = ot.LinearLeastSquaresCalibration(
    splitCalibrationModel, illX, splitY, [0.0, 0.0], "SVD"
)
unboundedLower.setBounds(
    ot.Interval([0.0, -3.0], [3.0, 3.0], [False, True], [True, True])
)
unboundedLower.run()
ott.assert_almost_equal(
    unboundedLower.getResult().getParameter(), [-1.0, 1.0], 1e-6, 1e-6
)
# a coordinate sitting on a bound must stay free when the step is blocked by
# another coordinate, instead of being frozen at its current value
blocked = ot.LinearLeastSquaresCalibration(splitCalibrationModel, illX, splitY, [0.0, 0.0], "SVD")
blocked.setBounds(ot.Interval([0.0, 0.0], [3.0, 3.0]))
blocked.run()
ott.assert_almost_equal(blocked.getResult().getParameter(), [0.0, 1.0], 1e-6, 1e-6)
# a parameter with equal bounds is fixed for good: releasing it would cycle
# until the iteration budget is exhausted
equality = ot.LinearLeastSquaresCalibration(
    splitCalibrationModel, illX, ot.Sample([[1.0, 1e-9, 0.1]]), [0.0, 0.0], "SVD"
)
equality.setBounds(ot.Interval([0.0, -3.0], [0.0, 3.0]))
equality.run()
ott.assert_almost_equal(equality.getResult().getParameter(), [0.0, 1.0], 1e-6, 1e-6)

# invalid active-bound tolerances are rejected instead of silently changing
# the covariance and the residual degrees of freedom
toleranceKey = "LinearLeastSquaresCalibration-ActiveBoundTolerance"
toleranceDefault = ot.ResourceMap.GetAsScalar(toleranceKey)
try:
    for invalidTolerance in [0.0, -1.0, float("inf"), float("nan")]:
        ot.ResourceMap.SetAsScalar(toleranceKey, invalidTolerance)
        algoInvalid = ot.LinearLeastSquaresCalibration(calibrationModel, x, y, candidate, "SVD")
        algoInvalid.setBounds(bounds)
        with ott.assert_raises(TypeError):
            algoInvalid.run()
finally:
    ot.ResourceMap.SetAsScalar(toleranceKey, toleranceDefault)

# save/load round trip
if HAVE_LIBXML2:
    algoSave = ot.LinearLeastSquaresCalibration(calibrationModel, x, y, candidate, "SVD")
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
    ott.assert_almost_equal(
        loadedAlgo.getResult().getParameter(), [0.5, 3.5], boundedMapRtol
    )
    os.remove(studyFile)
