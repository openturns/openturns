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
p_ref = [2.8, 1.2, 0.5]
params = [0, 1, 2]
model = ot.ParametricFunction(g, params, p_ref)
y = model(x)
y += ot.Normal([0.0] * 2, [0.05] * 2, ot.IdentityMatrix(2)).getSample(m)

# Parametric function needed by the calibration algorithm (input and parameters are swapped)
input = [3]
calibrationModel = ot.ParametricFunction(g, input, [0.0] * len(input))

candidate = [1.0] * 3
priorCovariance = ot.CovarianceMatrix(3)
for i in range(3):
    priorCovariance[i, i] = 3.0 + (1.0 + i) * (1.0 + i)
    for j in range(i):
        priorCovariance[i, j] = 1.0 / (1.0 + i + j)
errorCovariance = ot.CovarianceMatrix(2)
for i in range(2):
    errorCovariance[i, i] = 2.0 + (1.0 + i) * (1.0 + i)
    for j in range(i):
        errorCovariance[i, j] = 1.0 / (1.0 + i + j)
globalErrorCovariance = ot.CovarianceMatrix(2 * m)
for i in range(2 * m):
    globalErrorCovariance[i, i] = 2.0 + (1.0 + i) * (1.0 + i)
    for j in range(i):
        globalErrorCovariance[i, j] = 1.0 / (1.0 + i + j)
bootstrapSizes = [0, 30]
for bootstrapSize in bootstrapSizes:
    algo = ot.GaussianNonLinearCalibration(
        calibrationModel, x, y, candidate, priorCovariance, errorCovariance
    )
    algo.setBootstrapSize(bootstrapSize)
    algo.run()
    # To avoid discrepancy between the platforms with or without CMinpack
    print("result   (Auto)=", algo.getResult().getParameterMAP())
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
    # To avoid discrepancy between the platforms with or without CMinpack
    print("result    (TNC)=", result.getParameterMAP())
    print("error=", result.getObservationsError())
    algo = ot.GaussianNonLinearCalibration(
        calibrationModel, x, y, candidate, priorCovariance, globalErrorCovariance
    )
    algo.setBootstrapSize(bootstrapSize)
    algo.run()
    result = algo.getResult()
    print("result (Global)=", result.getParameterMAP())
    # Draw result
    graph = result.drawParameterDistributions()
    graph = result.drawResiduals()
    graph = result.drawObservationsVsInputs()
    graph = result.drawObservationsVsPredictions()

# unobserved inputs
p_ref = [2.8, 1.2, 0.5, 2.0]
params = [0, 1, 2, 3]
model = ot.ParametricFunction(g, params, p_ref)
x = ot.Sample(m, 0)
y = model(x)
y += ot.Normal([0.0] * 2, [0.05] * 2, ot.IdentityMatrix(2)).getSample(m)
# Parametric function needed by the calibration algorithm (input and parameters are swapped)
input = []
calibrationModel = ot.ParametricFunction(g, input, [0.0] * len(input))
priorCovariance = ot.CovarianceMatrix(4)
for i in range(4):
    priorCovariance[i, i] = 3.0 + (1.0 + i) * (1.0 + i)
    for j in range(i):
        priorCovariance[i, j] = 1.0 / (1.0 + i + j)
candidate = [1.0] * 4
algo = ot.GaussianNonLinearCalibration(
    calibrationModel, x, y, candidate, priorCovariance, errorCovariance
)
algo.run()
result = algo.getResult()
ot.PlatformInfo.SetNumericalPrecision(2)
print("result (unobs.)=", result.getParameterMAP())
print("error=", result.getObservationsError())

# test output at mean
model.setParameter(result.getParameterPrior().getMean())
outputAtPriorMean = model(x)
ott.assert_almost_equal(result.getOutputAtPriorMean(), outputAtPriorMean)

model.setParameter(result.getParameterPosterior().getMean())
outputAtPosteriorMean = model(x)
ott.assert_almost_equal(result.getOutputAtPosteriorMean(), outputAtPosteriorMean)
