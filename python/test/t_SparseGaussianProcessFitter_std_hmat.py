#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

from openturns.experimental import SparseGaussianProcessFitter
from openturns.experimental import SparseGaussianProcessRegression
from openturns.experimental import SparseGaussianProcessFitterResult

ot.TESTPREAMBLE()


# The HMAT linear algebra method must give results close to LAPACK up to
# the H-matrix approximation accuracy (default assembly epsilon 1e-4)
def _data():
    ot.RandomGenerator.SetSeed(0)
    f = ot.SymbolicFunction(["x"], ["x + x * sin(x)"])
    X = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0]])
    Y = f(X)
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    return X, Y, covarianceModel


def test_hmat_elbo():
    X, Y, covarianceModel = _data()
    elbo = {}
    for method in (SparseGaussianProcessFitterResult.LAPACK, SparseGaussianProcessFitterResult.HMAT):
        algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
        algo.setNoiseStdDev(1e-2)
        algo.setOptimizeNoiseStdDev(False)
        algo.setMethod(method)
        algo.run()
        result = algo.getResult()
        assert result.getLinearAlgebraMethod() == method
        elbo[method] = result.getOptimalELBO()
    ott.assert_almost_equal(elbo[SparseGaussianProcessFitterResult.HMAT], elbo[SparseGaussianProcessFitterResult.LAPACK], 1e-3, 1e-3)


def test_hmat_prediction():
    X, Y, covarianceModel = _data()
    posteriorMean = {}
    posteriorCovariance = {}
    for method in (SparseGaussianProcessFitterResult.LAPACK, SparseGaussianProcessFitterResult.HMAT):
        algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
        algo.setNoiseStdDev(1e-2)
        algo.setOptimizeNoiseStdDev(False)
        algo.setMethod(method)
        algo.run()
        result = algo.getResult()
        posteriorMean[method] = result.getPosteriorMean()
        posteriorCovariance[method] = result.getPosteriorCovariance()
    ott.assert_almost_equal(posteriorMean[SparseGaussianProcessFitterResult.HMAT], posteriorMean[SparseGaussianProcessFitterResult.LAPACK], 1e-3, 1e-3)
    ott.assert_almost_equal(posteriorCovariance[SparseGaussianProcessFitterResult.HMAT], posteriorCovariance[SparseGaussianProcessFitterResult.LAPACK], 1e-3, 1e-3)
    # the conditional variance must be positive and lower than the prior variance
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    algo.setNoiseStdDev(1e-2)
    algo.setOptimizeNoiseStdDev(False)
    algo.setMethod(SparseGaussianProcessFitterResult.HMAT)
    algo.run()
    variance = algo.getResult().getConditionalVariance(ot.Point([1.5]))
    assert variance > 0.0
    assert variance <= covarianceModel.getAmplitude()[0] ** 2


def test_hmat_regression():
    X, Y, covarianceModel = _data()
    fit_algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    fit_algo.setNoiseStdDev(1e-8)
    fit_algo.setOptimizeNoiseStdDev(False)
    fit_algo.setMethod(SparseGaussianProcessFitterResult.HMAT)
    fit_algo.run()
    # regression from the HMAT result, the metamodel must interpolate the data
    algo = SparseGaussianProcessRegression(fit_algo.getResult())
    algo.run()
    result = algo.getResult()
    ott.assert_almost_equal(result.getMetaModel()(X), Y, 1e-3, 1e-3)
    # the HMAT metamodel gradient and hessian must be available
    Xtest = ot.Sample([[1.5], [4.0], [7.5]])
    assert result.getMetaModel().gradient(Xtest[0]).getNbRows() > 0
    assert result.getMetaModel().hessian(Xtest[0]).getNbSheets() > 0


test_hmat_elbo()
test_hmat_prediction()
test_hmat_regression()
