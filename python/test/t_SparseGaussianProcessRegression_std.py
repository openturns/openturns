#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

from openturns.experimental import SparseGaussianProcessFitter
from openturns.experimental import SparseGaussianProcessFitterResult
from openturns.experimental import SparseGaussianProcessRegression

ot.TESTPREAMBLE()


def _data():
    f = ot.SymbolicFunction(["x"], ["x + x * sin(x)"])
    X = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0]])
    Y = f(X)
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    return X, Y, covarianceModel


# The regression metamodel must interpolate the training data when the
# inducing points are set to the input sample
def test_interpolation():
    ot.RandomGenerator.SetSeed(0)
    X, Y, covarianceModel = _data()
    fit_algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    fit_algo.setNoiseStdDev(1e-8)
    fit_algo.setOptimizeNoiseStdDev(False)
    fit_algo.run()
    algo = SparseGaussianProcessRegression(fit_algo.getResult())
    algo.run()
    result = algo.getResult()
    ott.assert_almost_equal(result.getMetaModel()(X), Y, 1e-3, 1e-4)
    # the conditional variance must be positive and lower than the prior variance
    variance = result.getConditionalVariance(ot.Point([1.5]))
    assert variance > 0.0
    assert variance <= covarianceModel.getAmplitude()[0] ** 2


# The regression must give the exact posterior prediction for a test point
def test_prediction():
    ot.RandomGenerator.SetSeed(0)
    X, Y, covarianceModel = _data()
    fit_algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    fit_algo.setNoiseStdDev(1e-8)
    fit_algo.setOptimizeNoiseStdDev(False)
    fit_algo.run()
    algo = SparseGaussianProcessRegression(fit_algo.getResult())
    algo.run()
    result = algo.getResult()
    ott.assert_almost_equal(result.getMetaModel()(ot.Point([1.5])), [2.34812], 1e-4, 1e-5)
    ott.assert_almost_equal(result.getConditionalVariance(ot.Point([1.5])), 0.177111, 1e-4, 1e-6)


# The regression built from samples must optimize the hyperparameters
def test_sparse_regression():
    ot.RandomGenerator.SetSeed(0)
    X, Y, covarianceModel = _data()
    Z = ot.Sample([[1.0], [3.0], [5.0]])
    algo = SparseGaussianProcessRegression(X, Y, covarianceModel, Z)
    algo.run()
    result = algo.getResult()
    assert result.getInducingPoints().getSize() == 3
    assert result.getOptimalELBO() < 0.0
    # the metamodel must be consistent with the posterior moments
    posteriorMean = result.getPosteriorMean()
    posteriorCovariance = result.getPosteriorCovariance()
    assert posteriorMean.getDimension() == 3
    assert posteriorCovariance.getDimension() == 3
    assert posteriorCovariance.isPositiveDefinite()
    # the metamodel must give a finite prediction on a test point
    y = result.getMetaModel()(ot.Point([1.5]))
    assert abs(y[0]) < 1e10
    assert y.getDimension() == 1


# The metamodel hessian must be consistent with the finite-difference reference
def test_hessian():
    ot.RandomGenerator.SetSeed(0)
    f = ot.SymbolicFunction(
        ["x0", "x1", "x2"], ["x0 * sin(x1) + x2^2 + 0.3 * x0 * x1"]
    )
    X = ot.Sample(
        [
            [1.0, 2.0, 0.5],
            [3.0, 1.0, 1.5],
            [5.0, 0.5, 2.5],
            [2.0, 3.0, 0.3],
            [4.0, 1.8, 1.2],
            [1.5, 2.5, 2.0],
        ]
    )
    Y = f(X)
    covarianceModel = ot.SquaredExponential([1.0, 1.0, 1.0])
    fit_algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    fit_algo.setNoiseStdDev(1e-3)
    fit_algo.setOptimizeNoiseStdDev(False)
    fit_algo.run()
    algo = SparseGaussianProcessRegression(fit_algo.getResult())
    algo.run()
    metaModel = algo.getResult().getMetaModel()
    x = ot.Point([2.3, 1.4, 1.7])
    hessian = metaModel.hessian(x)
    reference = ot.CenteredFiniteDifferenceHessian(1e-4, metaModel.getEvaluation()).hessian(x)
    ott.assert_almost_equal(hessian, reference, 1e-5, 1e-5)


# getMethod / setMethod must store and retrieve correctly
def test_method_accessor():
    X, Y, covarianceModel = _data()
    Z = ot.Sample([[1.0], [3.0], [5.0]])
    algo = SparseGaussianProcessRegression(X, Y, covarianceModel, Z)
    assert algo.getMethod() == SparseGaussianProcessFitterResult.LAPACK
    algo.setMethod(SparseGaussianProcessFitterResult.LAPACK)
    assert algo.getMethod() == SparseGaussianProcessFitterResult.LAPACK


# __repr__ / __str__ must not throw
def test_regression_repr_str():
    X, Y, covarianceModel = _data()
    Z = ot.Sample([[1.0], [3.0], [5.0]])
    algo = SparseGaussianProcessRegression(X, Y, covarianceModel, Z)
    assert len(str(algo)) > 0
    assert len(repr(algo)) > 0


# The metamodel must reject evaluation at wrong input dimension
def test_metamodel_wrong_input_dim():
    X, Y, covarianceModel = _data()
    fit_algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    fit_algo.setNoiseStdDev(1e-2)
    fit_algo.setOptimizeNoiseStdDev(False)
    fit_algo.run()
    algo = SparseGaussianProcessRegression(fit_algo.getResult())
    algo.run()
    meta = algo.getResult().getMetaModel()
    with ott.assert_raises((TypeError, RuntimeError)):
        meta(ot.Point([1.0, 2.0]))


# The metamodel gradient must reject evaluation at wrong input dimension
def test_gradient_wrong_input_dim():
    X, Y, covarianceModel = _data()
    fit_algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    fit_algo.setNoiseStdDev(1e-2)
    fit_algo.setOptimizeNoiseStdDev(False)
    fit_algo.run()
    algo = SparseGaussianProcessRegression(fit_algo.getResult())
    algo.run()
    meta = algo.getResult().getMetaModel()
    with ott.assert_raises((TypeError, RuntimeError)):
        meta.gradient(ot.Point([1.0, 2.0]))


# The metamodel hessian must reject evaluation at wrong input dimension
def test_hessian_wrong_input_dim():
    X, Y, covarianceModel = _data()
    fit_algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    fit_algo.setNoiseStdDev(1e-2)
    fit_algo.setOptimizeNoiseStdDev(False)
    fit_algo.run()
    algo = SparseGaussianProcessRegression(fit_algo.getResult())
    algo.run()
    meta = algo.getResult().getMetaModel()
    with ott.assert_raises((TypeError, RuntimeError)):
        meta.hessian(ot.Point([1.0, 2.0]))


# getConditionalVariance must reject wrong input dimension
def test_conditional_variance_wrong_input_dim():
    X, Y, covarianceModel = _data()
    fit_algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    fit_algo.setNoiseStdDev(1e-2)
    fit_algo.setOptimizeNoiseStdDev(False)
    fit_algo.run()
    algo = SparseGaussianProcessRegression(fit_algo.getResult())
    algo.run()
    result = algo.getResult()
    with ott.assert_raises((TypeError, RuntimeError)):
        result.getConditionalVariance(ot.Point([1.0, 2.0]))


if __name__ == "__main__":
    test_interpolation()
    test_prediction()
    test_sparse_regression()
    test_hessian()
    test_method_accessor()
    test_regression_repr_str()
    test_metamodel_wrong_input_dim()
    test_gradient_wrong_input_dim()
    test_hessian_wrong_input_dim()
    test_conditional_variance_wrong_input_dim()
