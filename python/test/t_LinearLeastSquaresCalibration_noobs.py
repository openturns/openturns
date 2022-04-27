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
outputDimension = g.getOutputDimension()

trueParameter = ot.Point([12.0, 7.0, -8])

parameterDimension = trueParameter.getDimension()

Theta1 = ot.Dirac(trueParameter[0])
Theta2 = ot.Dirac(trueParameter[1])
Theta3 = ot.Dirac(trueParameter[2])

inputRandomVector = ot.ComposedDistribution([Theta1, Theta2, Theta3])

candidate = ot.Point([8.0, 9.0, -6.0])

calibratedIndices = [0, 1, 2]
model = ot.ParametricFunction(g, calibratedIndices, candidate)

outputObservationNoiseSigma = 0.05
meanNoise = ot.Point(outputDimension)
covarianceNoise = ot.Point(outputDimension, outputObservationNoiseSigma)
R = ot.IdentityMatrix(outputDimension)
observationOutputNoise = ot.Normal(meanNoise, covarianceNoise, R)

size = 1000
inputObservations = ot.Sample(size, 0)

# Generate exact outputs
inputSample = inputRandomVector.getSample(size)
outputStress = g(inputSample)
# Add noise
sampleNoise = observationOutputNoise.getSample(size)
outputObservations = outputStress + sampleNoise
# Calibrate
algo = ot.LinearLeastSquaresCalibration(
    model, inputObservations, outputObservations, candidate, "SVD"
)
algo.run()
calibrationResult = algo.getResult()

# Check residual distribution
residualDistribution = calibrationResult.getObservationsError()
meanResidual = residualDistribution.getMean()[0]
assert meanResidual == 0.0
sigmaResidual = residualDistribution.getStandardDeviation()
print("sigmaResidual=", repr(sigmaResidual))
rtol = 0.0
atol = 5.0e-2
ott.assert_almost_equal(sigmaResidual, covarianceNoise, rtol, atol)

# Check MAP
parameterMAP = calibrationResult.getParameterMAP()
print("MAP=", repr(parameterMAP))
ott.assert_almost_equal(parameterMAP, trueParameter, 1.e-2)
