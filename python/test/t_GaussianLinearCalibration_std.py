#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import numpy as np
from openturns.testing import assert_almost_equal

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

def testCalibrationtExponential():
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
        algo = ot.GaussianLinearCalibration(modelX, x, y, candidate, priorCovariance, errorCovariance, method)
        algo.run()
        calibrationResult = algo.getResult()

        # Analysis of the results
        # Maximum A Posteriori estimator
        thetaMAP = calibrationResult.getParameterMAP()
        print("thetaMAP",thetaMAP)
        exactTheta = ot.Point([5.69186,0.0832132,0.992301])
        rtol = 1.e-2
        assert_almost_equal(thetaMAP, exactTheta, rtol)
        
        # Covariance matrix of theta
        thetaPosterior = calibrationResult.getParameterPosterior()
        covarianceThetaStar = np.array(thetaPosterior.getCovariance())
        exactCovarianceTheta = np.array(\
            [[ 0.308302, -0.000665387, 6.81135e-05 ], \
             [ -0.000665387, 8.36243e-06, -8.86775e-07 ], \
             [ 6.81135e-05, -8.86775e-07, 9.42234e-08 ]])
        assert_almost_equal(covarianceThetaStar, exactCovarianceTheta)
        
        # Check other fields
        print("result=", calibrationResult)
    
        # 2. Check with global error covariance
        print("Global error covariance")
        algo = ot.GaussianLinearCalibration(modelX, x, y, candidate, priorCovariance, globalErrorCovariance, method)
        algo.run()
        calibrationResult = algo.getResult()

        # Analysis of the results
        # Maximum A Posteriori estimator
        thetaMAP = calibrationResult.getParameterMAP()
        print("thetaMAP",thetaMAP)
        exactTheta = ot.Point([3.4397,0.095908,0.99096])
        rtol = 1.e-2
        assert_almost_equal(thetaMAP, exactTheta, rtol)
        
        # Covariance matrix of theta
        thetaPosterior = calibrationResult.getParameterPosterior()
        covarianceThetaStar = np.array(thetaPosterior.getCovariance())
        exactCovarianceTheta = np.array(\
            [[ 1.27112112e+00, -4.52977089e-03,  4.71588017e-04], \
             [-4.52977089e-03,  5.93651856e-04, -6.36371482e-05], \
             [ 4.71588017e-04, -6.36371482e-05,  6.84130285e-06]])
        assert_almost_equal(covarianceThetaStar, exactCovarianceTheta)
        
        # Check other fields
        print("result=", calibrationResult)
    return

def testCalibrationtChaboche():
    '''
    Thanks to - Antoine Dumas, Phimeca
    References
    - J. Lemaitre and J. L. Chaboche (2002) "Mechanics of solid materials" Cambridge University Press.
    '''
    ot.RandomGenerator.SetSeed(0)
    def modelChaboche(X):
        strain,R,C,gamma = X
        stress = R + C*(1-np.exp(-gamma*strain))
        return [stress]

    # Create the Python function.
    g = ot.PythonFunction(4, 1, modelChaboche) 
    
    # Define the random vector.
    Strain = ot.Uniform(0,0.07)
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
    inputRandomVector = ot.ComposedDistribution([Strain, R, C, Gamma])
    
    # Create the Monte-Carlo sample.
    sampleSize = 10
    inputSample = inputRandomVector.getSample(sampleSize)
    outputStress = g(inputSample)
        
    # Generate observation noise.
    stressObservationNoiseSigma = 40. # (Pa)
    noiseSigma = ot.Normal(0.,stressObservationNoiseSigma)
    sampleNoiseH = noiseSigma.getSample(sampleSize)
    observedStress = outputStress + sampleNoiseH
    observedStrain = inputSample[:,0]
    
    # Set the calibration parameters
    # Define the value of the reference values of the $\theta$ parameter. 
    # In the bayesian framework, this is called the mean of the *prior* gaussian distribution. 
    # In the data assimilation framework, this is called the *background*.
    R = 700 # Exact : 750
    C = 2500 # Exact : 2750
    Gamma = 8. # Exact : 10
    thetaPrior = ot.Point([R,C,Gamma])
    
    # The following statement create the calibrated function from the model. 
    # The calibrated parameters Ks, Zv, Zm are at indices 1, 2, 3 in the inputs arguments of the model.
    calibratedIndices = [1,2,3]
    mycf = ot.ParametricFunction(g, calibratedIndices, thetaPrior)
    
    # Gaussian linear calibration
    # The standard deviation of the observations.
    sigmaStress = 10. # (Pa)

    # Define the covariance matrix of the output Y of the model.
    errorCovariance = ot.CovarianceMatrix(1)
    errorCovariance[0,0] = sigmaStress**2
    
    # Define the covariance matrix of the parameters $\theta$ to calibrate.
    sigmaR = 0.1 * R
    sigmaC = 0.1 * C
    sigmaGamma = 0.1 * Gamma
    
    sigma = ot.CovarianceMatrix(3)
    sigma[0,0] = sigmaR**2
    sigma[1,1] = sigmaC**2
    sigma[2,2] = sigmaGamma**2
    
    # Calibration
    # The `GaussianLinearCalibration` class performs the gaussian linear 
    # calibration by linearizing the model in the neighbourhood of the prior.
    algo = ot.GaussianLinearCalibration(mycf, observedStrain, observedStress, \
                                        thetaPrior, sigma, errorCovariance)
    
    # The `run` method computes the solution of the problem.
    algo.run()
    
    calibrationResult = algo.getResult()
    
    # Analysis of the results
    # Maximum A Posteriori estimator
    thetaMAP = calibrationResult.getParameterMAP()
    print("Chaboche, thetaMAP",thetaMAP)
    exactTheta = ot.Point([762.661,3056.59,8.52781])
    assert_almost_equal(thetaMAP, exactTheta)
    
    # Covariance matrix of theta
    thetaPosterior = calibrationResult.getParameterPosterior()
    covarianceThetaStar = np.array(thetaPosterior.getCovariance())
    exactCovarianceTheta = np.array(\
        [[ 42.4899, 288.43, -1.70502 ],\
         [ 288.43, 15977.1, -67.6046 ],\
         [ -1.70502, -67.6046, 0.294659 ]])
    assert_almost_equal(covarianceThetaStar, exactCovarianceTheta)
    
    return

testCalibrationtExponential()
testCalibrationtChaboche()
