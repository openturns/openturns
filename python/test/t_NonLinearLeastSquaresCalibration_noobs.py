#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(3)

size = 100

# no observations
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

Theta1 = ot.Dirac(trueParameter[0])
Theta2 = ot.Dirac(trueParameter[1])
Theta3 = ot.Dirac(trueParameter[2])

inputRandomVector = ot.ComposedDistribution([Theta1, Theta2, Theta3])

model = ot.ParametricFunction(g, params, trueParameter)

inputSample = inputRandomVector.getSample(size)
y = g(inputSample)

outputObservationNoiseSigma = 0.05
meanNoise = ot.Point(outputDimension)
covarianceNoise = ot.Point(outputDimension, outputObservationNoiseSigma)
R = ot.IdentityMatrix(outputDimension)
observationOutputNoise = ot.Normal(meanNoise, covarianceNoise, R)

# Add noise
sampleNoise = observationOutputNoise.getSample(size)
y += sampleNoise

candidate = [1.0] * 3
bootstrapSizes = [0, 100]
for bootstrapSize in bootstrapSizes:
    algo = ot.NonLinearLeastSquaresCalibration(model, x, y, candidate)
    algo.setBootstrapSize(bootstrapSize)
    algo.run()
    # To avoid discrepance between the plaforms with or without CMinpack
    # Check MAP
    calibrationResult = algo.getResult()
    parameterMAP = calibrationResult.getParameterMAP()
    print("(Auto) MAP=", repr(parameterMAP))
    rtol = 1.0e-2
    atol = 0.0
    ott.assert_almost_equal(parameterMAP, trueParameter, rtol, atol)
    algo.setOptimizationAlgorithm(
        ot.MultiStart(
            ot.TNC(),
            ot.LowDiscrepancyExperiment(
                ot.SobolSequence(),
                ot.Normal(
                    candidate, ot.CovarianceMatrix(
                        ot.Point(candidate).getDimension())
                ),
                ot.ResourceMap.GetAsUnsignedInteger(
                    "NonLinearLeastSquaresCalibration-MultiStartSize"
                ),
            ).generate(),
        )
    )
    algo.run()
    # To avoid discrepance between the plaforms with or without CMinpack
    calibrationResult = algo.getResult()
    parameterMAP = calibrationResult.getParameterMAP()
    print("(Multistart/TNC) MAP=", repr(parameterMAP))
    rtol = 1.0e-2
    atol = 0.0
    ott.assert_almost_equal(parameterMAP, trueParameter, rtol, atol)
