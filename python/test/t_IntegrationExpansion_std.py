#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
from openturns.testing import assert_almost_equal
from openturns.usecases import ishigami_function as ishigami_function

ot.TESTPREAMBLE()

im = ishigami_function.IshigamiModel()
expectedCoefficientsLinear = [3.5, 1.62542, 0, 0, 0, 0, 0, -0.594723, 0, 0,
                              -1.29064, 0, 0, 0, 0, 1.37242, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, -1.95229, 0, 0, 0,
                              0, 0.194929, 0, 0, 0, 0, -1.08975, 0, 0, 0, 0,
                              0, 0, 0, 0, 0.409177, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 1.35741, 0, 0, 0, 0, 0, 0, -0.0126684,
                              0, 0, 0, 0, 0.164588, 0, 0, 0, 0, 0, 0, 0, 0,
                              -0.324901, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.33939,
                              0, 0, 0, 0, 0, 0, 0, 0, 0.00046142, 0, 0, 0, 0,
                              -0.0106965, 0, 0, 0, 0, 0, 0, 0, 0, 0.0490707,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0.0459147, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
expectedCoefficientsHyper = [3.5, 1.62542, 0, 0, 0, -0.594723, 0, -1.29064, 0,
                             0, 0, 0, 0, 0, -1.95229, 0, 0.194929, 0, 0, 0, 0,
                             0, 0, 1.37242, 0, 0, 1.35741, 0, -0.0126684, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.33939, 0, 0, 0, 0,
                             0, 0, 0.409177, 0, 0.00046142, 0, 0, 0, -1.08975,
                             0, 0, 0, 0, 0, 0.0459147, 0, 0, 0, 0, 0, 0, 0]

# Extract the relevant elements of the Ishigami test-case
im = ishigami_function.IshigamiModel()
dimension = im.dim
model = im.model

marginals = [im.X1, im.X2, im.X3]
distribution = im.distributionX

# Create the orthogonal basis
polynomialCollection = [ot.LegendreFactory()] * dimension

enumerateFunction = ot.LinearEnumerateFunction(dimension)
productBasis = ot.OrthogonalProductPolynomialFactory(polynomialCollection, enumerateFunction)

# Create the doe
degree = 10
basisSize = enumerateFunction.getBasisSizeFromTotalDegree(degree)
samplingSize = 32768
marginalSize = degree + 5

doeList = [ot.LowDiscrepancyExperiment(ot.LowDiscrepancySequence(ot.SobolSequence()), distribution, samplingSize),
           ot.GaussProductExperiment(distribution, [marginalSize] * dimension)]

for doe in doeList:
    # Sampling
    inputSample, weights = doe.generateWithWeights()
    wMin = min(weights)
    wMax = max(weights)
    outputSample = model(inputSample)
    # Check the full constructors
    for methodName in ["SVD", "QR", "Cholesky"]:
        # Create the polynomial chaos algorithm using the full constructor
        algo = otexp.LeastSquaresExpansion(inputSample, weights, outputSample, distribution, productBasis, basisSize, methodName)
        algo.run()
        # Check the coefficients
        result = algo.getResult()
        coeffs = result.getCoefficients().asPoint()
        ref = expectedCoefficientsLinear[:coeffs.getSize()]
        err = (coeffs - ref).norm()
        rtol = 1.0e-3
        atol = 1.0e-3
        assert_almost_equal(err, 0.0, rtol, atol)
        if (wMin == wMax):
            # Create the polynomial chaos algorithm using the full constructor
            algo = otexp.LeastSquaresExpansion(inputSample, outputSample, distribution, productBasis, basisSize, methodName)
            algo.run()
            # Check the coefficients
            result = algo.getResult()
            coeffs = result.getCoefficients().asPoint()
            ref = expectedCoefficientsLinear[:coeffs.getSize()]
            err = (coeffs - ref).norm()
            rtol = 1.0e-3
            atol = 1.0e-3
            assert_almost_equal(err, 0.0, rtol, atol)
    # Create the polynomial chaos algorithm using the simplified constructor
    algo = otexp.LeastSquaresExpansion(inputSample, weights, outputSample, distribution)
    algo.run()
    # Check the coefficients
    result = algo.getResult()
    coeffs = result.getCoefficients().asPoint()
    ref = expectedCoefficientsHyper[:coeffs.getSize()]
    err = (coeffs - ref).norm()
    rtol = 1.0e-2
    atol = 1.0e-2
    assert_almost_equal(err, 0.0, rtol, atol)
    # Check the constructors assuming uniform weights
    if wMin == wMax:
        # Create the polynomial chaos algorithm using the simplified constructor
        algo = otexp.LeastSquaresExpansion(inputSample, outputSample, distribution)
        algo.run()
        # Check the coefficients
        result = algo.getResult()
        coeffs = result.getCoefficients().asPoint()
        ref = expectedCoefficientsHyper[:coeffs.getSize()]
        err = (coeffs - ref).norm()
        rtol = 1.0e-2
        atol = 1.0e-2
        assert_almost_equal(err, 0.0, rtol, atol)
