#! /usr/bin/env python

import math

import numpy as np

import openturns as ot
import openturns.testing as ott

from openturns.experimental import HeteroscedasticSparseGaussianProcessFitter
from openturns.experimental import HeteroscedasticSparseGaussianProcessFitterResult
from openturns.experimental import HeteroscedasticSparseGaussianProcessRegression
from openturns.experimental import SparseGaussianProcessFitter

ot.TESTPREAMBLE()


def _sample():
    X = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0]])
    f = ot.SymbolicFunction(["x"], ["x + x * sin(x)"])
    return X, f(X)


def _hetero_sample():
    # White heteroscedastic noise with known log-linear variance (Goldberg-style
    # setup) through the version-stable OpenTURNS random number generator
    ot.RandomGenerator.SetSeed(1)
    N, M, U = 60, 10, 6
    X = ot.Sample(N, 1)
    for i in range(N):
        X[i, 0] = 8.0 * i / (N - 1)
    Y = ot.Sample(N, 1)
    for i in range(N):
        var = math.exp(-3.0 + 0.3 * X[i, 0])
        Y[i, 0] = X[i, 0] * math.sin(X[i, 0]) + math.sqrt(var) * ot.DistFunc.rNormal()
    Zf = ot.Sample([[8.0 * i / (N - 1)] for i in range(0, N, N // M)])
    Zg = ot.Sample([[8.0 * i / (N - 1)] for i in range(0, N, N // U)])
    return X, Y, Zf, Zg


def _numpy_uncollapsed_elbo(X, Y, Zf, Zg, covF, covG, mu0, mf, Sf, mg, Sg):
    Xn, Yn = np.array(X).ravel(), np.array(Y).ravel()
    Zfn, Zgn = np.array(Zf).ravel(), np.array(Zg).ravel()
    N, M, U = len(Xn), len(Zfn), len(Zgn)
    KuuF = np.array(
        [[covF.computeAsScalar(Zf[i], Zf[j]) for j in range(M)] for i in range(M)]
    )
    KfuF = np.array(
        [[covF.computeAsScalar(X[i], Zf[j]) for j in range(M)] for i in range(N)]
    )
    LuuF = np.linalg.cholesky(KuuF)
    A = KfuF.dot(np.linalg.inv(LuuF).T)
    muf = A.dot(mf)
    qf = np.sum(A * A, axis=1)
    kf = np.array([covF.computeAsScalar(X[i], X[i]) for i in range(N)])
    vf = kf - qf + np.sum(A.dot(Sf) * A, axis=1)
    KuuG = np.array(
        [[covG.computeAsScalar(Zg[i], Zg[j]) for j in range(U)] for i in range(U)]
    )
    KfuG = np.array(
        [[covG.computeAsScalar(X[i], Zg[j]) for j in range(U)] for i in range(N)]
    )
    LuuG = np.linalg.cholesky(KuuG)
    B = KfuG.dot(np.linalg.inv(LuuG).T)
    mug = mu0 + B.dot(mg)
    qg = np.sum(B * B, axis=1)
    kg = np.array([covG.computeAsScalar(X[i], X[i]) for i in range(N)])
    vg = kg - qg + np.sum(B.dot(Sg) * B, axis=1)
    expo = np.exp(-mug + 0.5 * vg)
    data = -0.5 * np.sum(
        np.log(2.0 * np.pi) + mug + ((Yn - muf) ** 2 + vf) * expo
    )
    signF, logdetF = np.linalg.slogdet(Sf)
    assert signF > 0.0
    klF = 0.5 * (np.trace(Sf) + mf.dot(mf) - M - logdetF)
    signG, logdetG = np.linalg.slogdet(Sg)
    assert signG > 0.0
    # centered whitening: the g prior mean cancels in the KL divergence
    klG = 0.5 * (np.trace(Sg) + mg.dot(mg) - U - logdetG)
    return data - klF - klG


# The uncollapsed ELBO must match the independent NumPy reference
def test_elbo_matches_reference():
    X, Y = _sample()
    covarianceModelF = ot.SquaredExponential([1.0])
    covarianceModelF.setActiveParameter([])
    covarianceModelG = ot.SquaredExponential([2.0])
    covarianceModelG.setActiveParameter([])
    Zf, Zg = X[0:3], X[0:2]
    algo = HeteroscedasticSparseGaussianProcessFitter(
        X, Y, covarianceModelF, covarianceModelG, Zf, Zg
    )
    algo.setOptimizeParameters(False)
    algo.setOptimizeVariational(False)
    algo.run()
    elbo = algo.getResult().getOptimalELBO()
    M, U = 3, 2
    reference = _numpy_uncollapsed_elbo(
        np.array(X),
        np.array(Y).ravel(),
        np.array(Zf),
        np.array(Zg),
        covarianceModelF,
        covarianceModelG,
        algo.getMu0(),
        np.zeros(M),
        np.eye(M),
        np.zeros(U),
        np.eye(U),
    )
    ott.assert_almost_equal(elbo, reference, 1e-8, 1e-8)


# The analytic ELBO gradient must match a centered finite difference
def test_elbo_gradient():
    X, Y = _sample()
    covarianceModelF = ot.SquaredExponential([1.0])
    covarianceModelF.setParameter([1.5, 2.0])
    covarianceModelG = ot.SquaredExponential([2.0])
    covarianceModelG.setParameter([0.5, 1.5])
    M, U = 3, 2
    Zf, Zg = X[0:M], X[0:U]
    algo = HeteroscedasticSparseGaussianProcessFitter(
        X, Y, covarianceModelF, covarianceModelG, Zf, Zg
    )
    objective = algo.getObjectiveFunction()
    # layout: [covF(2), covG(2), mu0, mf(3), vechLf(6), mg(2), vechLg(3)],
    # with non-trivial variational parameters (generic point, no exact zeros)
    parameter = ot.Point(
        [1.5, 2.0, 0.5, 1.5, -2.0]
        + [0.1, -0.2, 0.05]
        + [0.1, 0.05, -0.1, 0.02, -0.03, 0.05]
        + [0.07, -0.04]
        + [0.05, -0.02, 0.08]
    )
    assert len(parameter) == objective.getInputDimension()
    epsilon = 1e-6
    for i in range(len(parameter)):
        pointPlus = ot.Point(parameter)
        pointMinus = ot.Point(parameter)
        pointPlus[i] += epsilon
        pointMinus[i] -= epsilon
        fd = (objective(pointPlus)[0] - objective(pointMinus)[0])
        fd /= (2.0 * epsilon)
        analytic = objective.getGradient().gradient(parameter)[i, 0]
        ott.assert_almost_equal(analytic, fd, 1e-3, 1e-4)


# The optimization must improve the ELBO
def test_optimization_improves_elbo():
    X, Y = _sample()
    covarianceModelF = ot.SquaredExponential([1.0])
    covarianceModelG = ot.SquaredExponential([2.0])
    Zf, Zg = X[0:3], X[0:2]
    algo = HeteroscedasticSparseGaussianProcessFitter(
        X, Y, covarianceModelF, covarianceModelG, Zf, Zg
    )
    algo.run()
    optimized_elbo = algo.getResult().getOptimalELBO()
    assert math.isfinite(optimized_elbo)
    algo2 = HeteroscedasticSparseGaussianProcessFitter(
        X, Y, covarianceModelF, covarianceModelG, Zf, Zg
    )
    algo2.setOptimizeParameters(False)
    algo2.setOptimizeVariational(False)
    algo2.run()
    assert optimized_elbo >= algo2.getResult().getOptimalELBO()


# With a concentrated log-variance process the fit must reproduce the
# homoscedastic sparse fit (homoscedastic limit)
def test_homoscedastic_limit():
    X, Y = _sample()
    sigma = 0.2
    covarianceModelF = ot.SquaredExponential([1.0])
    covarianceModelF.setActiveParameter([])
    Zf = X[0:4]
    # homoscedastic reference with the same inducing points
    algoHomo = SparseGaussianProcessFitter(X, Y, covarianceModelF, Zf)
    algoHomo.setNoiseStdDev(sigma)
    algoHomo.setOptimizeNoiseStdDev(False)
    algoHomo.run()
    reference = algoHomo.getResult()
    # heteroscedastic fit with a nearly constant log-variance process
    covarianceModelG = ot.SquaredExponential([10.0], [1e-6])
    covarianceModelG.setActiveParameter([])
    Zg = X[0:2]
    algo = HeteroscedasticSparseGaussianProcessFitter(
        X, Y, covarianceModelF, covarianceModelG, Zf, Zg
    )
    algo.setMu0(math.log(sigma * sigma))
    algo.setOptimizeParameters(False)
    algo.run()
    result = algo.getResult()
    x_test = ot.Point([1.5])
    ott.assert_almost_equal(
        result.getMetaModel()(x_test),
        reference.getMetaModel()(x_test),
        1e-3,
        1e-3,
    )
    ott.assert_almost_equal(
        result.getConditionalVariance(x_test),
        reference.getConditionalVariance(x_test),
        1e-3,
        1e-3,
    )
    ott.assert_almost_equal(
        result.getPredictiveVariance(x_test),
        reference.getConditionalVariance(x_test) + sigma * sigma,
        1e-2,
        1e-2,
    )


# The latent log-variance process must recover a known noise trend and
# provide calibrated predictive intervals
def test_synthetic_recovery():
    X, Y, Zf, Zg = _hetero_sample()
    covarianceModelF = ot.SquaredExponential([1.0])
    covarianceModelG = ot.SquaredExponential([2.0])
    algo = HeteroscedasticSparseGaussianProcessFitter(
        X, Y, covarianceModelF, covarianceModelG, Zf, Zg
    )
    algo.run()
    result = algo.getResult()
    hetero_elbo = result.getOptimalELBO()
    assert math.isfinite(hetero_elbo)
    # the heteroscedastic fit must beat its homoscedastic counterpart
    covarianceHomo = ot.SquaredExponential([1.0])
    algoHomo = SparseGaussianProcessFitter(X, Y, covarianceHomo, Zf)
    algoHomo.setNoiseStdDev(0.2)
    algoHomo.run()
    assert hetero_elbo > algoHomo.getResult().getOptimalELBO()
    # the fitted noise profile must increase with the input
    assert result.getPredictiveVariance(ot.Point([8.0])) > result.getPredictiveVariance(
        ot.Point([0.0])
    )
    # the 95% predictive intervals must cover the true function
    grid = 40
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


# Constructor and setter validation
def test_invalid_arguments():
    X, Y = _sample()
    covarianceModelF = ot.SquaredExponential([1.0])
    covarianceModelG = ot.SquaredExponential([1.0])
    # input/output size mismatch
    with ott.assert_raises((TypeError, RuntimeError)):
        HeteroscedasticSparseGaussianProcessFitter(
            X, Y[0:3], covarianceModelF, covarianceModelG, X[0:2], X[0:2]
        )
    # too many inducing points
    tooMany = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0], [9.0]])
    with ott.assert_raises((TypeError, RuntimeError)):
        HeteroscedasticSparseGaussianProcessFitter(
            X, Y, covarianceModelF, covarianceModelG, X[0:2], tooMany
        )
    # inducing dimension mismatch
    algo = HeteroscedasticSparseGaussianProcessFitter(
        X, Y, covarianceModelF, covarianceModelG, X[0:2], X[0:2]
    )
    with ott.assert_raises((TypeError, RuntimeError)):
        algo.setInducingPointsF(ot.Sample([[1.0, 2.0]]))
    with ott.assert_raises((TypeError, RuntimeError)):
        algo.setInducingPointsG(ot.Sample([[1.0, 2.0]]))
    # scalar outputs only
    with ott.assert_raises((TypeError, RuntimeError)):
        HeteroscedasticSparseGaussianProcessFitter(
            X,
            ot.Sample([[1.0, 2.0]] * 6),
            covarianceModelF,
            covarianceModelG,
            X[0:2],
            X[0:2],
        )


# Accessors, flags, representations and persistence
def test_accessors_and_persistence():
    X, Y = _sample()
    covarianceModelF = ot.SquaredExponential([1.0])
    covarianceModelG = ot.SquaredExponential([2.0])
    Zf, Zg = X[0:3], X[0:2]
    algo = HeteroscedasticSparseGaussianProcessFitter(
        X, Y, covarianceModelF, covarianceModelG, Zf, Zg
    )
    assert algo.getOptimizeParameters() is True
    assert algo.getOptimizeVariational() is True
    algo.setOptimizeParameters(False)
    assert algo.getOptimizeParameters() is False
    algo.setOptimizeVariational(False)
    assert algo.getOptimizeVariational() is False
    assert math.isfinite(algo.getMu0())
    algo.setMu0(-1.0)
    ott.assert_almost_equal(algo.getMu0(), -1.0, 0, 0)
    ott.assert_almost_equal(algo.getInducingPointsF(), Zf, 0, 0)
    ott.assert_almost_equal(algo.getInducingPointsG(), Zg, 0, 0)
    assert len(str(algo)) > 0
    assert len(repr(algo)) > 0
    algo.setOptimizeParameters(True)
    algo.setOptimizeVariational(True)
    algo.run()
    result = algo.getResult()
    assert result.getInducingPointsF().getSize() == 3
    assert result.getInducingPointsG().getSize() == 2
    assert result.getPosteriorMeanF().getSize() == 3
    assert result.getPosteriorMeanG().getSize() == 2
    assert math.isfinite(result.getOptimalELBO())
    assert len(str(result)) > 0
    assert len(repr(result)) > 0
    filename = "test_hetero_sparse_gp_result.xml"
    study = ot.Study(filename)
    study.add("result", result)
    study.save()
    study2 = ot.Study(filename)
    study2.load()
    result2 = HeteroscedasticSparseGaussianProcessFitterResult()
    study2.fillObject("result", result2)
    ott.assert_almost_equal(
        result.getOptimalELBO(), result2.getOptimalELBO(), 1e-10, 1e-10
    )
    ott.assert_almost_equal(
        result.getPredictiveVariance(ot.Point([1.5])),
        result2.getPredictiveVariance(ot.Point([1.5])),
        1e-10,
        1e-10,
    )
    import os

    os.remove(filename)


# The regression class must agree with the fitter
def test_regression():
    X, Y = _sample()
    covarianceModelF = ot.SquaredExponential([1.0])
    covarianceModelF.setActiveParameter([])
    covarianceModelG = ot.SquaredExponential([2.0])
    covarianceModelG.setActiveParameter([])
    Zf, Zg = X[0:3], X[0:2]
    algo = HeteroscedasticSparseGaussianProcessFitter(
        X, Y, covarianceModelF, covarianceModelG, Zf, Zg
    )
    algo.setOptimizeParameters(False)
    algo.setOptimizeVariational(False)
    algo.run()
    result = algo.getResult()
    regression = HeteroscedasticSparseGaussianProcessRegression(result)
    regression.run()
    x_test = ot.Point([1.5])
    ott.assert_almost_equal(
        regression.getResult().getMetaModel()(x_test),
        result.getMetaModel()(x_test),
        1e-12,
        1e-12,
    )
    regression2 = HeteroscedasticSparseGaussianProcessRegression(
        X, Y, covarianceModelF, covarianceModelG, Zf, Zg
    )
    with ott.assert_raises((TypeError, RuntimeError)):
        regression2.getResult()
    regression2.run()
    assert math.isfinite(regression2.getResult().getOptimalELBO())


# Extended accessors and fitter persistence for coverage
def test_extended_accessors_and_fitter_persistence():
    X, Y = _sample()
    covarianceModelF = ot.SquaredExponential([1.0])
    covarianceModelG = ot.SquaredExponential([2.0])
    Zf, Zg = X[0:3], X[0:2]
    algo = HeteroscedasticSparseGaussianProcessFitter(
        X, Y, covarianceModelF, covarianceModelG, Zf, Zg
    )
    # optimization algorithm accessor
    solver = algo.getOptimizationAlgorithm()
    assert solver.getImplementation().getClassName() == "TNC"
    cobyla = ot.Cobyla()
    algo.setOptimizationAlgorithm(cobyla)
    assert algo.getOptimizationAlgorithm().getImplementation().getClassName() == "Cobyla"
    # covariance and variational accessors before run
    assert algo.getCovarianceModelF().getInputDimension() == 1
    assert algo.getCovarianceModelG().getInputDimension() == 1
    assert algo.getReducedCovarianceModelF().getInputDimension() == 1
    assert algo.getReducedCovarianceModelG().getInputDimension() == 1
    assert algo.getVariationalMeanF().getSize() == 3
    assert algo.getVariationalMeanG().getSize() == 2
    assert algo.getVariationalCovarianceF().getNbRows() == 3
    assert algo.getVariationalCovarianceG().getNbRows() == 2
    algo.setOptimizeParameters(False)
    algo.setOptimizeVariational(False)
    algo.run()
    result = algo.getResult()
    # result covariance models, whitening factors and prior mean
    assert result.getCovarianceModelF().getInputDimension() == 1
    assert result.getCovarianceModelG().getInputDimension() == 1
    assert result.getWhiteningFactorF().getNbRows() == 3
    assert result.getWhiteningFactorG().getNbRows() == 2
    assert math.isfinite(result.getMu0())
    ott.assert_almost_equal(
        result.getPosteriorMeanF(), algo.getVariationalMeanF(), 1e-14, 1e-14
    )
    ott.assert_almost_equal(
        result.getPosteriorMeanG(), algo.getVariationalMeanG(), 1e-14, 1e-14
    )
    # fitter save / load round trip
    filename = "test_hetero_sparse_gp_fitter.xml"
    study = ot.Study(filename)
    study.add("algo", algo)
    study.save()
    study2 = ot.Study(filename)
    study2.load()
    algo2 = HeteroscedasticSparseGaussianProcessFitter()
    study2.fillObject("algo", algo2)
    ott.assert_almost_equal(
        algo.getResult().getOptimalELBO(),
        algo2.getResult().getOptimalELBO(),
        1e-10,
        1e-10,
    )
    ott.assert_almost_equal(algo2.getMu0(), algo.getMu0(), 1e-14, 1e-14)
    import os

    os.remove(filename)


# Invalid ResourceMap keys must be rejected at construction
def test_resource_map_invalid_keys():
    X, Y = _sample()
    covarianceModelF = ot.SquaredExponential([1.0])
    covarianceModelG = ot.SquaredExponential([1.0])
    keys = [
        "HeteroscedasticSparseGaussianProcessFitter-DefaultOptimizationLowerBound",
        "HeteroscedasticSparseGaussianProcessFitter-DefaultOptimizationUpperBound",
        "HeteroscedasticSparseGaussianProcessFitter-OptimizationLowerBoundScaleFactor",
        "HeteroscedasticSparseGaussianProcessFitter-OptimizationUpperBoundScaleFactor",
        "HeteroscedasticSparseGaussianProcessFitter-VariationalBoundFactor",
    ]
    originals = [ot.ResourceMap.GetAsScalar(key) for key in keys]
    for key, original in zip(keys, originals):
        ot.ResourceMap.SetAsScalar(key, -1.0)
        try:
            with ott.assert_raises((TypeError, RuntimeError)):
                HeteroscedasticSparseGaussianProcessFitter(
                    X, Y, covarianceModelF, covarianceModelG, X[0:2], X[0:2]
                )
        finally:
            ot.ResourceMap.SetAsScalar(key, original)
    originalAlgo = ot.ResourceMap.GetAsString(
        "HeteroscedasticSparseGaussianProcessFitter-DefaultOptimizationAlgorithm"
    )
    ot.ResourceMap.SetAsString(
        "HeteroscedasticSparseGaussianProcessFitter-DefaultOptimizationAlgorithm",
        "InvalidAlgo",
    )
    try:
        with ott.assert_raises((TypeError, RuntimeError)):
            HeteroscedasticSparseGaussianProcessFitter(
                X, Y, covarianceModelF, covarianceModelG, X[0:2], X[0:2]
            )
    finally:
        ot.ResourceMap.SetAsString(
            "HeteroscedasticSparseGaussianProcessFitter-DefaultOptimizationAlgorithm",
            originalAlgo,
        )


# Replicate detection: exact duplicates are merged with sufficient statistics
def test_replicate_detection():
    Xd = ot.Sample([[1.0], [3.0], [3.0], [5.0], [5.0], [5.0], [8.0]])
    Yd = ot.Sample([[1.0], [2.0], [4.0], [1.0], [1.0], [4.0], [3.0]])
    covarianceModelF = ot.SquaredExponential([1.0])
    covarianceModelG = ot.SquaredExponential([1.0])
    algo = HeteroscedasticSparseGaussianProcessFitter(
        Xd, Yd, covarianceModelF, covarianceModelG, Xd[0:2], Xd[0:2]
    )
    ott.assert_almost_equal(
        algo.getUniqueInputSample(), [[1.0], [3.0], [5.0], [8.0]], 0, 0
    )
    assert list(algo.getReplicateCounts()) == [1, 2, 3, 1]
    ott.assert_almost_equal(
        algo.getReplicateMeanOutput(), [[1.0], [3.0], [2.0], [3.0]], 1e-14, 1e-14
    )
    # without duplicates the structure is the identity
    X, Y = _sample()
    algo2 = HeteroscedasticSparseGaussianProcessFitter(
        X, Y, covarianceModelF, covarianceModelG, X[0:2], X[0:2]
    )
    ott.assert_almost_equal(algo2.getUniqueInputSample(), X, 0, 0)
    assert list(algo2.getReplicateCounts()) == [1] * X.getSize()
    # a single repeated site collapses to one site
    Xs = ot.Sample([[2.0]] * 5)
    Ys = ot.Sample([[1.0], [2.0], [3.0], [4.0], [5.0]])
    algo3 = HeteroscedasticSparseGaussianProcessFitter(
        Xs, Ys, covarianceModelF, covarianceModelG, Xs[0:2], Xs[0:2]
    )
    assert algo3.getUniqueInputSample().getSize() == 1
    assert list(algo3.getReplicateCounts()) == [5]
    ott.assert_almost_equal(algo3.getReplicateMeanOutput(), [[3.0]], 1e-14, 1e-14)


# The site-space likelihood with replicates must match the independent
# NumPy reference evaluated on the duplicated data
def test_replicate_exactness():
    Xd = ot.Sample(
        [[1.0], [3.0], [3.0], [5.0], [6.0], [6.0], [6.0], [8.0]]
    )
    f = ot.SymbolicFunction(["x"], ["x + x * sin(x)"])
    Yd = f(Xd)
    covarianceModelF = ot.SquaredExponential([1.0])
    covarianceModelF.setActiveParameter([])
    covarianceModelG = ot.SquaredExponential([2.0])
    covarianceModelG.setActiveParameter([])
    Zf = ot.Sample([[1.0], [5.0], [8.0]])
    Zg = ot.Sample([[1.0], [8.0]])
    algo = HeteroscedasticSparseGaussianProcessFitter(
        Xd, Yd, covarianceModelF, covarianceModelG, Zf, Zg
    )
    algo.setMu0(-2.0)
    algo.setOptimizeParameters(False)
    algo.setOptimizeVariational(False)
    algo.run()
    elbo = algo.getResult().getOptimalELBO()
    M, U = 3, 2
    reference = _numpy_uncollapsed_elbo(
        np.array(Xd),
        np.array(Yd).ravel(),
        np.array(Zf),
        np.array(Zg),
        covarianceModelF,
        covarianceModelG,
        algo.getMu0(),
        np.zeros(M),
        np.eye(M),
        np.zeros(U),
        np.eye(U),
    )
    ott.assert_almost_equal(elbo, reference, 1e-8, 1e-8)
    # the analytic gradient with replicates must match finite differences
    # (variational parameters only, covariance models fixed)
    algoFD = HeteroscedasticSparseGaussianProcessFitter(
        Xd, Yd, covarianceModelF, covarianceModelG, Zf, Zg
    )
    algoFD.setMu0(-2.0)
    algoFD.setOptimizeParameters(False)
    objective = algoFD.getObjectiveFunction()
    parameter = ot.Point([0.0] * objective.getInputDimension())
    assert len(parameter) == M + M * (M + 1) // 2 + U + U * (U + 1) // 2
    epsilon = 1e-6
    for i in range(len(parameter)):
        pointPlus = ot.Point(parameter)
        pointMinus = ot.Point(parameter)
        pointPlus[i] += epsilon
        pointMinus[i] -= epsilon
        fd = (objective(pointPlus)[0] - objective(pointMinus)[0])
        fd /= (2.0 * epsilon)
        analytic = objective.getGradient().gradient(parameter)[i, 0]
        ott.assert_almost_equal(analytic, fd, 1e-3, 1e-4)
    # predictions at a new point must be finite with positive variances
    x_test = ot.Point([4.0])
    assert math.isfinite(algo.getResult().getMetaModel()(x_test)[0])
    assert algo.getResult().getConditionalVariance(x_test) > 0.0
    assert algo.getResult().getPredictiveVariance(x_test) > 0.0


# ALM/IMSPE criteria must match brute force and reject invalid inputs
def test_alm_imspe():
    X, Y = _sample()
    covarianceModelF = ot.SquaredExponential([1.0])
    covarianceModelF.setActiveParameter([])
    covarianceModelG = ot.SquaredExponential([2.0])
    covarianceModelG.setActiveParameter([])
    Zf, Zg = X[0:3], X[0:2]
    algo = HeteroscedasticSparseGaussianProcessFitter(
        X, Y, covarianceModelF, covarianceModelG, Zf, Zg
    )
    algo.setMu0(-2.0)
    algo.setOptimizeParameters(False)
    algo.setOptimizeVariational(False)
    algo.run()
    result = algo.getResult()
    candidates = ot.Sample([[0.5], [2.0], [4.5], [7.0], [9.0]])
    manual = [result.getPredictiveVariance(candidates[i]) for i in range(5)]
    assert result.computeALM(candidates) == int(np.argmax(manual))
    ott.assert_almost_equal(
        result.computeIMSPE(candidates), float(np.mean(manual)), 1e-14, 1e-14
    )
    with ott.assert_raises((TypeError, RuntimeError)):
        result.computeALM(ot.Sample(0, 1))
    with ott.assert_raises((TypeError, RuntimeError)):
        result.computeIMSPE(ot.Sample(0, 1))
    with ott.assert_raises((TypeError, RuntimeError)):
        result.computeALM(ot.Sample([[1.0, 2.0]]))


# Save / load must preserve a fit on replicated data
def test_replicate_save_load():
    Xd = ot.Sample([[1.0], [3.0], [3.0], [5.0], [8.0], [8.0]])
    f = ot.SymbolicFunction(["x"], ["x + x * sin(x)"])
    Yd = f(Xd)
    covarianceModelF = ot.SquaredExponential([1.0])
    covarianceModelF.setActiveParameter([])
    covarianceModelG = ot.SquaredExponential([2.0])
    covarianceModelG.setActiveParameter([])
    algo = HeteroscedasticSparseGaussianProcessFitter(
        Xd, Yd, covarianceModelF, covarianceModelG, Xd[0:2], Xd[0:2]
    )
    algo.setMu0(-2.0)
    algo.setOptimizeParameters(False)
    algo.setOptimizeVariational(False)
    algo.run()
    filename = "test_hetero_sparse_gp_replicates.xml"
    study = ot.Study(filename)
    study.add("algo", algo)
    study.save()
    study2 = ot.Study(filename)
    study2.load()
    algo2 = HeteroscedasticSparseGaussianProcessFitter()
    study2.fillObject("algo", algo2)
    assert list(algo2.getReplicateCounts()) == list(algo.getReplicateCounts())
    ott.assert_almost_equal(
        algo.getResult().getOptimalELBO(),
        algo2.getResult().getOptimalELBO(),
        1e-10,
        1e-10,
    )
    import os

    os.remove(filename)


if __name__ == "__main__":
    test_elbo_matches_reference()
    test_elbo_gradient()
    test_optimization_improves_elbo()
    test_homoscedastic_limit()
    test_synthetic_recovery()
    test_invalid_arguments()
    test_accessors_and_persistence()
    test_extended_accessors_and_fitter_persistence()
    test_regression()
    test_resource_map_invalid_keys()
    test_replicate_detection()
    test_replicate_exactness()
    test_alm_imspe()
    test_replicate_save_load()
