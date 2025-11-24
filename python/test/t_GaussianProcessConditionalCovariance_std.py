#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.ResourceMap.SetAsScalar("Cobyla-DefaultRhoBeg", 0.5)
ot.ResourceMap.SetAsScalar("OptimizationAlgorithm-DefaultMaximumAbsoluteError", 1e-8)


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
    gccc = ot.GaussianProcessConditionalCovariance(result)

    mean = gccc.getConditionalMean(X)
    ott.assert_almost_equal(mean, Y, 0.0, 1e-12)

    covariance = gccc.getConditionalCovariance(X)
    nullMatrix = ot.Matrix(sampleSize, sampleSize)
    ott.assert_almost_equal(covariance, nullMatrix, 0.0, 1e-12)

    # Covariance per marginal & extract variance component
    coll = gccc.getDiagonalCovarianceCollection(X)
    var = [mat[0, 0] for mat in coll]
    ott.assert_almost_equal(var, [0] * sampleSize, 1e-12, 1e-12)

    # Variance per marginal
    var = gccc.getConditionalMarginalVariance(X)
    ott.assert_almost_equal(var, ot.Sample(sampleSize, 1), 1e-12, 1e-12)

    # Kriging variance is non-null on validation points
    validCovariance = gccc.getConditionalCovariance(X2)
    values = ot.Matrix(
        [
            [0.81942182, -0.35599947, -0.17488593, 0.04622401, -0.03143555, 0.04054783],
            [-0.35599947, 0.20874735, 0.10943841, -0.03236419, 0.02397483, -0.03269184],
            [-0.17488593, 0.10943841, 0.05832917, -0.01779918, 0.01355719, -0.01891618],
            [0.04622401, -0.03236419, -0.01779918, 0.00578327, -0.00467674, 0.00688697],
            [-0.03143555, 0.02397483, 0.01355719, -0.00467674, 0.0040267, -0.00631173],
            [0.04054783, -0.03269184, -0.01891618, 0.00688697, -0.00631173, 0.01059488],
        ]
    )
    ott.assert_almost_equal(validCovariance - values, nullMatrix, 1.0e-5, 1e-6)


# Test 2
def test_two_inputs_one_output():

    inputDimension = 2
    # Learning data
    levels = [8, 5]
    box = ot.Box(levels)
    inputSample = box.generate()
    # Scale each direction
    inputSample *= 10.0

    model = ot.SymbolicFunction(["x", "y"], ["cos(0.5*x) + sin(y)"])
    outputSample = model(inputSample)

    # 2) Definition of exponential model
    # The parameters have been calibrated using TNC optimization
    # and AbsoluteExponential models
    scale = [5.33532, 2.61534]
    amplitude = [1.61536]
    covarianceModel = ot.SquaredExponential(scale, amplitude)

    # 3) Basis definition
    basis = ot.ConstantBasisFactory(inputDimension).build()

    # 4) GPF algorithm
    fit_algo = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
    # set sensible optimization bounds and estimate hyper parameters
    fit_algo.setOptimizationBounds(
        ot.Interval(inputSample.getMin(), inputSample.getMax())
    )
    fit_algo.run()

    # perform an evaluation
    fit_result = fit_algo.getResult()
    # Regression algorithm
    algo = ot.GaussianProcessRegression(fit_result)
    algo.run()
    result = algo.getResult()

    # 5) Kriging variance is 0 on learning points
    gccc = ot.GaussianProcessConditionalCovariance(result)
    covariance = gccc.getConditionalCovariance(inputSample)

    mean = gccc.getConditionalMean(inputSample)
    ott.assert_almost_equal(mean, outputSample, 1e-5, 1e-8)

    ott.assert_almost_equal(covariance, ot.SquareMatrix(len(inputSample)), 7e-7, 7e-7)

    # Covariance per marginal & extract variance component
    coll = gccc.getDiagonalCovarianceCollection(inputSample)
    var = [mat[0, 0] for mat in coll]
    ott.assert_almost_equal(var, [0] * len(var), 0.0, 1e-12)

    # Variance per marginal
    var = gccc.getConditionalMarginalVariance(inputSample)
    ott.assert_almost_equal(var, ot.Sample(inputSample.getSize(), 1), 0.0, 1e-12)


def test_two_outputs():
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
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    covarianceModel = ot.TensorizedCovarianceModel([covarianceModel] * 2)

    fit_algo = ot.GaussianProcessFitter(sampleX, sampleY, covarianceModel, basis)
    # set sensible optimization bounds and estimate hyper parameters
    fit_algo.run()

    # perform an evaluation
    fit_result = fit_algo.getResult()
    algo = ot.GaussianProcessRegression(fit_result)
    algo.run()
    result = algo.getResult()
    gccc = ot.GaussianProcessConditionalCovariance(result)

    mean = gccc.getConditionalMean(sampleX)
    ott.assert_almost_equal(mean, sampleY, 1e-5, 1e-8)

    # Check the conditional covariance
    reference_covariance = ot.Matrix(
        [[2.7237, 0, 3.40675, 0],
         [0, 7082.96, 0, 8859.22],
         [3.40675, 0, 4.54477, 0],
         [0, 8859.22, 0, 11818.623]])
    covariance = gccc([[9.5], [10.0]]).getCovariance()
    print(covariance)
    ott.assert_almost_equal(covariance, reference_covariance, 0.0, 2e-2)

    marginalVariance_0 = gccc.getConditionalMarginalVariance(sampleX, 0)
    ott.assert_almost_equal(marginalVariance_0, ot.Sample(len(sampleX), 1), 0.0, 1e-6)
    marginalVariance_1 = gccc.getConditionalMarginalVariance(sampleX, 1)
    ott.assert_almost_equal(marginalVariance_1, ot.Sample(len(sampleX), 1), 0.0, 1e-6)

    # Marginal variance on a specific point
    x = [1.1]
    covTest = gccc.getDiagonalCovariance(x)
    print(covTest)
    ref_cov_test = ot.Matrix(
        [
            [0.0022268, 0],
            [0, 5.79077],
        ]
    )

    margVarTest_0 = gccc.getConditionalMarginalVariance(x, 0)
    margVarTest_1 = gccc.getConditionalMarginalVariance(x, 1)
    ott.assert_almost_equal(covTest[0, 0], margVarTest_0, 0.0, 0)
    ott.assert_almost_equal(covTest[1, 1], margVarTest_1, 0.0, 0)
    ott.assert_almost_equal(covTest, ref_cov_test, 1.0e-6, 1e-6)


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

    gccc = ot.GaussianProcessConditionalCovariance(result)
    variance = gccc.getConditionalMarginalVariance(x)
    ott.assert_almost_equal(variance, ot.Sample(len(x), 1), 1e-12, 1e-12)


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

    # create covariance model
    covarianceModel = ot.SquaredExponential([1.6326932047296538], [4.895995962015954])
    trend_function = ot.SymbolicFunction("x", "1.49543")
    # GPR (comparable with test_one_input_one_output)
    algo = ot.GaussianProcessRegression(X, Y, covarianceModel, trend_function)
    algo.run()
    result = algo.getResult()
    gccc = ot.GaussianProcessConditionalCovariance(result)

    mean = gccc.getConditionalMean(X)
    ott.assert_almost_equal(mean, Y, 1e-5, 1e-8)

    covariance = gccc.getConditionalCovariance(X)
    nullMatrix = ot.Matrix(sampleSize, sampleSize)
    ott.assert_almost_equal(covariance, nullMatrix, 0.0, 1e-12)

    variance = gccc.getConditionalMarginalVariance(X)
    ott.assert_almost_equal(variance, ot.Sample(len(X), 1), 1e-12, 1e-12)

    # Kriging variance is non-null on validation points
    validCovariance = gccc.getConditionalCovariance(X2)

    values = ot.Matrix(
        [
            [
                0.786400318519185,
                -0.342314710430317,
                -0.167625132016427,
                0.0437937446519361,
                -0.0291542115306344,
                0.0362074153614559,
            ],
            [
                -0.342314710430317,
                0.20307609313608,
                0.106429376006901,
                -0.0313570361766278,
                0.0230293899173111,
                -0.0308930847149105,
            ],
            [
                -0.167625132016427,
                0.106429376006901,
                0.0567326538237296,
                -0.0172648099111221,
                0.0130555631357385,
                -0.0179618049872801,
            ],
            [
                0.0437937446519361,
                -0.0313570361766278,
                -0.0172648099111221,
                0.00560441404059731,
                -0.00450884121944028,
                0.00656752917461922,
            ],
            [
                -0.0291542115306344,
                0.0230293899173111,
                0.0130555631357385,
                -0.00450884121944028,
                0.00386908619998749,
                -0.00601186391616793,
            ],
            [
                0.0362074153614559,
                -0.0308930847149105,
                -0.0179618049872801,
                0.00656752917461922,
                -0.00601186391616793,
                0.0100243621895402,
            ],
        ]
    )
    ott.assert_almost_equal(validCovariance - values, nullMatrix, 1.0e-8, 1e-8)


if __name__ == "__main__":
    test_one_input_one_output()
    test_two_inputs_one_output()
    test_two_outputs()
    test_stationary_fun()
    test_gpr_no_opt()
