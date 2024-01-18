#! /usr/bin/env python
"""
Consider a model exactly linear with respect to the parameters.
In this case, the LLSQ calibration performs as good as it can.
Considers a case without observed inputs.
"""

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

ot.RandomGenerator.SetSeed(0)


# This model is linear in (a, b, c) and identifiable.
# Derived from y = a + b * x + c * x^2 at x=[-1.0, -0.6, -0.2, 0.2, 0.6, 1.0]
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
inputDimension = g.getInputDimension()
outputDimension = g.getOutputDimension()

trueParameter = ot.Point([12.0, 7.0, -8])

parameterDimension = trueParameter.getDimension()

Theta1 = ot.Dirac(trueParameter[0])
Theta2 = ot.Dirac(trueParameter[1])
Theta3 = ot.Dirac(trueParameter[2])

inputRandomVector = ot.JointDistribution([Theta1, Theta2, Theta3])

candidate = ot.Point([8.0, 9.0, -6.0])

calibratedIndices = [0, 1, 2]
model = ot.ParametricFunction(g, calibratedIndices, candidate)

outputObservationNoiseSigma = 0.01
meanNoise = ot.Point(outputDimension)
covarianceNoise = ot.Point(outputDimension, outputObservationNoiseSigma)
R = ot.IdentityMatrix(outputDimension)
observationOutputNoise = ot.Normal(meanNoise, covarianceNoise, R)

size = 100
inputObservations = ot.Sample(size, 0)

# Generate exact outputs
inputSample = inputRandomVector.getSample(size)
outputStress = g(inputSample)
# Add noise
sampleNoiseH = observationOutputNoise.getSample(size)
outputObservations = outputStress + sampleNoiseH

priorCovariance = ot.CovarianceMatrix(inputDimension)
for i in range(inputDimension):
    priorCovariance[i, i] = 3.0 + (1.0 + i) * (1.0 + i)
    for j in range(i):
        priorCovariance[i, j] = 1.0 / (1.0 + i + j)
errorCovariance = ot.CovarianceMatrix(outputDimension)
for i in range(outputDimension):
    errorCovariance[i, i] = 2.0 + (1.0 + i) * (1.0 + i)
    for j in range(i):
        errorCovariance[i, j] = 1.0 / (1.0 + i + j)
globalErrorCovariance = ot.CovarianceMatrix(outputDimension * size)
for i in range(outputDimension * size):
    globalErrorCovariance[i, i] = 0.1 * (2.0 + (1.0 + i) * (1.0 + i))
    for j in range(i):
        globalErrorCovariance[i, j] = 0.1 / (1.0 + i + j)

methods = ["SVD", "QR", "Cholesky"]
for method in methods:
    print("method=", method)
    # 1. Check with local error covariance
    print("Local error covariance")
    algo = ot.GaussianLinearCalibration(
        model,
        inputObservations,
        outputObservations,
        candidate,
        priorCovariance,
        errorCovariance,
        method,
    )
    algo.run()
    calibrationResult = algo.getResult()

    # Analysis of the results
    # Maximum A Posteriori estimator
    parameterMAP = calibrationResult.getParameterMAP()
    print("MAP=", repr(parameterMAP))
    rtol = 0.0
    atol = 1.0
    ott.assert_almost_equal(parameterMAP, trueParameter, rtol, atol)

    # 2. Check with global error covariance
    print("Global error covariance")
    algo = ot.GaussianLinearCalibration(
        model,
        inputObservations,
        outputObservations,
        candidate,
        priorCovariance,
        globalErrorCovariance,
        method,
    )
    algo.run()
    calibrationResult = algo.getResult()

    # Analysis of the results
    # Maximum A Posteriori estimator
    parameterMAP = calibrationResult.getParameterMAP()
    print("MAP=", repr(parameterMAP))
    rtol = 0.0
    atol = 1.0
    ott.assert_almost_equal(parameterMAP, trueParameter, rtol, atol)
