#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from openturns.testing import assert_almost_equal

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)


def matrixToSample(matrix):
    '''Converts a matrix into a Sample, so that we can
    assert_almost_equal a Matrix'''
    size = matrix.getNbRows()
    dimension = matrix.getNbColumns()
    sample = ot.Sample(size, dimension)
    for i in range(size):
        for j in range(dimension):
            sample[i, j] = matrix[i, j]
    return sample


'''
Test the calibration of an exponential function with 3 parameters.
Test the three decompositions (QR, SVD, Cholesky) of the least
squares problem.
Test the local and global error covariances.
'''
m = 10
x = [[0.5 + i] for i in range(m)]

inVars = ["a", "b", "c", "x"]
formulas = ["a + b * exp(c * x)", "(a * x^2 + b) / (c + x^2)"]
model = ot.SymbolicFunction(inVars, formulas)
p_ref = [2.8, 1.2, 0.5]
params = [0, 1, 2]
modelX = ot.ParametricFunction(model, params, p_ref)
y = modelX(x)
y += ot.Normal([0.0]*2, [0.05]*2, ot.IdentityMatrix(2)).getSample(m)
candidate = [1.0]*3
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

methods = ["SVD", "QR", "Cholesky"]
for method in methods:
    print("method=", method)
    # 1. Check with local error covariance
    print("Local error covariance")
    algo = ot.GaussianLinearCalibration(
        modelX, x, y, candidate, priorCovariance, errorCovariance, method)
    algo.run()
    calibrationResult = algo.getResult()

    # Analysis of the results
    # Maximum A Posteriori estimator
    thetaMAP = calibrationResult.getParameterMAP()
    exactTheta = ot.Point([5.69186, 0.0832132, 0.992301])
    rtol = 1.e-2
    assert_almost_equal(thetaMAP, exactTheta, rtol)

    # Covariance matrix of theta
    thetaPosterior = calibrationResult.getParameterPosterior()
    covarianceThetaStar = matrixToSample(thetaPosterior.getCovariance())
    exactCovarianceTheta = ot.Sample(
        [[0.308302, -0.000665387, 6.81135e-05],
         [-0.000665387, 8.36243e-06, -8.86775e-07],
         [6.81135e-05, -8.86775e-07, 9.42234e-08]])
    assert_almost_equal(covarianceThetaStar, exactCovarianceTheta)

    # Check other fields
    print("result=", calibrationResult)

    # 2. Check with global error covariance
    print("Global error covariance")
    algo = ot.GaussianLinearCalibration(
        modelX, x, y, candidate, priorCovariance, globalErrorCovariance, method)
    algo.run()
    calibrationResult = algo.getResult()

    # Analysis of the results
    # Maximum A Posteriori estimator
    thetaMAP = calibrationResult.getParameterMAP()
    exactTheta = ot.Point([3.4397, 0.095908, 0.99096])
    rtol = 1.e-2
    assert_almost_equal(thetaMAP, exactTheta, rtol)

    # Covariance matrix of theta
    thetaPosterior = calibrationResult.getParameterPosterior()
    covarianceThetaStar = matrixToSample(thetaPosterior.getCovariance())
    exactCovarianceTheta = ot.Sample(
        [[1.27112112e+00, -4.52977089e-03,  4.71588017e-04],
         [-4.52977089e-03,  5.93651856e-04, -6.36371482e-05],
         [4.71588017e-04, -6.36371482e-05,  6.84130285e-06]])
    assert_almost_equal(covarianceThetaStar, exactCovarianceTheta)

    # Check other fields
    print("result=", calibrationResult)
