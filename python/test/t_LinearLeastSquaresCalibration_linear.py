#! /usr/bin/env python
"""
Consider a model exactly linear with respect to the parameters.
In this case, the LLSQ calibration performs as good as it can.
"""

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

ot.RandomGenerator.SetSeed(0)


def modelLineaire(X):
    x, theta1, theta2, theta3 = X
    y = theta1 + theta2 * x + theta3 * x**2
    return [y]


g = ot.PythonFunction(4, 1, modelLineaire)

trueParameter = ot.Point([12.0, 7.0, -8])

parameterDimension = trueParameter.getDimension()

X = ot.Uniform()
Theta1 = ot.Dirac(trueParameter[0])
Theta2 = ot.Dirac(trueParameter[1])
Theta3 = ot.Dirac(trueParameter[2])

inputRandomVector = ot.ComposedDistribution([X, Theta1, Theta2, Theta3])

candidate = ot.Point([8.0, 9.0, -6.0])

calibratedIndices = [1, 2, 3]
model = ot.ParametricFunction(g, calibratedIndices, candidate)

outputObservationNoiseSigma = 2.0  # (Pa)
observationOutputNoise = ot.Normal(0.0, outputObservationNoiseSigma)

size = 1000

# Generate exact outputs
inputSample = inputRandomVector.getSample(size)
outputStress = g(inputSample)
# Add noise
sampleNoiseH = observationOutputNoise.getSample(size)
outputObservations = outputStress + sampleNoiseH
# Calibrate
inputObservations = inputSample[:, 0]
algo = ot.LinearLeastSquaresCalibration(
    model, inputObservations, outputObservations, candidate, "SVD"
)
algo.run()
calibrationResult = algo.getResult()

# Check residual distribution
residualDistribution = calibrationResult.getObservationsError()
meanResidual = residualDistribution.getMean()[0]
assert meanResidual == 0.0
sigmaResidual = residualDistribution.getStandardDeviation()[0]
rtol = 0.0
atol = 5.0e-2
ott.assert_almost_equal(sigmaResidual, outputObservationNoiseSigma, rtol, atol)

# Check other fields
print("result=", calibrationResult)
