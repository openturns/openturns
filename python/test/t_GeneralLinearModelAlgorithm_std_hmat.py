#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from openturns.testing import assert_almost_equal

ot.TESTPREAMBLE()

try:

    # Set precision
    ot.PlatformInfo.SetNumericalPrecision(3)
    ot.ResourceMap.Set("GeneralLinearModelAlgorithm-LinearAlgebra", "HMAT")

    # Test 1
    print("========================")
    print("Test standard using HMat")
    print("========================")
    sampleSize = 6
    inputDimension = 1

    # Create the function to estimate
    input_description = ["x0"]
    formulas = ["x0"]
    model = ot.SymbolicFunction(input_description, formulas)

    X = ot.Sample(sampleSize, inputDimension)
    X2 = ot.Sample(sampleSize, inputDimension)
    for i in range(sampleSize):
        X[i, 0] = 3.0 + i
        X2[i, 0] = 2.5 + i
    X[0, 0] = 1.0
    X[1, 0] = 3.0
    X2[0, 0] = 2.0
    X2[1, 0] = 4.0
    Y = model(X)
    # Data validation
    Y2 = model(X2)
    for i in range(sampleSize):
        # Add a small noise to data
        Y[i, 0] += 0.01 * ot.DistFunc.rNormal()

    basis = ot.LinearBasisFactory(inputDimension).build()
    covarianceModel = ot.DiracCovarianceModel(inputDimension)
    algo = ot.GeneralLinearModelAlgorithm(X, Y, covarianceModel, basis)
    algo.run()

    # perform an evaluation
    result = algo.getResult()
    metaModel = result.getMetaModel()
    conditionalCovariance = result.getCovarianceModel()
    residual = metaModel(X) - Y
    assert_almost_equal(residual.computeCenteredMoment(2),
                        [0.00013144], 1e-5, 1e-5)
    print("Test Ok")

except:
    import sys
    print("t_GeneralLinearModelAlgorithm_std_hmat.py",
          sys.exc_info()[0], sys.exc_info()[1])
