#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


def _data():
    sampleSize = 40
    inputDimension = 1
    # Create the function to estimate
    model = ot.SymbolicFunction(["x0"], ["x0"])
    X = ot.Sample(sampleSize, inputDimension)
    for i in range(sampleSize):
        X[i, 0] = 3.0 + (8.0 * i) / sampleSize
    Y = model(X)
    # Add a small noise to data
    Y += (
        ot.GaussianProcess(ot.AbsoluteExponential([0.1], [0.2]), ot.Mesh(X))
        .getRealization()
        .getValues()
    )
    return X, Y


# Case of a misspecified covariance model
def test_dirac_optimized(X, Y):
    """Optimization of the Dirac covariance model amplitude"""
    ot.ResourceMap.Reset()
    inputDimension = X.getDimension()
    basis = ot.LinearBasisFactory(inputDimension).build()
    covarianceModel = ot.DiracCovarianceModel(inputDimension)
    algo = ot.GeneralLinearModelAlgorithm(X, Y, covarianceModel, basis)
    algo.run()
    result = algo.getResult()
    ott.assert_almost_equal(
        result.getCovarianceModel().getAmplitude(), [0.19575], 1e-4, 1e-4
    )
    ott.assert_almost_equal(
        result.getTrendCoefficients(), [-0.110943, 1.01498], 1e-4, 1e-4
    )


# Now without estimating covariance parameters
def test_dirac_not_optimized(X, Y):
    """Fixed Dirac covariance model parameters"""
    ot.ResourceMap.Reset()
    inputDimension = X.getDimension()
    basis = ot.LinearBasisFactory(inputDimension).build()
    covarianceModel = ot.DiracCovarianceModel(inputDimension)
    algo = ot.GeneralLinearModelAlgorithm(X, Y, covarianceModel, basis, True)
    algo.setOptimizeParameters(False)
    algo.run()
    result = algo.getResult()
    ott.assert_almost_equal(
        result.getCovarianceModel().getAmplitude(), [1.0], 1e-4, 1e-4
    )
    ott.assert_almost_equal(
        result.getTrendCoefficients(), [-0.110943, 1.01498], 1e-4, 1e-4
    )


# Case of a well specified covariance model
# Test the optimization when the amplitude is deduced analytically from
# the scale
def test_reduced_unbiased(X, Y):
    """Scale optimization with analytical unbiased amplitude estimate"""
    ot.ResourceMap.Reset()
    inputDimension = X.getDimension()
    basis = ot.LinearBasisFactory(inputDimension).build()
    covarianceModel = ot.AbsoluteExponential(inputDimension)
    algo = ot.GeneralLinearModelAlgorithm(X, Y, covarianceModel, basis)
    algo.run()
    result = algo.getResult()
    ott.assert_almost_equal(
        result.getCovarianceModel().getParameter(), [0.132773, 0.195632], 1e-4, 1e-4
    )
    ott.assert_almost_equal(
        result.getTrendCoefficients(), [-0.103425, 1.01411], 1e-4, 1e-4
    )


def test_reduced_biased(X, Y):
    """Scale optimization with analytical biased amplitude estimate"""
    ot.ResourceMap.Reset()
    ot.ResourceMap.SetAsBool("GeneralLinearModelAlgorithm-UnbiasedVariance", False)
    inputDimension = X.getDimension()
    basis = ot.LinearBasisFactory(inputDimension).build()
    covarianceModel = ot.AbsoluteExponential(inputDimension)
    algo = ot.GeneralLinearModelAlgorithm(X, Y, covarianceModel, basis)
    algo.run()
    result = algo.getResult()
    ott.assert_almost_equal(
        result.getCovarianceModel().getParameter(), [0.132773, 0.190679], 1e-4, 1e-4
    )
    ott.assert_almost_equal(
        result.getTrendCoefficients(), [-0.103425, 1.01411], 1e-4, 1e-4
    )


def test_full_optim(X, Y):
    """Full optimization of scale and amplitude"""
    ot.ResourceMap.Reset()
    ot.ResourceMap.SetAsBool(
        "GeneralLinearModelAlgorithm-UseAnalyticalAmplitudeEstimate", False
    )
    inputDimension = X.getDimension()
    basis = ot.LinearBasisFactory(inputDimension).build()
    covarianceModel = ot.AbsoluteExponential(inputDimension)
    algo = ot.GeneralLinearModelAlgorithm(X, Y, covarianceModel, basis)
    # Define interval
    bounds = ot.Interval([1e-2] * 2, [100] * 2)
    algo.setOptimizationBounds(bounds)
    algo.run()
    result = algo.getResult()
    ott.assert_almost_equal(
        result.getCovarianceModel().getParameter(), [0.132775, 0.190699], 1e-4, 1e-4
    )
    ott.assert_almost_equal(
        result.getTrendCoefficients(), [-0.103425, 1.01411], 1e-4, 1e-4
    )


if __name__ == "__main__":
    X, Y = _data()
    test_dirac_optimized(X, Y)
    test_dirac_not_optimized(X, Y)
    test_reduced_unbiased(X, Y)
    test_reduced_biased(X, Y)
    test_full_optim(X, Y)
