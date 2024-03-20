#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)


def matrixToSample(matrix):
    """Converts a matrix into a Sample, so that we can
    assert_almost_equal a Matrix"""
    size = matrix.getNbRows()
    dimension = matrix.getNbColumns()
    sample = ot.Sample(size, dimension)
    for i in range(size):
        for j in range(dimension):
            sample[i, j] = matrix[i, j]
    return sample


def local2globalCovariance(localErrorCovariance, size, outputDimension):
    """
    Converts a local covariance into a global covariance.
    size = inputObservations.getSize()
    outputDimension = modelObservations.getDimension()
    """
    covarianceDimension = size * outputDimension
    observationDimension = localErrorCovariance.getDimension()
    globalErrorCovariance = ot.CovarianceMatrix(covarianceDimension)
    for i in range(size):
        for j in range(observationDimension):
            for k in range(observationDimension):
                globalErrorCovariance[
                    i * observationDimension + j, i * observationDimension + k
                ] = localErrorCovariance[j, k]
    return globalErrorCovariance


"""
Thanks to - Antoine Dumas, Phimeca
References
- J. Lemaitre and J. L. Chaboche (2002) "Mechanics of solid materials" Cambridge University Press.
"""
ot.RandomGenerator.SetSeed(0)

# Create the function.
g = ot.SymbolicFunction(["strain", "R", "C", "gam"], ["R + C*(1-exp(-gam*strain))"])

# Define the random vector.
Strain = ot.Uniform(0, 0.07)
unknownR = 750
unknownC = 2750
unknownGamma = 10
R = ot.Dirac(unknownR)
C = ot.Dirac(unknownC)
Gamma = ot.Dirac(unknownGamma)

Strain.setDescription(["Strain"])
R.setDescription(["R"])
C.setDescription(["C"])
Gamma.setDescription(["Gamma"])

# Create the joint input distribution function.
inputRandomVector = ot.JointDistribution([Strain, R, C, Gamma])

# Create the Monte-Carlo sample.
sampleSize = 10
inputSample = inputRandomVector.getSample(sampleSize)
outputStress = g(inputSample)

# Generate observation noise.
stressObservationNoiseSigma = 40.0  # (Pa)
noiseSigma = ot.Normal(0.0, stressObservationNoiseSigma)
sampleNoiseH = noiseSigma.getSample(sampleSize)
observedStress = outputStress + sampleNoiseH
observedStrain = inputSample[:, 0]

# Set the calibration parameters
# Define the value of the reference values of the $\theta$ parameter.
# In the bayesian framework, this is called the mean of the *prior* gaussian distribution.
# In the data assimilation framework, this is called the *background*.
R = 700  # Exact : 750
C = 2500  # Exact : 2750
Gamma = 8.0  # Exact : 10
thetaPrior = ot.Point([R, C, Gamma])

# The following statement create the calibrated function from the model.
# The calibrated parameters Ks, Zv, Zm are at indices 1, 2, 3 in the inputs arguments of the model.
calibratedIndices = [1, 2, 3]
mycf = ot.ParametricFunction(g, calibratedIndices, thetaPrior)

# Gaussian linear calibration
# The standard deviation of the observations.
sigmaStress = 10.0  # (Pa)

# Define the covariance matrix of the output Y of the model.
localErrorCovariance = ot.CovarianceMatrix(1)
localErrorCovariance[0, 0] = sigmaStress**2

# Define the covariance matrix of the parameters $\theta$ to calibrate.
sigmaR = 0.1 * R
sigmaC = 0.1 * C
sigmaGamma = 0.1 * Gamma

priorCovariance = ot.CovarianceMatrix(3)
priorCovariance[0, 0] = sigmaR**2
priorCovariance[1, 1] = sigmaC**2
priorCovariance[2, 2] = sigmaGamma**2

methods = ["SVD", "QR", "Cholesky"]
for method in methods:
    print("method=", method)
    # 1. Local calibration
    # The `GaussianLinearCalibration` class performs the gaussian linear
    # calibration by linearizing the model in the neighbourhood of the prior.
    algo = ot.GaussianLinearCalibration(
        mycf,
        observedStrain,
        observedStress,
        thetaPrior,
        priorCovariance,
        localErrorCovariance,
        method,
    )

    # The `run` method computes the solution of the problem.
    algo.run()

    calibrationResult = algo.getResult()

    # Analysis of the results
    # Maximum A Posteriori estimator
    thetaMAP = calibrationResult.getParameterMAP()
    exactTheta = ot.Point([762.661, 3056.59, 8.52781])
    assert_almost_equal(thetaMAP, exactTheta)

    # Covariance matrix of theta
    thetaPosterior = calibrationResult.getParameterPosterior()
    covarianceThetaStar = matrixToSample(thetaPosterior.getCovariance())
    exactCovarianceTheta = ot.Sample(
        [
            [42.4899, 288.43, -1.70502],
            [288.43, 15977.1, -67.6046],
            [-1.70502, -67.6046, 0.294659],
        ]
    )
    assert_almost_equal(covarianceThetaStar, exactCovarianceTheta)

    print("result=", calibrationResult)

    # 2. Global covariance
    outputDimension = observedStress.getDimension()
    globalErrorCovariance = local2globalCovariance(
        localErrorCovariance, sampleSize, outputDimension
    )

    # The `GaussianLinearCalibration` class performs the gaussian linear
    # calibration by linearizing the model in the neighbourhood of the prior.
    algo = ot.GaussianLinearCalibration(
        mycf,
        observedStrain,
        observedStress,
        thetaPrior,
        priorCovariance,
        globalErrorCovariance,
        method,
    )

    # The `run` method computes the solution of the problem.
    algo.run()

    # Check other fields
    calibrationResult = algo.getResult()

    # Analysis of the results
    # Maximum A Posteriori estimator
    thetaMAP = calibrationResult.getParameterMAP()
    exactTheta = ot.Point([762.661, 3056.59, 8.52781])
    assert_almost_equal(thetaMAP, exactTheta)

    # Covariance matrix of theta
    thetaPosterior = calibrationResult.getParameterPosterior()
    covarianceThetaStar = matrixToSample(thetaPosterior.getCovariance())
    exactCovarianceTheta = ot.Sample(
        [
            [42.4899, 288.43, -1.70502],
            [288.43, 15977.1, -67.6046],
            [-1.70502, -67.6046, 0.294659],
        ]
    )
    assert_almost_equal(covarianceThetaStar, exactCovarianceTheta)

    # Check other fields
    print("result=", calibrationResult)
