#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m
import sys

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(2)

m = 10
x = [[0.5 + i] for i in range(m)]

inVars = ["a", "b", "c", "x"]
formulas = ["a + b * exp(c * x)", "(a * x^2 + b) / (c + x^2)"]
model = ot.SymbolicFunction(inVars, formulas)
p_ref = [2.8, 1.2, 0.5]
params = [0, 1, 2]
modelX = ot.ParametricFunction(model, params, p_ref)
y = modelX(x)
y += ot.Normal([0.0] * 2, [0.05] * 2, ot.IdentityMatrix(2)).getSample(m)
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
        modelX, x, y, candidate, priorCovariance, errorCovariance
    )
    algo.setBootstrapSize(bootstrapSize)
    algo.run()
    # To avoid discrepance between the plaforms with or without CMinpack
    print("result   (Auto)=", algo.getResult().getParameterMAP())
    algo.setOptimizationAlgorithm(
        ot.MultiStart(
            ot.TNC(),
            ot.LowDiscrepancyExperiment(
                ot.SobolSequence(),
                ot.Normal(
                    candidate, ot.CovarianceMatrix(ot.Point(candidate).getDimension())
                ),
                ot.ResourceMap.GetAsUnsignedInteger(
                    "GaussianNonLinearCalibration-MultiStartSize"
                ),
            ).generate(),
        )
    )
    algo.run()
    # To avoid discrepance between the plaforms with or without CMinpack
    print("result    (TNC)=", algo.getResult().getParameterMAP())
    print("error=", algo.getResult().getObservationsError())
    algo = ot.GaussianNonLinearCalibration(
        modelX, x, y, candidate, priorCovariance, globalErrorCovariance
    )
    algo.setBootstrapSize(bootstrapSize)
    algo.run()
    print("result (Global)=", algo.getResult().getParameterMAP())

# unobserved inputs
p_ref = [2.8, 1.2, 0.5, 2.0]
params = [0, 1, 2, 3]
modelX = ot.ParametricFunction(model, params, p_ref)
x = ot.Sample(m, 0)
y = modelX(x)
y += ot.Normal([0.0] * 2, [0.05] * 2, ot.IdentityMatrix(2)).getSample(m)
priorCovariance = ot.CovarianceMatrix(4)
for i in range(4):
    priorCovariance[i, i] = 3.0 + (1.0 + i) * (1.0 + i)
    for j in range(i):
        priorCovariance[i, j] = 1.0 / (1.0 + i + j)
candidate = [1.0] * 4
algo = ot.GaussianNonLinearCalibration(
    modelX, x, y, candidate, priorCovariance, errorCovariance
)
algo.run()
result = algo.getResult()
ot.PlatformInfo.SetNumericalPrecision(2)
print("result (unobs.)=", result.getParameterMAP())
print("error=", algo.getResult().getObservationsError())

# test output at mean
modelX.setParameter(algo.getResult().getParameterPrior().getMean())
outputAtPriorMean = modelX(x)
ott.assert_almost_equal(algo.getResult().getOutputAtPriorMean(), outputAtPriorMean)

modelX.setParameter(algo.getResult().getParameterPosterior().getMean())
outputAtPosteriorMean = modelX(x)
ott.assert_almost_equal(
    algo.getResult().getOutputAtPosteriorMean(), outputAtPosteriorMean
)
