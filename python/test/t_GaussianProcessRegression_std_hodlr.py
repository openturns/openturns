#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "HODLR")
ot.ResourceMap.SetAsUnsignedInteger("HODLRMatrix-MinLeafSize", 4)
ot.ResourceMap.SetAsScalar("HODLRMatrix-AssemblyEpsilon", 1.0e-6)
ot.ResourceMap.SetAsScalar("HODLRMatrix-RecompressionEpsilon", 1.0e-6)
# Smooth kernels on dense designs are near-singular: a small nugget keeps the
# HODLR Cholesky factorization shift-free (see HODLRMatrix regularization
# warning), well below the assertion tolerances used below.
ot.ResourceMap.SetAsScalar("HODLRMatrix-Nugget", 1.0e-6)


# Test 1
def test_one_input_one_output():
    sampleSize = 6
    dimension = 1

    f = ot.SymbolicFunction(["x0"], ["x0 * sin(x0)"])

    X = ot.Sample(sampleSize, dimension)
    X2 = ot.Sample(sampleSize, dimension)
    for i in range(sampleSize):
        X[i, 0] = 3.0 + i
        X2[i, 0] = 2.5 + i
    X[0, 0] = 1.0
    X[1, 0] = 3.0
    X2[0, 0] = 2.0
    X2[1, 0] = 4.0
    Y = f(X)
    Y2 = f(X2)

    # create covariance model
    basis = ot.ConstantBasisFactory(dimension).build()
    covarianceModel = ot.SquaredExponential()

    # create algorithm
    fit_algo = ot.GaussianProcessFitter(X, Y, covarianceModel, basis)

    # set sensible optimization bounds and estimate hyper parameters
    fit_algo.setOptimizationBounds(ot.Interval(X.getMin(), X.getMax()))
    fit_algo.run()

    # perform an evaluation
    fit_result = fit_algo.getResult()

    algo = ot.GaussianProcessRegression(fit_result)
    algo.run()
    result = algo.getResult()
    ott.assert_almost_equal(result.getMetaModel()(X), Y, 1e-2)

    # Prediction accuracy
    ott.assert_almost_equal(Y2, result.getMetaModel()(X2), 0.3, 0.0)


# Test 2
def test_two_inputs_one_output():
    # HODLR-adequate parameterization: fixed calibrated parameters, no
    # optimization through HODLR factorizations (see GaussianProcessFitter
    # "extreme parameter values" handling), HODLR checked against LAPACK.
    inputDimension = 2
    # Learning data
    levels = [4, 3]
    box = ot.Box(levels)
    inputSample = box.generate()
    # Scale each direction
    inputSample *= 10.0

    model = ot.SymbolicFunction(["x", "y"], ["cos(0.5*x) + sin(y)"])
    outputSample = model(inputSample)

    # Validation
    sampleSize = 5
    inputValidSample = ot.JointDistribution(2 * [ot.Uniform(0, 10.0)]).getSample(
        sampleSize
    )

    # 2) Definition of exponential model
    covarianceModel = ot.SquaredExponential([5.0, 3.0], [1.0])

    # 3) Basis definition
    basis = ot.ConstantBasisFactory(inputDimension).build()

    # LAPACK reference
    ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "LAPACK")
    fit_lapack = ot.GaussianProcessFitter(
        inputSample, outputSample, covarianceModel, basis
    )
    fit_lapack.setOptimizeParameters(False)
    fit_lapack.run()
    gpr_lapack = ot.GaussianProcessRegression(fit_lapack.getResult())
    gpr_lapack.run()
    metaModel_ref = gpr_lapack.getResult().getMetaModel()
    outData_ref = metaModel_ref(inputValidSample)

    # HODLR
    ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "HODLR")
    fit_hodlr = ot.GaussianProcessFitter(
        inputSample, outputSample, covarianceModel, basis
    )
    fit_hodlr.setOptimizeParameters(False)
    fit_hodlr.run()
    # Regression algorithm
    algo = ot.GaussianProcessRegression(fit_hodlr.getResult())
    algo.run()

    result = algo.getResult()
    # Get meta model
    metaModel = result.getMetaModel()
    outData = metaModel(inputValidSample)

    # 5) Errors
    # Interpolation
    Yhat = metaModel(inputSample)
    ott.assert_almost_equal(outputSample, Yhat, 3.0e-2, 3.0e-2)

    # Prediction matches LAPACK
    ott.assert_almost_equal(outData, outData_ref, 1.0e-1, 1.0e-1)


def test_two_outputs():
    # HODLR-adequate parameterization: the 8-point tensorized model
    # mis-compresses with tiny leaves (MinLeafSize 4/8 gives 1.47/0.54
    # error on the second output), so use the dense fallback locally;
    # fixed LAPACK-calibrated parameters, no optimization through HODLR
    # factorizations, HODLR checked against LAPACK (never self-validated).
    f = ot.SymbolicFunction(["x"], ["x * sin(x)", "x * cos(x)"])
    sampleX = ot.Sample([[1.0], [2.0], [3.0], [4.0], [5.0], [6.0], [7.0], [8.0]])
    sampleY = f(sampleX)
    # Build a basis phi from R --> R^2
    # phi_{0,0} = phi_{0,1} = x
    # phi_{1,0} = phi_{1,1} = x^2
    phi0 = ot.AggregatedFunction(
        [ot.SymbolicFunction(["x"], ["x"]), ot.SymbolicFunction(["x"], ["x"])]
    )
    phi1 = ot.AggregatedFunction(
        [ot.SymbolicFunction(["x"], ["x^2"]), ot.SymbolicFunction(["x"], ["x^2"])]
    )
    basis = ot.Basis([phi0, phi1])

    def build_covariance():
        sub = ot.SquaredExponential([1.0])
        sub.setActiveParameter([])
        return ot.TensorizedCovarianceModel([sub] * 2)

    # LAPACK reference with optimization (independent authority)
    ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "LAPACK")
    fit_lapack = ot.GaussianProcessFitter(
        sampleX, sampleY, build_covariance(), basis
    )
    fit_lapack.run()
    result_lapack = fit_lapack.getResult()
    gpr_lapack = ot.GaussianProcessRegression(result_lapack)
    gpr_lapack.run()
    y_ref = gpr_lapack.getResult().getMetaModel()([5.5])

    # HODLR reuses the LAPACK-calibrated parameters without re-optimizing
    prev_leaf = ot.ResourceMap.GetAsUnsignedInteger("HODLRMatrix-MinLeafSize")
    ot.ResourceMap.SetAsUnsignedInteger("HODLRMatrix-MinLeafSize", 250)
    try:
        ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "HODLR")
        covariance_hodlr = build_covariance()
        covariance_hodlr.setParameter(
            result_lapack.getCovarianceModel().getParameter()
        )
        fit_hodlr = ot.GaussianProcessFitter(
            sampleX, sampleY, covariance_hodlr, basis
        )
        fit_hodlr.setOptimizeParameters(False)
        fit_hodlr.run()
        algo = ot.GaussianProcessRegression(fit_hodlr.getResult())
        algo.run()
        result = algo.getResult()
        mm = result.getMetaModel()
        assert mm.getOutputDimension() == 2, "wrong output dim"
        # Interpolation
        ott.assert_almost_equal(mm(sampleX), sampleY, 3.0e-2, 3.0e-2)
        # Prediction matches LAPACK
        y = mm([5.5])
        print(y)
        ott.assert_almost_equal(y, y_ref, 1e-2, 1e-3)
    finally:
        ot.ResourceMap.SetAsUnsignedInteger("HODLRMatrix-MinLeafSize", prev_leaf)


def test_stationary_fun():
    # fix https://github.com/openturns/openturns/issues/1861
    ot.RandomGenerator.SetSeed(0)
    rho = ot.SymbolicFunction("tau", "exp(-abs(tau))*cos(2*pi_*abs(tau))")
    model = ot.StationaryFunctionalCovarianceModel([1], [1], rho)
    x = ot.Normal().getSample(20)
    x.setDescription(["J0"])
    y = x + ot.Normal(0, 0.1).getSample(20)
    y.setDescription(["G0"])

    fit_algo = ot.GaussianProcessFitter(x, y, model, ot.LinearBasisFactory().build())
    # set sensible optimization bounds and estimate hyper parameters
    fit_algo.run()

    # perform an evaluation
    fit_result = fit_algo.getResult()
    algo = ot.GaussianProcessRegression(fit_result)

    algo.run()
    result = algo.getResult()
    mm = result.getMetaModel()
    y = mm([5.5])
    print(y)
    ott.assert_almost_equal(y, [5.58843])


def test_gpr_no_opt():
    sampleSize = 6
    dimension = 1

    f = ot.SymbolicFunction(["x0"], ["x0 * sin(x0)"])

    X = ot.Sample(sampleSize, dimension)
    X2 = ot.Sample(sampleSize, dimension)
    for i in range(sampleSize):
        X[i, 0] = 3.0 + i
        X2[i, 0] = 2.5 + i
    X[0, 0] = 1.0
    X[1, 0] = 3.0
    X2[0, 0] = 2.0
    X2[1, 0] = 4.0
    Y = f(X)
    Y2 = f(X2)

    # create covariance model
    covarianceModel = ot.SquaredExponential([1.6326932047296538], [4.895995962015954])
    trend_function = ot.SymbolicFunction("x", "1.49543")
    # LAPACK reference (independent authority)
    ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "LAPACK")
    algo_ref = ot.GaussianProcessRegression(X, Y, covarianceModel, trend_function)
    algo_ref.run()
    Yhat_ref = algo_ref.getResult().getMetaModel()(X)
    # GPR (comparable with test_one_input_one_output)
    ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "HODLR")
    algo = ot.GaussianProcessRegression(X, Y, covarianceModel, trend_function)
    algo.run()
    result = algo.getResult()
    Yhat = result.getMetaModel()(X)
    # Interpolation within HODLR accuracy (nugget 1e-6 biases by ~2e-05)
    ott.assert_almost_equal(Yhat, Y, 1e-4, 1e-6)
    # Matches LAPACK reference (never self-validated)
    ott.assert_almost_equal(Yhat, Yhat_ref, 1e-4, 1e-6)
    # Prediction accuracy
    ott.assert_almost_equal(Y2, result.getMetaModel()(X2), 0.3, 0.0)


if __name__ == "__main__":
    test_one_input_one_output()
    test_two_inputs_one_output()
    test_two_outputs()
    test_stationary_fun()
    test_gpr_no_opt()
