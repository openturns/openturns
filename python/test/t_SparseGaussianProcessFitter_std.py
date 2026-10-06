#! /usr/bin/env python

import math
import os

import numpy as np

import openturns as ot
import openturns.testing as ott

from openturns.experimental import SparseGaussianProcessFitter
from openturns.experimental import SparseGaussianProcessFitterResult
from openturns.experimental import SparseGaussianProcessEvaluation
from openturns.experimental import SparseGaussianProcessGradient
from openturns.experimental import SparseGaussianProcessHessian

ot.TESTPREAMBLE()


def _sample():
    X = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0]])
    f = ot.SymbolicFunction(["x"], ["x + x * sin(x)"])
    return X, f(X)


def _numpy_elbo(X, Y, Z, sigma, cov):
    N, M = len(X), len(Z)
    sigma2 = sigma * sigma
    Kff = np.array([[cov.computeAsScalar(X[i], X[j]) for j in range(N)] for i in range(N)])
    Kuu = np.array([[cov.computeAsScalar(Z[i], Z[j]) for j in range(M)] for i in range(M)])
    Kfu = np.array([[cov.computeAsScalar(X[i], Z[j]) for j in range(M)] for i in range(N)])
    Luu = np.linalg.cholesky(Kuu)
    A = Kfu.dot(np.linalg.inv(Luu).T)
    G = A.T.dot(A)
    B = np.eye(M) * sigma2 + G
    Lb = np.linalg.cholesky(B)
    Aty = A.T.dot(Y)
    logdetB = 2.0 * np.sum(np.log(np.diag(Lb)))
    # stable quadratic term
    w = np.linalg.solve(G, Aty)
    u = np.linalg.solve(Lb, w)
    yperp = Y - A.dot(w)
    quadratic = w.dot(w) - sigma2 * u.dot(u) + yperp.dot(yperp) / sigma2
    # tighter regularization of Titsias (2025)
    resid = np.maximum(np.diag(Kff) - np.sum(A * A, axis=1), 0.0)
    log_term = 0.5 * np.sum(np.log1p(resid / sigma2))
    elbo = -0.5 * (
        N * np.log(2.0 * np.pi)
        + (N - M) * math.log(sigma2)
        + logdetB
        + quadratic
    ) - log_term
    return elbo


def _numpy_posterior(X, Y, Z, sigma, cov):
    M = len(Z)
    N = len(X)
    sigma2 = sigma * sigma
    Kuu = np.array([[cov.computeAsScalar(Z[i], Z[j]) for j in range(M)] for i in range(M)])
    Kfu = np.array([[cov.computeAsScalar(X[i], Z[j]) for j in range(M)] for i in range(N)])
    Luu = np.linalg.cholesky(Kuu)
    A = Kfu.dot(np.linalg.inv(Luu).T)
    G = A.T.dot(A)
    B = np.eye(M) * sigma2 + G
    Aty = A.T.dot(Y)
    m_w = np.linalg.solve(B, Aty)
    S_ww = sigma2 * np.linalg.solve(B, np.eye(M))
    return m_w, S_ww


def _hetero_variances():
    return ot.Point([0.01, 0.04, 0.02, 0.09, 0.05, 0.16])


def _numpy_elbo_hetero(X, Y, Z, variances, cov):
    N, M = len(X), len(Z)
    D = np.array(variances)
    Kff = np.array([[cov.computeAsScalar(X[i], X[j]) for j in range(N)] for i in range(N)])
    Kuu = np.array([[cov.computeAsScalar(Z[i], Z[j]) for j in range(M)] for i in range(M)])
    Kfu = np.array([[cov.computeAsScalar(X[i], Z[j]) for j in range(M)] for i in range(N)])
    Luu = np.linalg.cholesky(Kuu)
    A = Kfu.dot(np.linalg.inv(Luu).T)
    s = 1.0 / np.sqrt(D)
    As = A * s[:, None]
    Ys = Y * s
    G = As.T.dot(As)
    B = G + np.eye(M)
    Lb = np.linalg.cholesky(B)
    Atys = As.T.dot(Ys)
    logdetB = 2.0 * np.sum(np.log(np.diag(Lb)))
    w = np.linalg.solve(G, Atys)
    u = np.linalg.solve(Lb, w)
    yperp = Ys - As.dot(w)
    quadratic = w.dot(w) - u.dot(u) + yperp.dot(yperp)
    resid = np.maximum(np.diag(Kff) - np.sum(A * A, axis=1), 0.0)
    log_term = 0.5 * np.sum(np.log1p(resid / D))
    elbo = -0.5 * (
        N * np.log(2.0 * np.pi)
        + np.sum(np.log(D))
        + logdetB
        + quadratic
    ) - log_term
    return elbo


def _numpy_posterior_hetero(X, Y, Z, variances, cov):
    M = len(Z)
    D = np.array(variances)
    Kuu = np.array([[cov.computeAsScalar(Z[i], Z[j]) for j in range(M)] for i in range(M)])
    Kfu = np.array([[cov.computeAsScalar(X[i], Z[j]) for j in range(M)] for i in range(len(X))])
    Luu = np.linalg.cholesky(Kuu)
    A = Kfu.dot(np.linalg.inv(Luu).T)
    s = 1.0 / np.sqrt(D)
    As = A * s[:, None]
    Ys = Y * s
    B = As.T.dot(As) + np.eye(M)
    m_w = np.linalg.solve(B, As.T.dot(Ys))
    S_ww = np.linalg.inv(B)
    return m_w, S_ww


# The collapsed ELBO with M=N and Z=X must equal the exact GP log marginal likelihood
def test_elbo_matches_exact_log_likelihood():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    N = X.getSize()
    Xn = np.array(X)
    Yn = np.array(Y).ravel()
    for sigma in [1.0, 0.5, 0.1, 1e-2, 1e-6]:
        algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
        algo.setNoiseStdDev(sigma)
        algo.setOptimizeNoiseStdDev(False)
        algo.run()
        elbo = algo.getResult().getOptimalELBO()
        Kff = np.array(
            [[covarianceModel.computeAsScalar(Xn[i], Xn[j]) for j in range(N)] for i in range(N)]
        )
        Knp = Kff + np.eye(N) * sigma * sigma
        sign, logdet = np.linalg.slogdet(Knp)
        assert sign > 0.0
        exact = -0.5 * (
            N * np.log(2.0 * np.pi)
            + logdet
            + Yn.dot(np.linalg.solve(Knp, Yn))
        )
        ott.assert_almost_equal(elbo, exact, 1e-6, 1e-9)


# The collapsed ELBO for M<N must match the reference formula (tighter log regularization)
def test_elbo_matches_reference_for_sparse():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    Z = X[0:4]
    Xn, Yn = np.array(X), np.array(Y).ravel()
    for sigma in [0.5, 0.1, 1e-2]:
        algo = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
        algo.setNoiseStdDev(sigma)
        algo.setOptimizeNoiseStdDev(False)
        algo.run()
        elbo = algo.getResult().getOptimalELBO()
        reference = _numpy_elbo(Xn, Yn, np.array(Z), sigma, covarianceModel)
        ott.assert_almost_equal(elbo, reference, 1e-6, 1e-8)


# The variational posterior must match the reference posterior in the whitened space
def test_posterior_matches_reference():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    Z = X[0:4]
    sigma = 0.1
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo.setNoiseStdDev(sigma)
    algo.setOptimizeNoiseStdDev(False)
    algo.run()
    result = algo.getResult()
    m_w, S_ww = _numpy_posterior(np.array(X), np.array(Y).ravel(), np.array(Z), sigma, covarianceModel)
    ott.assert_almost_equal(np.array(result.getPosteriorMean()), m_w, 1e-6, 1e-9)
    ott.assert_almost_equal(np.array(result.getPosteriorCovariance()), S_ww, 1e-6, 1e-9)


# Predictions and conditional variance must match the sparse GP posterior moments
def test_prediction_matches_posterior():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    Z = X[0:4]
    sigma = 0.1
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo.setNoiseStdDev(sigma)
    algo.setOptimizeNoiseStdDev(False)
    algo.run()
    result = algo.getResult()
    m_w, S_ww = _numpy_posterior(np.array(X), np.array(Y).ravel(), np.array(Z), sigma, covarianceModel)
    x_test = ot.Point([1.5])
    kz = np.array(
        [covarianceModel.computeAsScalar(x_test, Z[j]) for j in range(4)]
    )
    Luu = np.linalg.cholesky(
        np.array(
            [
                [covarianceModel.computeAsScalar(Z[i], Z[j]) for j in range(4)]
                for i in range(4)
            ]
        )
    )
    a = np.linalg.solve(Luu, kz)
    mean = a.dot(m_w)
    var = covarianceModel.computeAsScalar(x_test, x_test) - a.dot(a) + a.dot(S_ww).dot(a)
    ott.assert_almost_equal(result.getMetaModel()(x_test), [float(mean)], 1e-6, 1e-9)
    ott.assert_almost_equal(result.getConditionalVariance(x_test), float(var), 1e-6, 1e-9)

    # exact posterior for M=N: mean 2.34789, variance 0.177189
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    algo.setNoiseStdDev(1e-2)
    algo.setOptimizeNoiseStdDev(False)
    algo.run()
    result = algo.getResult()
    ott.assert_almost_equal(result.getMetaModel()(x_test), [2.34789], 1e-4, 1e-5)
    ott.assert_almost_equal(result.getConditionalVariance(x_test), 0.177189, 1e-4, 1e-6)


# The ELBO must be non decreasing with respect to nested sets of inducing points
def test_elbo_monotonicity():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    sigma = 0.1
    previous = -1e300
    for M in range(1, 7):
        algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:M])
        algo.setNoiseStdDev(sigma)
        algo.setOptimizeNoiseStdDev(False)
        algo.run()
        elbo = algo.getResult().getOptimalELBO()
        assert elbo >= previous, "ELBO decreases when M increases"
        previous = elbo


# The optimization must improve the ELBO
def test_optimization_improves_elbo():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:4])
    algo.setNoiseStdDev(0.5)
    algo.run()
    optimized_elbo = algo.getResult().getOptimalELBO()
    algo2 = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:4])
    algo2.setNoiseStdDev(0.5)
    algo2.setOptimizeNoiseStdDev(False)
    algo2.run()
    fixed_elbo = algo2.getResult().getOptimalELBO()
    assert optimized_elbo >= fixed_elbo, "optimization degrades the ELBO"
    # the optimized noise variance must be consistent with the ELBO
    assert algo.getResult().getNoiseStdDev() > 0.0


# The inducing points must be optimized in the allowed range
def test_optimize_inducing_points():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    algo.setNoiseStdDev(0.5)
    algo.setOptimizeInducingPoints(True)
    algo.run()
    result = algo.getResult()
    inducingPoints = result.getInducingPoints()
    assert inducingPoints.getMin()[0] >= X.getMin()[0] - 0.1 * (X.getMax()[0] - X.getMin()[0])
    assert inducingPoints.getMax()[0] <= X.getMax()[0] + 0.1 * (X.getMax()[0] - X.getMin()[0])
    # optimizing the inducing points must improve the ELBO
    algo2 = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    algo2.setNoiseStdDev(0.5)
    algo2.setOptimizeNoiseStdDev(False)
    algo2.run()
    assert result.getOptimalELBO() >= algo2.getResult().getOptimalELBO()


# The analytic gradient of the ELBO must be close to a centered finite difference
def test_elbo_gradient():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    algo.setOptimizeParameters(False)
    algo.setNoiseStdDev(0.1)
    objective = algo.getObjectiveFunction()
    logNoise = math.log(0.1)
    epsilon = 1e-6
    fd = (objective(ot.Point([logNoise + epsilon]))[0] - objective(ot.Point([logNoise - epsilon]))[0]) / (2.0 * epsilon)
    analytic = objective.getGradient().gradient(ot.Point([logNoise]))[0, 0]
    ott.assert_almost_equal(analytic, fd, 1e-5, 1e-6)


# The analytic gradient of the ELBO wrt the covariance parameters, the noise
# variance and the inducing points must match a centered finite difference
def test_elbo_gradient_all_parameters():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setParameter([1.5, 2.0])
    Z = X[0:3]
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo.setNoiseStdDev(0.4)
    algo.setOptimizeNoiseStdDev(True)
    algo.setOptimizeInducingPoints(True)
    objective = algo.getObjectiveFunction()
    parameter = ot.Point([1.5, 2.0, math.log(0.4)])
    for i in range(len(Z)):
        parameter.add(Z[i][0])
    epsilon = 1e-5
    for i in range(len(parameter)):
        pointPlus = ot.Point(parameter)
        pointMinus = ot.Point(parameter)
        pointPlus[i] += epsilon
        pointMinus[i] -= epsilon
        fd = (objective(pointPlus)[0] - objective(pointMinus)[0])
        fd /= (2.0 * epsilon)
        analytic = objective.getGradient().gradient(parameter)[i, 0]
        ott.assert_almost_equal(analytic, fd, 1e-3, 1e-4)
    # same check when the number of inducing points equals the training size
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    algo.setNoiseStdDev(0.4)
    algo.setOptimizeNoiseStdDev(True)
    algo.setOptimizeInducingPoints(True)
    objective = algo.getObjectiveFunction()
    parameter = ot.Point([1.5, 2.0, math.log(0.4)])
    for i in range(len(X)):
        parameter.add(X[i][0])
    for i in range(len(parameter)):
        pointPlus = ot.Point(parameter)
        pointMinus = ot.Point(parameter)
        pointPlus[i] += epsilon
        pointMinus[i] -= epsilon
        fd = (objective(pointPlus)[0] - objective(pointMinus)[0])
        fd /= (2.0 * epsilon)
        analytic = objective.getGradient().gradient(parameter)[i, 0]
        ott.assert_almost_equal(analytic, fd, 1e-3, 1e-4)


# Constructor: input/output size mismatch (Sample constructor)
def test_constructor_mismatched_samples():
    X = ot.Sample([[1.0], [3.0], [5.0]])
    Y = ot.Sample([[1.0], [2.0]])
    covarianceModel = ot.SquaredExponential([1.0])
    with ott.assert_raises((TypeError, RuntimeError)):
        SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:2])


# Constructor: input/output size mismatch (integer constructor)
def test_constructor_mismatched_samples_integer():
    X = ot.Sample([[1.0], [3.0], [5.0]])
    Y = ot.Sample([[1.0], [2.0]])
    covarianceModel = ot.SquaredExponential([1.0])
    with ott.assert_raises((TypeError, RuntimeError)):
        SparseGaussianProcessFitter(X, Y, covarianceModel, 2)


# Constructor: too many inducing points via integer (greater than sample size)
def test_constructor_too_many_inducing_points_integer():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    with ott.assert_raises((TypeError, RuntimeError)):
        SparseGaussianProcessFitter(X, Y, covarianceModel, 100)


# Constructor: zero inducing points via integer
def test_constructor_zero_inducing_points_integer():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    with ott.assert_raises((TypeError, RuntimeError)):
        SparseGaussianProcessFitter(X, Y, covarianceModel, 0)


# Constructor: too many inducing points via Sample
def test_constructor_too_many_inducing_points_sample():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    tooMany = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0], [9.0]])
    with ott.assert_raises((TypeError, RuntimeError)):
        SparseGaussianProcessFitter(X, Y, covarianceModel, tooMany)


# Constructor: inducing point dimension mismatch
def test_constructor_inducing_points_wrong_dim():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    Z = ot.Sample([[1.0, 2.0], [3.0, 4.0], [5.0, 6.0]])
    with ott.assert_raises((TypeError, RuntimeError)):
        SparseGaussianProcessFitter(X, Y, covarianceModel, Z)


# Constructor: empty inducing points
def test_constructor_empty_inducing_points():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    Z = ot.Sample(0, 1)
    with ott.assert_raises((TypeError, RuntimeError)):
        SparseGaussianProcessFitter(X, Y, covarianceModel, Z)


# Constructor: empty output sample
def test_constructor_empty_output_sample():
    X = ot.Sample([[1.0], [3.0], [5.0]])
    Y = ot.Sample(0, 1)
    covarianceModel = ot.SquaredExponential([1.0])
    with ott.assert_raises((TypeError, RuntimeError)):
        SparseGaussianProcessFitter(X, Y, covarianceModel, X)


# setNoiseStdDev: zero
def test_set_noise_variance_zero():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    with ott.assert_raises((TypeError, RuntimeError)):
        algo.setNoiseStdDev(0.0)


# setNoiseStdDev: negative
def test_set_noise_variance_negative():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    with ott.assert_raises((TypeError, RuntimeError)):
        algo.setNoiseStdDev(-1.0)


# setInducingPoints: dimension mismatch
def test_set_inducing_points_wrong_dim():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    Z = ot.Sample([[1.0, 2.0], [3.0, 4.0]])
    with ott.assert_raises((TypeError, RuntimeError)):
        algo.setInducingPoints(Z)


# setInducingPoints: zero inducing points
def test_set_inducing_points_empty():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    Z = ot.Sample(0, 1)
    with ott.assert_raises((TypeError, RuntimeError)):
        algo.setInducingPoints(Z)


# setInducingPoints: too many inducing points
def test_set_inducing_points_too_many():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    Z = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0], [9.0]])
    with ott.assert_raises((TypeError, RuntimeError)):
        algo.setInducingPoints(Z)


# The objective function must reject a parameter vector with wrong size
def test_objective_wrong_param_size():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    algo.setNoiseStdDev(0.1)
    algo.setOptimizeNoiseStdDev(False)
    obj = algo.getObjectiveFunction()
    # right size is 1 (log noise) since parameters are fixed
    with ott.assert_raises((TypeError, RuntimeError)):
        obj(ot.Point([1.0, 2.0]))


# The metamodel must reject evaluation at wrong input dimension
def test_metamodel_wrong_input_dim():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    algo.setNoiseStdDev(0.1)
    algo.setOptimizeNoiseStdDev(False)
    algo.run()
    meta = algo.getResult().getMetaModel()
    with ott.assert_raises((TypeError, RuntimeError)):
        meta(ot.Point([1.0, 2.0]))


# The metamodel gradient must reject evaluation at wrong input dimension
def test_metamodel_gradient_wrong_input_dim():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    algo.setNoiseStdDev(0.1)
    algo.setOptimizeNoiseStdDev(False)
    algo.run()
    meta = algo.getResult().getMetaModel()
    with ott.assert_raises((TypeError, RuntimeError)):
        meta.gradient(ot.Point([1.0, 2.0]))


# The metamodel hessian must reject evaluation at wrong input dimension
def test_metamodel_hessian_wrong_input_dim():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    algo.setNoiseStdDev(0.1)
    algo.setOptimizeNoiseStdDev(False)
    algo.run()
    meta = algo.getResult().getMetaModel()
    with ott.assert_raises((TypeError, RuntimeError)):
        meta.hessian(ot.Point([1.0, 2.0]))


# getConditionalVariance must reject wrong input dimension
def test_conditional_variance_wrong_input_dim():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    algo.setNoiseStdDev(0.1)
    algo.setOptimizeNoiseStdDev(False)
    algo.run()
    result = algo.getResult()
    with ott.assert_raises((TypeError, RuntimeError)):
        result.getConditionalVariance(ot.Point([1.0, 2.0]))


# getOptimizeParameters / getOptimizeInducingPoints / getOptimizeNoiseStdDev
# must return the value set by the corresponding setter
def test_optimization_flag_accessors():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    # defaults
    assert algo.getOptimizeParameters() is True
    assert algo.getOptimizeInducingPoints() is False
    assert algo.getOptimizeNoiseStdDev() is True
    # after set
    algo.setOptimizeParameters(False)
    assert algo.getOptimizeParameters() is False
    algo.setOptimizeInducingPoints(True)
    assert algo.getOptimizeInducingPoints() is True
    algo.setOptimizeNoiseStdDev(False)
    assert algo.getOptimizeNoiseStdDev() is False


# getMethod / setMethod on the fitter must store and retrieve correctly
def test_method_accessor():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    # default is LAPACK
    assert algo.getMethod() == SparseGaussianProcessFitterResult.LAPACK
    algo.setMethod(SparseGaussianProcessFitterResult.LAPACK)
    assert algo.getMethod() == SparseGaussianProcessFitterResult.LAPACK


# getOptimizationAlgorithm / setOptimizationAlgorithm
def test_optimization_algorithm_accessor():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    solver = algo.getOptimizationAlgorithm()
    assert solver.getImplementation().getClassName() == "TNC"
    # set a different algorithm
    cobyla = ot.Cobyla()
    algo.setOptimizationAlgorithm(cobyla)
    solver2 = algo.getOptimizationAlgorithm()
    assert solver2.getImplementation().getClassName() == "Cobyla"


# getInducingPoints / setInducingPoints on the fitter
def test_inducing_points_accessor():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    Z = X[0:3]
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    ott.assert_almost_equal(algo.getInducingPoints(), Z, 0, 0)
    Z2 = X[0:4]
    algo.setInducingPoints(Z2)
    ott.assert_almost_equal(algo.getInducingPoints(), Z2, 0, 0)


# getCovarianceModel / getReducedCovarianceModel
def test_covariance_model_accessor():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    cov = algo.getCovarianceModel()
    ott.assert_almost_equal(cov.getScale(), covarianceModel.getScale(), 0, 0)
    ott.assert_almost_equal(cov.getAmplitude(), covarianceModel.getAmplitude(), 0, 0)
    reduced = algo.getReducedCovarianceModel()
    assert reduced.getInputDimension() == 1


# SparseGaussianProcessFitterResult accessors: getNoiseStdDev,
# getCovarianceModel, getWhiteningFactor
def test_fitter_result_accessors():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setParameter([1.5, 2.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:4])
    algo.setNoiseStdDev(0.3)
    algo.setOptimizeParameters(False)
    algo.setOptimizeNoiseStdDev(False)
    algo.run()
    result = algo.getResult()
    # getNoiseStdDev
    ott.assert_almost_equal(result.getNoiseStdDev(), 0.3, 1e-14, 1e-14)
    # getCovarianceModel
    cov = result.getCovarianceModel()
    ott.assert_almost_equal(cov.getParameter(), [1.5, 2.0], 1e-14, 1e-14)
    # getWhiteningFactor
    L = result.getWhiteningFactor()
    assert L.getNbRows() == 4
    assert L.getNbColumns() == 4
    # L must be lower triangular
    for i in range(4):
        for j in range(i + 1, 4):
            ott.assert_almost_equal(L[i, j], 0.0, 1e-14, 1e-14)


# __repr__ / __str__ must not throw
def test_repr_str():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    assert len(str(algo)) > 0
    assert len(repr(algo)) > 0


# __repr__ / __str__ on the result must not throw
def test_result_repr_str():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    algo.run()
    result = algo.getResult()
    assert len(str(result)) > 0
    assert len(repr(result)) > 0


# Save / load must preserve the result through a Study
def test_save_load():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:4])
    algo.setNoiseStdDev(0.1)
    algo.setOptimizeNoiseStdDev(False)
    algo.run()
    result = algo.getResult()
    filename = "test_sparse_gp_fitter_result.xml"
    study = ot.Study(filename)
    study.add("result", result)
    study.save()
    study2 = ot.Study(filename)
    study2.load()
    result2 = SparseGaussianProcessFitterResult()
    study2.fillObject("result", result2)
    ott.assert_almost_equal(result.getOptimalELBO(), result2.getOptimalELBO(), 1e-10, 1e-10)
    ott.assert_almost_equal(result.getNoiseStdDev(), result2.getNoiseStdDev(), 1e-14, 1e-14)
    ott.assert_almost_equal(result.getPosteriorMean(), result2.getPosteriorMean(), 1e-10, 1e-10)
    ott.assert_almost_equal(
        result.getPosteriorCovariance(), result2.getPosteriorCovariance(), 1e-10, 1e-10
    )
    ott.assert_almost_equal(
        result.getConditionalVariance(ot.Point([1.5])),
        result2.getConditionalVariance(ot.Point([1.5])),
        1e-10,
        1e-10,
    )
    os.remove(filename)


# Save / load must preserve the fitter itself through a Study
def test_fitter_save_load():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:4])
    algo.setNoiseStdDev(0.1)
    algo.setOptimizeNoiseStdDev(False)
    algo.run()
    filename = "test_sparse_gp_fitter.xml"
    study = ot.Study(filename)
    study.add("algo", algo)
    study.save()
    study2 = ot.Study(filename)
    study2.load()
    algo2 = SparseGaussianProcessFitter()
    study2.fillObject("algo", algo2)
    ott.assert_almost_equal(
        algo.getResult().getOptimalELBO(),
        algo2.getResult().getOptimalELBO(),
        1e-10,
        1e-10,
    )
    ott.assert_almost_equal(
        algo.getResult().getMetaModel()(ot.Point([1.5])),
        algo2.getResult().getMetaModel()(ot.Point([1.5])),
        1e-10,
        1e-10,
    )
    assert algo2.getMethod() == SparseGaussianProcessFitterResult.LAPACK
    os.remove(filename)


# The result must report the LAPACK linear algebra method by default
def test_result_linear_algebra_method():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    algo.setNoiseStdDev(0.1)
    algo.setOptimizeNoiseStdDev(False)
    algo.run()
    result = algo.getResult()
    method = result.getLinearAlgebraMethod()
    assert method == SparseGaussianProcessFitterResult.LAPACK


# setMethod must reject values outside the LAPACK/HMAT enumeration
def test_set_method_invalid():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    with ott.assert_raises((TypeError, RuntimeError)):
        algo.setMethod(999)


# The standalone Evaluation/Gradient/Hessian must agree with the metamodel
def test_direct_evaluation_gradient_hessian():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:4])
    algo.setNoiseStdDev(0.1)
    algo.setOptimizeNoiseStdDev(False)
    algo.run()
    result = algo.getResult()
    metaModel = result.getMetaModel()
    evaluation = SparseGaussianProcessEvaluation(
        result.getCovarianceModel(),
        result.getInducingPoints(),
        result.getWhiteningFactor(),
        result.getPosteriorMean(),
        result.getPosteriorCovariance(),
    )
    gradient = SparseGaussianProcessGradient(
        result.getCovarianceModel(),
        result.getInducingPoints(),
        result.getWhiteningFactor(),
        result.getPosteriorMean(),
    )
    hessian = SparseGaussianProcessHessian(
        result.getCovarianceModel(),
        result.getInducingPoints(),
        result.getWhiteningFactor(),
        result.getPosteriorMean(),
    )
    x = ot.Point([1.5])
    ott.assert_almost_equal(
        ot.Function(evaluation)(x), metaModel(x), 1e-12, 1e-12
    )
    # looser tolerance: without analytic gradient/hessian on the metamodel
    # these fall back to finite differences
    ott.assert_almost_equal(
        gradient.gradient(x), metaModel.gradient(x), 1e-6, 1e-6
    )
    ott.assert_almost_equal(
        hessian.hessian(x), metaModel.hessian(x), 1e-6, 1e-6
    )


def test_resource_map_default_noise_variance_zero():
    original = ot.ResourceMap.GetAsScalar(
        "SparseGaussianProcessFitter-DefaultNoiseStdDev"
    )
    ot.ResourceMap.SetAsScalar("SparseGaussianProcessFitter-DefaultNoiseStdDev", 0.0)
    try:
        with ott.assert_raises((TypeError, RuntimeError)):
            SparseGaussianProcessFitter()
    finally:
        ot.ResourceMap.SetAsScalar(
            "SparseGaussianProcessFitter-DefaultNoiseStdDev", original
        )


def test_resource_map_default_noise_variance_negative():
    original = ot.ResourceMap.GetAsScalar(
        "SparseGaussianProcessFitter-DefaultNoiseStdDev"
    )
    ot.ResourceMap.SetAsScalar(
        "SparseGaussianProcessFitter-DefaultNoiseStdDev", -1.0
    )
    try:
        with ott.assert_raises((TypeError, RuntimeError)):
            SparseGaussianProcessFitter()
    finally:
        ot.ResourceMap.SetAsScalar(
            "SparseGaussianProcessFitter-DefaultNoiseStdDev", original
        )


def test_resource_map_default_optimization_lower_bound_nonpositive():
    X, Y = _sample()
    cov = ot.MaternModel([1.0], 1.5)
    original = ot.ResourceMap.GetAsScalar(
        "SparseGaussianProcessFitter-DefaultOptimizationLowerBound"
    )
    ot.ResourceMap.SetAsScalar(
        "SparseGaussianProcessFitter-DefaultOptimizationLowerBound", 0.0
    )
    try:
        with ott.assert_raises((TypeError, RuntimeError)):
            SparseGaussianProcessFitter(X, Y, cov, 4)
    finally:
        ot.ResourceMap.SetAsScalar(
            "SparseGaussianProcessFitter-DefaultOptimizationLowerBound", original
        )


def test_resource_map_default_optimization_upper_bound_nonpositive():
    X, Y = _sample()
    cov = ot.MaternModel([1.0], 1.5)
    original = ot.ResourceMap.GetAsScalar(
        "SparseGaussianProcessFitter-DefaultOptimizationUpperBound"
    )
    ot.ResourceMap.SetAsScalar(
        "SparseGaussianProcessFitter-DefaultOptimizationUpperBound", -1.0
    )
    try:
        with ott.assert_raises((TypeError, RuntimeError)):
            SparseGaussianProcessFitter(X, Y, cov, 4)
    finally:
        ot.ResourceMap.SetAsScalar(
            "SparseGaussianProcessFitter-DefaultOptimizationUpperBound", original
        )


def test_resource_map_scale_factor_nonpositive():
    X, Y = _sample()
    cov = ot.MaternModel([1.0], 1.5)
    original_lo = ot.ResourceMap.GetAsScalar(
        "SparseGaussianProcessFitter-OptimizationLowerBoundScaleFactor"
    )
    ot.ResourceMap.SetAsScalar(
        "SparseGaussianProcessFitter-OptimizationLowerBoundScaleFactor", 0.0
    )
    try:
        with ott.assert_raises((TypeError, RuntimeError)):
            SparseGaussianProcessFitter(X, Y, cov, 4)
    finally:
        ot.ResourceMap.SetAsScalar(
            "SparseGaussianProcessFitter-OptimizationLowerBoundScaleFactor", original_lo
        )
    original_hi = ot.ResourceMap.GetAsScalar(
        "SparseGaussianProcessFitter-OptimizationUpperBoundScaleFactor"
    )
    ot.ResourceMap.SetAsScalar(
        "SparseGaussianProcessFitter-OptimizationUpperBoundScaleFactor", -1.0
    )
    try:
        with ott.assert_raises((TypeError, RuntimeError)):
            SparseGaussianProcessFitter(X, Y, cov, 4)
    finally:
        ot.ResourceMap.SetAsScalar(
            "SparseGaussianProcessFitter-OptimizationUpperBoundScaleFactor", original_hi
        )


def test_resource_map_noise_bounds_nonpositive():
    X, Y = _sample()
    cov = ot.MaternModel([1.0], 1.5)
    original_lo = ot.ResourceMap.GetAsScalar(
        "SparseGaussianProcessFitter-DefaultNoiseStdDevLowerBound"
    )
    ot.ResourceMap.SetAsScalar(
        "SparseGaussianProcessFitter-DefaultNoiseStdDevLowerBound", 0.0
    )
    try:
        with ott.assert_raises((TypeError, RuntimeError)):
            SparseGaussianProcessFitter(X, Y, cov, 4)
    finally:
        ot.ResourceMap.SetAsScalar(
            "SparseGaussianProcessFitter-DefaultNoiseStdDevLowerBound", original_lo
        )
    original_hi = ot.ResourceMap.GetAsScalar(
        "SparseGaussianProcessFitter-DefaultNoiseStdDevUpperBound"
    )
    ot.ResourceMap.SetAsScalar(
        "SparseGaussianProcessFitter-DefaultNoiseStdDevUpperBound", -1.0
    )
    try:
        with ott.assert_raises((TypeError, RuntimeError)):
            SparseGaussianProcessFitter(X, Y, cov, 4)
    finally:
        ot.ResourceMap.SetAsScalar(
            "SparseGaussianProcessFitter-DefaultNoiseStdDevUpperBound", original_hi
        )


# The Sample overload of getConditionalVariance must match the Point overload
def test_conditional_variance_sample():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    Z = X[0:4]
    sigma = 0.1
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo.setNoiseStdDev(sigma)
    algo.setOptimizeNoiseStdDev(False)
    algo.run()
    result = algo.getResult()
    sample = ot.Sample([[1.5], [4.0], [7.5]])
    varianceSample = result.getConditionalVariance(sample)
    assert len(varianceSample) == 3
    for i in range(3):
        ott.assert_almost_equal(
            varianceSample[i], result.getConditionalVariance(sample[i]), 1e-14, 1e-14
        )
        assert varianceSample[i] > 0.0


def test_resource_map_default_optimization_algorithm_invalid():
    X, Y = _sample()
    cov = ot.MaternModel([1.0], 1.5)
    original = ot.ResourceMap.GetAsString(
        "SparseGaussianProcessFitter-DefaultOptimizationAlgorithm"
    )
    ot.ResourceMap.SetAsString(
        "SparseGaussianProcessFitter-DefaultOptimizationAlgorithm", "InvalidAlgo"
    )
    try:
        with ott.assert_raises((TypeError, RuntimeError)):
            SparseGaussianProcessFitter(X, Y, cov, 4)
    finally:
        ot.ResourceMap.SetAsString(
            "SparseGaussianProcessFitter-DefaultOptimizationAlgorithm", original
        )


def test_resource_map_optimization_normalization_disabled():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    original = ot.ResourceMap.GetAsBool(
        "SparseGaussianProcessFitter-OptimizationNormalization"
    )
    ot.ResourceMap.SetAsBool(
        "SparseGaussianProcessFitter-OptimizationNormalization", False
    )
    try:
        algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:4])
        algo.setNoiseStdDev(0.5)
        algo.run()
        assert algo.getResult().getOptimalELBO() < 0.0
    finally:
        ot.ResourceMap.SetAsBool(
            "SparseGaussianProcessFitter-OptimizationNormalization", original
        )


def test_resource_map_linear_algebra_hmat():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    original = ot.ResourceMap.GetAsString(
        "SparseGaussianProcessFitter-LinearAlgebra"
    )
    ot.ResourceMap.SetAsString("SparseGaussianProcessFitter-LinearAlgebra", "HMAT")
    try:
        algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
        assert algo.getMethod() == SparseGaussianProcessFitterResult.HMAT
    finally:
        ot.ResourceMap.SetAsString(
            "SparseGaussianProcessFitter-LinearAlgebra", original
        )


# Constant per-observation variances must reproduce the homoscedastic fit
def test_heteroscedastic_constant_matches_homoscedastic():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    Z = X[0:4]
    sigma = 0.25
    # homoscedastic reference
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo.setNoiseStdDev(sigma)
    algo.setOptimizeNoiseStdDev(False)
    algo.run()
    reference = algo.getResult()
    # heteroscedastic with constant variances
    algo2 = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo2.setNoiseVariances(ot.Point([sigma * sigma] * X.getSize()))
    assert algo2.getOptimizeNoiseStdDev() is False
    algo2.run()
    result = algo2.getResult()
    ott.assert_almost_equal(
        result.getOptimalELBO(), reference.getOptimalELBO(), 1e-8, 1e-9
    )
    ott.assert_almost_equal(
        np.array(result.getPosteriorMean()),
        np.array(reference.getPosteriorMean()),
        1e-9,
        1e-7,
    )
    ott.assert_almost_equal(
        np.array(result.getPosteriorCovariance()),
        np.array(reference.getPosteriorCovariance()),
        1e-9,
        1e-7,
    )
    x_test = ot.Point([1.5])
    ott.assert_almost_equal(
        result.getMetaModel()(x_test), reference.getMetaModel()(x_test), 1e-9, 1e-7
    )
    ott.assert_almost_equal(
        result.getConditionalVariance(x_test),
        reference.getConditionalVariance(x_test),
        1e-9,
        1e-7,
    )
    # the M=N case must agree as well
    algo3 = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    algo3.setNoiseVariances(ot.Point([sigma * sigma] * X.getSize()))
    algo3.run()
    algo4 = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    algo4.setNoiseStdDev(sigma)
    algo4.setOptimizeNoiseStdDev(False)
    algo4.run()
    ott.assert_almost_equal(
        algo3.getResult().getOptimalELBO(),
        algo4.getResult().getOptimalELBO(),
        1e-8,
        1e-9,
    )


# The heteroscedastic ELBO with M=N and Z=X must equal the exact GP log
# marginal likelihood with diagonal noise
def test_heteroscedastic_elbo_matches_exact():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    N = X.getSize()
    Xn = np.array(X)
    Yn = np.array(Y).ravel()
    variances = _hetero_variances()
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    algo.setNoiseVariances(variances)
    algo.run()
    elbo = algo.getResult().getOptimalELBO()
    Kff = np.array(
        [[covarianceModel.computeAsScalar(Xn[i], Xn[j]) for j in range(N)] for i in range(N)]
    )
    Knp = Kff + np.diag(np.array(variances))
    sign, logdet = np.linalg.slogdet(Knp)
    assert sign > 0.0
    exact = -0.5 * (
        N * np.log(2.0 * np.pi)
        + logdet
        + Yn.dot(np.linalg.solve(Knp, Yn))
    )
    ott.assert_almost_equal(elbo, exact, 1e-6, 1e-9)


# The heteroscedastic ELBO for M<N must match the reference formula
def test_heteroscedastic_elbo_matches_reference_sparse():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    Z = X[0:4]
    variances = _hetero_variances()
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo.setNoiseVariances(variances)
    algo.run()
    elbo = algo.getResult().getOptimalELBO()
    reference = _numpy_elbo_hetero(
        np.array(X), np.array(Y).ravel(), np.array(Z), variances, covarianceModel
    )
    ott.assert_almost_equal(elbo, reference, 1e-6, 1e-8)


# The heteroscedastic variational posterior must match the reference posterior
def test_heteroscedastic_posterior_matches_reference():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    Z = X[0:4]
    variances = _hetero_variances()
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo.setNoiseVariances(variances)
    algo.run()
    result = algo.getResult()
    m_w, S_ww = _numpy_posterior_hetero(
        np.array(X), np.array(Y).ravel(), np.array(Z), variances, covarianceModel
    )
    ott.assert_almost_equal(np.array(result.getPosteriorMean()), m_w, 1e-6, 1e-9)
    ott.assert_almost_equal(np.array(result.getPosteriorCovariance()), S_ww, 1e-6, 1e-9)
    # predictions and conditional variance must match the posterior moments
    x_test = ot.Point([1.5])
    kz = np.array(
        [covarianceModel.computeAsScalar(x_test, Z[j]) for j in range(4)]
    )
    Luu = np.linalg.cholesky(
        np.array(
            [
                [covarianceModel.computeAsScalar(Z[i], Z[j]) for j in range(4)]
                for i in range(4)
            ]
        )
    )
    a = np.linalg.solve(Luu, kz)
    mean = a.dot(m_w)
    var = covarianceModel.computeAsScalar(x_test, x_test) - a.dot(a) + a.dot(S_ww).dot(a)
    ott.assert_almost_equal(result.getMetaModel()(x_test), [float(mean)], 1e-6, 1e-9)
    ott.assert_almost_equal(result.getConditionalVariance(x_test), float(var), 1e-6, 1e-9)


# The analytic heteroscedastic ELBO gradient must match a centered finite difference
def test_heteroscedastic_elbo_gradient():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setParameter([1.5, 2.0])
    Z = X[0:3]
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo.setNoiseVariances(_hetero_variances())
    algo.setOptimizeInducingPoints(True)
    objective = algo.getObjectiveFunction()
    parameter = ot.Point([1.5, 2.0])
    for i in range(len(Z)):
        parameter.add(Z[i][0])
    # no noise parameter in the optimization vector with fixed variances
    assert len(parameter) == objective.getInputDimension()
    epsilon = 1e-5
    for i in range(len(parameter)):
        pointPlus = ot.Point(parameter)
        pointMinus = ot.Point(parameter)
        pointPlus[i] += epsilon
        pointMinus[i] -= epsilon
        fd = (objective(pointPlus)[0] - objective(pointMinus)[0])
        fd /= (2.0 * epsilon)
        analytic = objective.getGradient().gradient(parameter)[i, 0]
        ott.assert_almost_equal(analytic, fd, 1e-3, 1e-4)
    # same check when the number of inducing points equals the training size
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    algo.setNoiseVariances(_hetero_variances())
    algo.setOptimizeInducingPoints(True)
    objective = algo.getObjectiveFunction()
    parameter = ot.Point([1.5, 2.0])
    for i in range(len(X)):
        parameter.add(X[i][0])
    for i in range(len(parameter)):
        pointPlus = ot.Point(parameter)
        pointMinus = ot.Point(parameter)
        pointPlus[i] += epsilon
        pointMinus[i] -= epsilon
        fd = (objective(pointPlus)[0] - objective(pointMinus)[0])
        fd /= (2.0 * epsilon)
        analytic = objective.getGradient().gradient(parameter)[i, 0]
        ott.assert_almost_equal(analytic, fd, 1e-3, 1e-4)


# setNoiseVariances must reject mismatched sizes and non-positive values
def test_noise_variances_invalid():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    with ott.assert_raises((TypeError, RuntimeError)):
        algo.setNoiseVariances(ot.Point([0.1, 0.2]))
    with ott.assert_raises((TypeError, RuntimeError)):
        algo.setNoiseVariances(ot.Point([0.1, 0.0, -0.2, 0.1, 0.1, 0.1]))
    with ott.assert_raises((TypeError, RuntimeError)):
        algo.setNoiseVariances(ot.Point([0.0] * X.getSize()))


# Enabling the noise optimization with fixed variances must raise,
# and clearing the variances must restore the homoscedastic likelihood
def test_noise_variances_optimization_flag():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    algo.setNoiseVariances(_hetero_variances())
    assert algo.getOptimizeNoiseStdDev() is False
    ott.assert_almost_equal(algo.getNoiseVariances(), _hetero_variances(), 0, 0)
    with ott.assert_raises((TypeError, RuntimeError)):
        algo.setOptimizeNoiseStdDev(True)
    algo.setNoiseVariances(ot.Point())
    assert algo.getNoiseVariances().getSize() == 0


# The optimization must improve the heteroscedastic ELBO
def test_heteroscedastic_optimization_improves_elbo():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:4])
    algo.setNoiseVariances(_hetero_variances())
    algo.run()
    optimized_elbo = algo.getResult().getOptimalELBO()
    ott.assert_almost_equal(
        algo.getResult().getNoiseVariances(), _hetero_variances(), 0, 0
    )
    algo2 = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:4])
    algo2.setNoiseVariances(_hetero_variances())
    algo2.setOptimizeParameters(False)
    algo2.run()
    fixed_elbo = algo2.getResult().getOptimalELBO()
    assert optimized_elbo >= fixed_elbo, "optimization degrades the ELBO"


# Save / load must preserve the heteroscedastic result through a Study
def test_heteroscedastic_save_load():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:4])
    algo.setNoiseVariances(_hetero_variances())
    algo.run()
    result = algo.getResult()
    filename = "test_sparse_gp_hetero_result.xml"
    study = ot.Study(filename)
    study.add("result", result)
    study.save()
    study2 = ot.Study(filename)
    study2.load()
    result2 = SparseGaussianProcessFitterResult()
    study2.fillObject("result", result2)
    ott.assert_almost_equal(result.getOptimalELBO(), result2.getOptimalELBO(), 1e-10, 1e-10)
    ott.assert_almost_equal(
        result.getNoiseVariances(), result2.getNoiseVariances(), 1e-14, 1e-14
    )
    ott.assert_almost_equal(
        result.getMetaModel()(ot.Point([1.5])),
        result2.getMetaModel()(ot.Point([1.5])),
        1e-10,
        1e-10,
    )
    os.remove(filename)
    # the fitter itself must round-trip as well
    filename = "test_sparse_gp_hetero_fitter.xml"
    study = ot.Study(filename)
    study.add("algo", algo)
    study.save()
    study2 = ot.Study(filename)
    study2.load()
    algo2 = SparseGaussianProcessFitter()
    study2.fillObject("algo", algo2)
    ott.assert_almost_equal(
        algo.getResult().getOptimalELBO(),
        algo2.getResult().getOptimalELBO(),
        1e-10,
        1e-10,
    )
    ott.assert_almost_equal(
        algo.getResult().getNoiseVariances(),
        algo2.getResult().getNoiseVariances(),
        1e-14,
        1e-14,
    )
    assert algo2.getOptimizeNoiseStdDev() is False
    os.remove(filename)


def _log_variance_function(a=-3.0, b=0.2):
    base = ot.SymbolicFunction(["x", "a", "b"], ["a + b * x"])
    return ot.ParametricFunction(base, [1, 2], [a, b])


def _log_variances(X, a=-3.0, b=0.2):
    return [a + b * x[0] for x in X]


# A constant log-variance function must reproduce the homoscedastic fit
def test_variance_function_constant_matches_homoscedastic():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    Z = X[0:4]
    logVar = -2.0
    sigma = math.sqrt(math.exp(logVar))
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo.setNoiseStdDev(sigma)
    algo.setOptimizeNoiseStdDev(False)
    algo.run()
    reference = algo.getResult()
    g = ot.SymbolicFunction(["x"], [str(logVar)])
    algo2 = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo2.setVarianceFunction(g)
    assert algo2.getOptimizeNoiseStdDev() is False
    assert algo2.getOptimizeVarianceFunction() is True
    algo2.run()
    result = algo2.getResult()
    ott.assert_almost_equal(
        result.getOptimalELBO(), reference.getOptimalELBO(), 1e-8, 1e-9
    )
    ott.assert_almost_equal(
        np.array(result.getPosteriorMean()),
        np.array(reference.getPosteriorMean()),
        1e-9,
        1e-7,
    )
    x_test = ot.Point([1.5])
    ott.assert_almost_equal(
        result.getMetaModel()(x_test), reference.getMetaModel()(x_test), 1e-9, 1e-7
    )
    ott.assert_almost_equal(
        result.getVarianceFunction()(x_test), [logVar], 1e-14, 1e-14
    )


# The log-variance function ELBO with M=N and Z=X must equal the exact GP
# log marginal likelihood with diagonal noise exp(g(X))
def test_variance_function_matches_exact():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    N = X.getSize()
    Xn = np.array(X)
    Yn = np.array(Y).ravel()
    g = _log_variance_function()
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    algo.setVarianceFunction(g)
    algo.setOptimizeVarianceFunction(False)
    algo.run()
    elbo = algo.getResult().getOptimalELBO()
    D = np.exp(np.array(_log_variances(Xn)))
    Kff = np.array(
        [[covarianceModel.computeAsScalar(Xn[i], Xn[j]) for j in range(N)] for i in range(N)]
    )
    Knp = Kff + np.diag(D)
    sign, logdet = np.linalg.slogdet(Knp)
    assert sign > 0.0
    exact = -0.5 * (
        N * np.log(2.0 * np.pi)
        + logdet
        + Yn.dot(np.linalg.solve(Knp, Yn))
    )
    ott.assert_almost_equal(elbo, exact, 1e-6, 1e-9)


# The log-variance function ELBO for M<N must match the reference formula
def test_variance_function_matches_reference_sparse():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    Z = X[0:4]
    g = _log_variance_function()
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo.setVarianceFunction(g)
    algo.setOptimizeVarianceFunction(False)
    algo.run()
    elbo = algo.getResult().getOptimalELBO()
    D = np.exp(np.array(_log_variances(np.array(X))))
    reference = _numpy_elbo_hetero(
        np.array(X), np.array(Y).ravel(), np.array(Z), D, covarianceModel
    )
    ott.assert_almost_equal(elbo, reference, 1e-6, 1e-8)


# The analytic ELBO gradient with a log-variance function must match a
# centered finite difference, including the function parameter block
def test_variance_function_gradient():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setParameter([1.5, 2.0])
    Z = X[0:3]
    g = _log_variance_function(a=-2.5, b=0.1)
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo.setVarianceFunction(g)
    algo.setOptimizeInducingPoints(True)
    objective = algo.getObjectiveFunction()
    parameter = ot.Point([1.5, 2.0, -2.5, 0.1])
    for i in range(len(Z)):
        parameter.add(Z[i][0])
    assert len(parameter) == objective.getInputDimension()
    epsilon = 1e-5
    for i in range(len(parameter)):
        pointPlus = ot.Point(parameter)
        pointMinus = ot.Point(parameter)
        pointPlus[i] += epsilon
        pointMinus[i] -= epsilon
        fd = (objective(pointPlus)[0] - objective(pointMinus)[0])
        fd /= (2.0 * epsilon)
        analytic = objective.getGradient().gradient(parameter)[i, 0]
        ott.assert_almost_equal(analytic, fd, 1e-3, 1e-4)
    # same check when the number of inducing points equals the training size
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X)
    algo.setVarianceFunction(_log_variance_function(a=-2.5, b=0.1))
    algo.setOptimizeInducingPoints(True)
    objective = algo.getObjectiveFunction()
    parameter = ot.Point([1.5, 2.0, -2.5, 0.1])
    for i in range(len(X)):
        parameter.add(X[i][0])
    for i in range(len(parameter)):
        pointPlus = ot.Point(parameter)
        pointMinus = ot.Point(parameter)
        pointPlus[i] += epsilon
        pointMinus[i] -= epsilon
        fd = (objective(pointPlus)[0] - objective(pointMinus)[0])
        fd /= (2.0 * epsilon)
        analytic = objective.getGradient().gradient(parameter)[i, 0]
        ott.assert_almost_equal(analytic, fd, 1e-3, 1e-4)


# The joint optimization of the covariance and log-variance parameters
# must improve the ELBO
def test_variance_function_optimization_improves_elbo():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:4])
    algo.setVarianceFunction(_log_variance_function(a=-1.0, b=0.0))
    algo.run()
    optimized_elbo = algo.getResult().getOptimalELBO()
    optimized = list(algo.getResult().getVarianceFunction().getParameter())
    assert all(math.isfinite(v) for v in optimized)
    algo2 = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:4])
    algo2.setVarianceFunction(_log_variance_function(a=-1.0, b=0.0))
    algo2.setOptimizeParameters(False)
    algo2.setOptimizeVarianceFunction(False)
    algo2.run()
    fixed_elbo = algo2.getResult().getOptimalELBO()
    assert optimized_elbo >= fixed_elbo, "optimization degrades the ELBO"


# Disabling the variance function optimization must leave its parameters untouched
def test_variance_function_optimize_flag_off():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    algo.setVarianceFunction(_log_variance_function(a=-2.0, b=0.3))
    algo.setOptimizeVarianceFunction(False)
    assert algo.getOptimizeVarianceFunction() is False
    objective = algo.getObjectiveFunction()
    # no variance parameter in the optimization vector, hence empty here
    assert objective.getInputDimension() == 0
    algo.run()
    ott.assert_almost_equal(
        algo.getResult().getVarianceFunction().getParameter(), [-2.0, 0.3], 0, 0
    )


# setVarianceFunction must reject dimension mismatches, and the scalar noise
# optimization must stay disabled with a log-variance function
def test_variance_function_invalid():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    wrongInput = ot.SymbolicFunction(["x1", "x2"], ["x1 + x2"])
    with ott.assert_raises((TypeError, RuntimeError)):
        algo.setVarianceFunction(wrongInput)
    wrongOutput = ot.SymbolicFunction(["x"], ["x", "2 * x"])
    with ott.assert_raises((TypeError, RuntimeError)):
        algo.setVarianceFunction(wrongOutput)
    algo.setVarianceFunction(_log_variance_function())
    with ott.assert_raises((TypeError, RuntimeError)):
        algo.setOptimizeNoiseStdDev(True)


# The log-variance function and the fixed variances are mutually exclusive
def test_variance_function_mode_exclusion():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    # function then variances: fixed mode wins with identical ELBO
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    algo.setVarianceFunction(_log_variance_function())
    algo.setNoiseVariances(_hetero_variances())
    assert algo.getNoiseVariances().getSize() == X.getSize()
    algo.run()
    switched = algo.getResult().getOptimalELBO()
    algo2 = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    algo2.setNoiseVariances(_hetero_variances())
    algo2.run()
    ott.assert_almost_equal(
        switched, algo2.getResult().getOptimalELBO(), 1e-10, 1e-10
    )
    # variances then function: function mode wins, variances are cleared
    algo3 = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
    algo3.setNoiseVariances(_hetero_variances())
    algo3.setVarianceFunction(_log_variance_function())
    assert algo3.getNoiseVariances().getSize() == 0
    assert algo3.getOptimizeNoiseStdDev() is False
    # back to scalar: empty variances clear the function mode
    algo3.setNoiseVariances(ot.Point())
    algo3.setOptimizeNoiseStdDev(True)
    assert algo3.getOptimizeNoiseStdDev() is True
    algo3.setNoiseStdDev(0.1)
    algo3.setOptimizeNoiseStdDev(False)
    algo3.run()
    result3 = algo3.getResult()
    ott.assert_almost_equal(
        result3.getPredictiveVariance(ot.Point([1.5])),
        result3.getConditionalVariance(ot.Point([1.5])) + 0.01,
        1e-12,
        1e-12,
    )


# The predictive variance must combine the conditional variance with the
# noise variance in each likelihood mode
def test_predictive_variance():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    Z = X[0:4]
    x_test = ot.Point([1.5])
    sample_test = ot.Sample([[1.5], [4.0]])
    # homoscedastic: conditional variance plus scalar noise variance
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo.setNoiseStdDev(0.1)
    algo.setOptimizeNoiseStdDev(False)
    algo.run()
    result = algo.getResult()
    ott.assert_almost_equal(
        result.getPredictiveVariance(x_test),
        result.getConditionalVariance(x_test) + 0.01,
        1e-12,
        1e-12,
    )
    predictive = result.getPredictiveVariance(sample_test)
    assert len(predictive) == 2
    for i in range(2):
        ott.assert_almost_equal(
            predictive[i], result.getPredictiveVariance(sample_test[i]), 1e-14, 1e-14
        )
    # log-variance function: conditional variance plus exp(g(x))
    g = _log_variance_function()
    algo2 = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo2.setVarianceFunction(g)
    algo2.setOptimizeVarianceFunction(False)
    algo2.run()
    result2 = algo2.getResult()
    ott.assert_almost_equal(
        result2.getPredictiveVariance(x_test),
        result2.getConditionalVariance(x_test) + math.exp(g(x_test)[0]),
        1e-12,
        1e-12,
    )
    # fixed variances: predictive variance is undefined away from training data
    algo3 = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo3.setNoiseVariances(_hetero_variances())
    algo3.run()
    with ott.assert_raises((TypeError, RuntimeError)):
        algo3.getResult().getPredictiveVariance(x_test)


# Save / load must preserve the log-variance function fit through a Study
def test_variance_function_save_load():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:4])
    algo.setVarianceFunction(_log_variance_function())
    algo.setOptimizeVarianceFunction(False)
    algo.run()
    result = algo.getResult()
    filename = "test_sparse_gp_varfunc_result.xml"
    study = ot.Study(filename)
    study.add("result", result)
    study.save()
    study2 = ot.Study(filename)
    study2.load()
    result2 = SparseGaussianProcessFitterResult()
    study2.fillObject("result", result2)
    ott.assert_almost_equal(result.getOptimalELBO(), result2.getOptimalELBO(), 1e-10, 1e-10)
    ott.assert_almost_equal(
        result.getVarianceFunction()(ot.Point([1.5])),
        result2.getVarianceFunction()(ot.Point([1.5])),
        1e-14,
        1e-14,
    )
    ott.assert_almost_equal(
        result.getPredictiveVariance(ot.Point([1.5])),
        result2.getPredictiveVariance(ot.Point([1.5])),
        1e-10,
        1e-10,
    )
    os.remove(filename)
    filename = "test_sparse_gp_varfunc_fitter.xml"
    study = ot.Study(filename)
    study.add("algo", algo)
    study.save()
    study2 = ot.Study(filename)
    study2.load()
    algo2 = SparseGaussianProcessFitter()
    study2.fillObject("algo", algo2)
    ott.assert_almost_equal(
        algo.getResult().getOptimalELBO(),
        algo2.getResult().getOptimalELBO(),
        1e-10,
        1e-10,
    )
    ott.assert_almost_equal(
        algo.getResult().getVarianceFunction().getParameter(),
        algo2.getResult().getVarianceFunction().getParameter(),
        1e-14,
        1e-14,
    )
    os.remove(filename)


def test_resource_map_variance_bounds_inverted():
    X, Y = _sample()
    covarianceModel = ot.SquaredExponential([1.0])
    original_lo = ot.ResourceMap.GetAsScalar(
        "SparseGaussianProcessFitter-DefaultVarianceFunctionLowerBound"
    )
    original_hi = ot.ResourceMap.GetAsScalar(
        "SparseGaussianProcessFitter-DefaultVarianceFunctionUpperBound"
    )
    ot.ResourceMap.SetAsScalar(
        "SparseGaussianProcessFitter-DefaultVarianceFunctionLowerBound", 5.0
    )
    ot.ResourceMap.SetAsScalar(
        "SparseGaussianProcessFitter-DefaultVarianceFunctionUpperBound", -5.0
    )
    try:
        with ott.assert_raises((TypeError, RuntimeError)):
            algo = SparseGaussianProcessFitter(X, Y, covarianceModel, X[0:3])
            algo.setVarianceFunction(_log_variance_function())
    finally:
        ot.ResourceMap.SetAsScalar(
            "SparseGaussianProcessFitter-DefaultVarianceFunctionLowerBound", original_lo
        )
        ot.ResourceMap.SetAsScalar(
            "SparseGaussianProcessFitter-DefaultVarianceFunctionUpperBound", original_hi
        )


# Literature-style heteroscedastic validation with known log-linear noise
# (Goldberg / hetGP setup): the noise trend must be recovered, the
# heteroscedastic fit must beat its homoscedastic counterpart, and the
# predictive intervals must cover the true function. Fully deterministic.
def test_variance_function_literature_synthetic():
    N, M = 30, 6
    X = ot.Sample(N, 1)
    for i in range(N):
        X[i, 0] = 8.0 * i / (N - 1)
    aTrue, bTrue = -3.0, 0.3
    Y = ot.Sample(N, 1)
    for i in range(N):
        var = math.exp(aTrue + bTrue * X[i, 0])
        pattern = math.sin(6.0 * math.pi * i / N + 1.0)
        Y[i, 0] = X[i, 0] * math.sin(X[i, 0]) + math.sqrt(var) * pattern
    Z = ot.Sample([[8.0 * i / (N - 1)] for i in range(0, N, N // M)])
    assert Z.getSize() == M
    covarianceModel = ot.SquaredExponential([1.0])
    algo = SparseGaussianProcessFitter(X, Y, covarianceModel, Z)
    algo.setVarianceFunction(_log_variance_function(a=-1.0, b=0.0))
    algo.run()
    result = algo.getResult()
    aOpt, bOpt = result.getVarianceFunction().getParameter()
    # the increasing noise trend must be recovered
    assert math.isfinite(aOpt) and math.isfinite(bOpt)
    assert bOpt > 0.0
    hetero_elbo = result.getOptimalELBO()
    # the heteroscedastic fit must beat its homoscedastic counterpart
    covarianceHomo = ot.SquaredExponential([1.0])
    algoHomo = SparseGaussianProcessFitter(X, Y, covarianceHomo, Z)
    algoHomo.setNoiseStdDev(0.2)
    algoHomo.run()
    assert hetero_elbo > algoHomo.getResult().getOptimalELBO()
    # the 95% predictive intervals must cover the true function
    grid = 20
    confidence = 0
    for k in range(grid):
        x = 8.0 * k / (grid - 1)
        point = ot.Point([x])
        mean = result.getMetaModel()(point)[0]
        var = result.getPredictiveVariance(point)
        assert var > result.getConditionalVariance(point)
        if abs(mean - x * math.sin(x)) <= 1.96 * math.sqrt(var):
            confidence += 1
    assert confidence / grid >= 0.8


if __name__ == "__main__":
    test_elbo_matches_exact_log_likelihood()
    test_elbo_matches_reference_for_sparse()
    test_posterior_matches_reference()
    test_prediction_matches_posterior()
    test_elbo_monotonicity()
    test_optimization_improves_elbo()
    test_optimize_inducing_points()
    test_elbo_gradient()
    test_elbo_gradient_all_parameters()
    test_constructor_mismatched_samples()
    test_constructor_mismatched_samples_integer()
    test_constructor_too_many_inducing_points_integer()
    test_constructor_zero_inducing_points_integer()
    test_constructor_too_many_inducing_points_sample()
    test_constructor_inducing_points_wrong_dim()
    test_constructor_empty_inducing_points()
    test_constructor_empty_output_sample()
    test_set_noise_variance_zero()
    test_set_noise_variance_negative()
    test_set_inducing_points_wrong_dim()
    test_set_inducing_points_empty()
    test_set_inducing_points_too_many()
    test_objective_wrong_param_size()
    test_metamodel_wrong_input_dim()
    test_metamodel_gradient_wrong_input_dim()
    test_metamodel_hessian_wrong_input_dim()
    test_conditional_variance_wrong_input_dim()
    test_optimization_flag_accessors()
    test_method_accessor()
    test_optimization_algorithm_accessor()
    test_inducing_points_accessor()
    test_covariance_model_accessor()
    test_fitter_result_accessors()
    test_repr_str()
    test_result_repr_str()
    test_save_load()
    test_fitter_save_load()
    test_result_linear_algebra_method()
    test_set_method_invalid()
    test_direct_evaluation_gradient_hessian()
    test_resource_map_default_noise_variance_zero()
    test_resource_map_default_noise_variance_negative()
    test_resource_map_default_optimization_lower_bound_nonpositive()
    test_resource_map_default_optimization_upper_bound_nonpositive()
    test_resource_map_scale_factor_nonpositive()
    test_resource_map_noise_bounds_nonpositive()
    test_conditional_variance_sample()
    test_resource_map_default_optimization_algorithm_invalid()
    test_resource_map_optimization_normalization_disabled()
    test_resource_map_linear_algebra_hmat()
    test_heteroscedastic_constant_matches_homoscedastic()
    test_heteroscedastic_elbo_matches_exact()
    test_heteroscedastic_elbo_matches_reference_sparse()
    test_heteroscedastic_posterior_matches_reference()
    test_heteroscedastic_elbo_gradient()
    test_noise_variances_invalid()
    test_noise_variances_optimization_flag()
    test_heteroscedastic_optimization_improves_elbo()
    test_heteroscedastic_save_load()
    test_variance_function_constant_matches_homoscedastic()
    test_variance_function_matches_exact()
    test_variance_function_matches_reference_sparse()
    test_variance_function_gradient()
    test_variance_function_optimization_improves_elbo()
    test_variance_function_optimize_flag_off()
    test_variance_function_invalid()
    test_variance_function_mode_exclusion()
    test_predictive_variance()
    test_variance_function_save_load()
    test_resource_map_variance_bounds_inverted()
    test_variance_function_literature_synthetic()
