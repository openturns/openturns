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
# The C++ assembly differentiates at the MAP: evaluate the Jacobian
# from a copy of the model set to thetaStar, not trueParameter
thetaModel = ot.ParametricFunction(g, params, list(thetaStar))
for i in range(len(x)):
    jacobian = thetaModel.parameterGradient(ot.Point(x[i]))
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
# Laplace with a singular Gram matrix under Cholesky: the Gram inverse must
# not be required on the Laplace path. Model theta^2 fitted to -1 has its
# optimum at theta=0 with zero Jacobian but definite full Hessian H=10.
ot.ResourceMap.SetAsString("LinearLeastSquaresCalibration-Method", "Cholesky")
flatG = ot.SymbolicFunction(["theta", "x"], ["theta^2"])
flatModel = ot.ParametricFunction(flatG, [0], [0.0])
flatX = [[0.0]] * 5
flatY = [[-1.0]] * 5
flatAlgo = ot.NonLinearLeastSquaresCalibration(flatModel, flatX, flatY, [0.0])
flatAlgo.setOptimizationAlgorithm(ot.TNC())
flatAlgo.setUseLaplaceApproximation(True)
flatAlgo.setBootstrapSize(0)
flatAlgo.run()
flatResult = flatAlgo.getResult()
ott.assert_almost_equal(flatResult.getParameterMAP(), [0.0], 1e-2, 1e-3)
flatCovariance = flatResult.getParameterPosterior().getCovariance()
ot.Normal([0.0], flatCovariance)
# Brute-force check of the Laplace assembly: cov * H == sigma2 with the
# full Hessian H = J'J + sum_k r_k Hess(r_k) at the MAP
flatThetaStar = flatResult.getParameterMAP()
flatResidualFunction = flatResult.getResidualFunction()
flatHessian = 0.0
flatResiduals = flatResidualFunction(flatThetaStar)
flatHessianTensor = flatResidualFunction.hessian(flatThetaStar)
for k in range(len(flatResiduals)):
    jacobian = flatModel.parameterGradient(ot.Point(flatX[k]))
    flatHessian += jacobian[0, 0] ** 2
    flatHessian += flatResiduals[k] * flatHessianTensor[0, 0, k]
flatSigma2 = sum(v * v for v in flatResiduals) / (
    len(flatResiduals) - len(flatThetaStar)
)
ott.assert_almost_equal(flatCovariance[0, 0] * flatHessian, flatSigma2, 1e-6, 1e-6)
ot.ResourceMap.Reset()
