#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(3)

size = 100
x = ot.Sample(size, 0)

g = ot.SymbolicFunction(
    ["a", "b", "c"],
    [
        "a +  -1.0  * b +  1.0  * c",
        "a +  -0.6  * b +  0.36  * c",
        "a +  -0.2  * b +  0.04  * c",
        "a +  0.2  * b +  0.04  * c",
        "a +  0.6  * b +  0.36  * c",
        "a +  1.0  * b +  1.0  * c",
    ],
)

outputDimension = g.getOutputDimension()

trueParameter = [2.8, 1.2, 0.5]
params = [0, 1, 2]
model = ot.ParametricFunction(g, params, trueParameter)

x = ot.Sample(size, 0)
y = model(x)
outputObservationNoiseSigma = 0.05
meanNoise = ot.Point(outputDimension)
covarianceNoise = ot.Point(outputDimension, outputObservationNoiseSigma)
R = ot.IdentityMatrix(outputDimension)
observationOutputNoise = ot.Normal(meanNoise, covarianceNoise, R)

# Add noise
sampleNoise = observationOutputNoise.getSample(size)
y += sampleNoise

candidate = [1.0] * 3
priorCovariance = ot.CovarianceMatrix(3)
for i in range(3):
    priorCovariance[i, i] = 3.0 + (1.0 + i) * (1.0 + i)
    for j in range(i):
        priorCovariance[i, j] = 1.0 / (1.0 + i + j)
errorCovariance = ot.CovarianceMatrix(outputDimension)
for i in range(outputDimension):
    errorCovariance[i, i] = 0.1 * (2.0 + (1.0 + i) * (1.0 + i))
    for j in range(i):
        errorCovariance[i, j] = 0.1 / (1.0 + i + j)
globalErrorCovariance = ot.CovarianceMatrix(outputDimension * size)
for i in range(outputDimension * size):
    globalErrorCovariance[i, i] = 0.1 * (2.0 + (1.0 + i) * (1.0 + i))
    for j in range(i):
        globalErrorCovariance[i, j] = 0.1 / (1.0 + i + j)
for bootstrapSize in [0, 30]:
    # 1. Constructor 1
    algo = ot.GaussianNonLinearCalibration(
        model, x, y, candidate, priorCovariance, errorCovariance
    )
    algo.setBootstrapSize(bootstrapSize)
    algo.run()
    # To avoid discrepance between the platforms with or without CMinpack
    # Check MAP
    calibrationResult = algo.getResult()
    parameterMAP = calibrationResult.getParameterMAP()
    print("(Auto) MAP=", repr(parameterMAP))
    rtol = 0.0
    atol = 0.5
    ott.assert_almost_equal(parameterMAP, trueParameter, rtol, atol)
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
    # To avoid discrepance between the platforms with or without CMinpack
    # Check MAP
    calibrationResult = algo.getResult()
    parameterMAP = calibrationResult.getParameterMAP()
    print("(Multistart/TNC) MAP=", repr(parameterMAP))
    rtol = 0.0
    atol = 0.5
    ott.assert_almost_equal(parameterMAP, trueParameter, rtol, atol)
    # 2. With globalErrorCovariance
    algo = ot.GaussianNonLinearCalibration(
        model, x, y, candidate, priorCovariance, globalErrorCovariance
    )
    algo.setBootstrapSize(bootstrapSize)
    algo.run()
    calibrationResult = algo.getResult()
    parameterMAP = calibrationResult.getParameterMAP()
    # print("(Global) MAP=", repr(parameterMAP))
    rtol = 0.0
    atol = 0.5
    ref = [2.61, 1.2, 0.731]
    ott.assert_almost_equal(parameterMAP, ref, rtol, atol)
