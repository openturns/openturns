#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

from openturns.experimental import SparseGaussianProcessFitter
from openturns.experimental import SparseGaussianProcessRegression
from openturns.experimental import SparseGaussianProcessFitterResult

ot.TESTPREAMBLE()


# The sparse gaussian process regression built from an HMAT fitter result
# must give predictions close to the LAPACK ones up to the H-matrix accuracy
def _data():
    ot.RandomGenerator.SetSeed(0)
    f = ot.SymbolicFunction(["x"], ["x + x * sin(x)"])
    X = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0]])
    Y = f(X)
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    return X, Y, covarianceModel


def test_regression_from_result():
    X, Y, covarianceModel = _data()
    Xtest = ot.Sample([[1.5], [4.0], [7.5]])
    prediction = {}
    variance = {}
    for method in (SparseGaussianProcessFitterResult.LAPACK, SparseGaussianProcessFitterResult.HMAT):
        fit_algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
        fit_algo.setNoiseStdDev(1e-2)
        fit_algo.setOptimizeNoiseStdDev(False)
        fit_algo.setMethod(method)
        fit_algo.run()
        algo = SparseGaussianProcessRegression(fit_algo.getResult())
        algo.run()
        result = algo.getResult()
        prediction[method] = result.getMetaModel()(Xtest)
        variance[method] = result.getConditionalVariance(Xtest)
    ott.assert_almost_equal(prediction[SparseGaussianProcessFitterResult.HMAT], prediction[SparseGaussianProcessFitterResult.LAPACK], 1e-3, 1e-3)
    ott.assert_almost_equal(variance[SparseGaussianProcessFitterResult.HMAT], variance[SparseGaussianProcessFitterResult.LAPACK], 1e-3, 1e-3)


def test_regression_raw_inputs():
    # the raw-input regression performs a fit with default settings (noise
    # variance optimized) in LAPACK; it must produce finite predictions,
    # note that the HMAT method requires fixed parameters (no noise
    # optimization) since the ELBO gradient is LAPACK-only
    X, Y, covarianceModel = _data()
    algo = SparseGaussianProcessRegression(X, Y, covarianceModel, X)
    algo.run()
    prediction = algo.getResult().getMetaModel()(X)
    assert all(map(lambda x: x == x and x != float("inf"), prediction.asPoint()))
    # the conditional variance must be positive
    variance = algo.getResult().getConditionalVariance(ot.Point([1.5]))
    assert variance > 0.0


test_regression_from_result()
test_regression_raw_inputs()
