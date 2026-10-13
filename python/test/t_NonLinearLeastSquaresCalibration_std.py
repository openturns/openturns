#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(2)

m = 10
x = [[0.5 + i] for i in range(m)]

inVars = ["a", "b", "c", "x"]
formulas = ["a + b * exp(c * x)", "(a * x^2 + b) / (c + x^2)"]
g = ot.SymbolicFunction(inVars, formulas)
trueParameter = [2.8, 1.2, 0.5]
params = [0, 1, 2]
model = ot.ParametricFunction(g, params, trueParameter)
y = model(x)
y += ot.Normal([0.0] * 2, [0.05] * 2, ot.IdentityMatrix(2)).getSample(m)
candidate = [1.0] * 3
bootstrapSizes = [0, 100]
for bootstrapSize in bootstrapSizes:
    algo = ot.NonLinearLeastSquaresCalibration(model, x, y, candidate)
    algo.setBootstrapSize(bootstrapSize)
    algo.run()
    result = algo.getResult()
    # To avoid discrepance between the platforms with or without CMinpack
    print("result (Auto)=", result.getParameterMAP())
    ott.assert_almost_equal(
        result.getObservationsError().getMean(), [0.0051, -0.0028], 1e-1, 1e-3
    )
    multiStartSize = 10
    algo.setOptimizationAlgorithm(
        ot.MultiStart(
            ot.TNC(),
            ot.LowDiscrepancyExperiment(
                ot.SobolSequence(),
                ot.Normal(
                    candidate, ot.CovarianceMatrix(ot.Point(candidate).getDimension())
                ),
                multiStartSize,
            ).generate(),
        )
    )
    algo.run()
    result = algo.getResult()
    # To avoid discrepance between the platforms with or without CMinpack
    print("result  (TNC)=", result.getParameterMAP())
    ott.assert_almost_equal(
        result.getObservationsError().getMean(), [0.0051, -0.0028], 1e-1, 1e-3
    )
    # Draw result
    graph = result.drawParameterDistributions()
    graph = result.drawResiduals()
    graph = result.drawObservationsVsInputs()
    graph = result.drawObservationsVsPredictions()

# Laplace approximation of the posterior covariance, see issue #3273
# (analytic posterior path: bootstrap disabled)
laplaceAlgo = ot.NonLinearLeastSquaresCalibration(model, x, y, candidate)
assert not laplaceAlgo.getUseLaplaceApproximation()
laplaceAlgo.setUseLaplaceApproximation(True)
assert laplaceAlgo.getUseLaplaceApproximation()
laplaceAlgo.setBootstrapSize(0)
laplaceAlgo.run()
laplaceResult = laplaceAlgo.getResult()
laplaceCovariance = laplaceResult.getParameterPosterior().getCovariance()
# a valid covariance: Normal construction requires definiteness
ot.Normal(candidate, laplaceCovariance)
gaussAlgo = ot.NonLinearLeastSquaresCalibration(model, x, y, candidate)
gaussAlgo.setBootstrapSize(0)
gaussAlgo.run()
gaussResult = gaussAlgo.getResult()
# the optimum is untouched, only the covariance changes
ott.assert_almost_equal(
    laplaceResult.getParameterMAP(), gaussResult.getParameterMAP(), 1e-12, 1e-12
)
gaussCovariance = gaussResult.getParameterPosterior().getCovariance()
# brute-force check of the Laplace assembly: H = J'J + sum_k r_k Hess(r_k).
# Both posteriors share the same residual variance, so L*H == G*gram
# with L the Laplace covariance, G the Gauss-Newton one and gram = J'J
thetaStar = laplaceResult.getParameterMAP()
residualFunction = laplaceResult.getResidualFunction()
residualHessian = residualFunction.hessian(thetaStar)
residuals = residualFunction(thetaStar)
parameterDimension = len(thetaStar)
gram = ot.Matrix(parameterDimension, parameterDimension)
for i in range(len(x)):
    jacobian = model.parameterGradient(ot.Point(x[i]))
    for a in range(parameterDimension):
        for b in range(parameterDimension):
            gram[a, b] += sum(
                jacobian[a, k] * jacobian[b, k] for k in range(len(y[0]))
            )
hessian = ot.Matrix(gram)
for k in range(len(residuals)):
    for a in range(parameterDimension):
        for b in range(parameterDimension):
            hessian[a, b] += residuals[k] * residualHessian[a, b, k]
left = ot.Matrix(laplaceCovariance) * hessian
right = ot.Matrix(gaussCovariance) * gram
for a in range(parameterDimension):
    for b in range(parameterDimension):
        ott.assert_almost_equal(left[a, b], right[a, b], 1e-6, 1e-6)
# on a model linear in its parameters both methods coincide
linearG = ot.SymbolicFunction(["a", "b", "x"], ["a + b * x", "a - b * x"])
linearModel = ot.ParametricFunction(linearG, [0, 1], [2.8, 1.2])
linearY = linearModel(x)
linearY += ot.Normal([0.0] * 2, [0.05] * 2, ot.IdentityMatrix(2)).getSample(m)
linearGauss = ot.NonLinearLeastSquaresCalibration(
    linearModel, x, linearY, [1.0, 1.0]
)
linearGauss.setBootstrapSize(0)
linearGauss.run()
linearLaplace = ot.NonLinearLeastSquaresCalibration(
    linearModel, x, linearY, [1.0, 1.0]
)
linearLaplace.setUseLaplaceApproximation(True)
linearLaplace.setBootstrapSize(0)
linearLaplace.run()
ott.assert_almost_equal(
    linearLaplace.getResult().getParameterPosterior().getCovariance(),
    linearGauss.getResult().getParameterPosterior().getCovariance(),
    1e-6,
    1e-6,
)
