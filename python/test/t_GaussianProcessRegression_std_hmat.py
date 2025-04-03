#! /usr/bin/env python

import openturns as ot
from openturns.experimental import GaussianProcessRegression, GaussianProcessFitter
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.ResourceMap.SetAsUnsignedInteger(
    "OptimizationAlgorithm-DefaultMaximumCallsNumber", 20000
)
ot.ResourceMap.SetAsScalar("Cobyla-DefaultRhoBeg", 0.5)
ot.ResourceMap.SetAsScalar("OptimizationAlgorithm-DefaultMaximumAbsoluteError", 1e-8)

ot.PlatformInfo.SetNumericalPrecision(3)
ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "HMAT")
ot.ResourceMap.SetAsScalar("HMatrix-RegularizationEpsilon", 1e-7)


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
    fit_algo = GaussianProcessFitter(X, Y, covarianceModel, basis)

    # set sensible optimization bounds and estimate hyper parameters
    fit_algo.setOptimizationBounds(ot.Interval(X.getMin(), X.getMax()))
    fit_algo.run()

    # perform an evaluation
    fit_result = fit_algo.getResult()

    algo = GaussianProcessRegression(fit_result)
    algo.run()
    result = algo.getResult()
    ott.assert_almost_equal(result.getMetaModel()(X), Y, 1e-2)

    # Prediction accuracy
    ott.assert_almost_equal(Y2, result.getMetaModel()(X2), 0.3, 0.0)


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

    # Validation
    sampleSize = 10
    inputValidSample = ot.JointDistribution(2 * [ot.Uniform(0, 10.0)]).getSample(
        sampleSize
    )
    outputValidSample = model(inputValidSample)

    # 2) Definition of exponential model
    # The parameters have been calibrated using TNC optimization
    # and AbsoluteExponential models
    scale = [5.33532, 2.61534]
    amplitude = [1.61536]
    covarianceModel = ot.SquaredExponential(scale, amplitude)

    # 3) Basis definition
    basis = ot.ConstantBasisFactory(inputDimension).build()

    # 4) GPF algorithm
    fit_algo = GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
    # set sensible optimization bounds and estimate hyper parameters
    fit_algo.setOptimizationBounds(
        ot.Interval(inputSample.getMin(), inputSample.getMax())
    )
    fit_algo.run()

    # perform an evaluation
    fit_result = fit_algo.getResult()
    # Regression algorithm
    algo = GaussianProcessRegression(fit_result)
    algo.run()

    result = algo.getResult()
    # Get meta model
    metaModel = result.getMetaModel()
    outData = metaModel(inputValidSample)

    # 5) Errors
    # Interpolation
    Yhat = metaModel(inputSample)
    ott.assert_almost_equal(outputSample, Yhat, 3.0e-2, 3.0e-2)

    # Prediction
    ott.assert_almost_equal(outputValidSample, outData, 1.0e-1, 1e-1)


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

    fit_algo = GaussianProcessFitter(sampleX, sampleY, covarianceModel, basis)
    # set sensible optimization bounds and estimate hyper parameters
    fit_algo.run()

    # perform an evaluation
    fit_result = fit_algo.getResult()
    algo = GaussianProcessRegression(fit_result)
    algo.run()
    result = algo.getResult()
    mm = result.getMetaModel()
    assert mm.getOutputDimension() == 2, "wrong output dim"
    ott.assert_almost_equal(mm([5.5]), [-3.88, 3.90], 1e-2, 1e-3)


def test_stationary_fun():
    # fix https://github.com/openturns/openturns/issues/1861
    ot.RandomGenerator.SetSeed(0)
    rho = ot.SymbolicFunction("tau", "exp(-abs(tau))*cos(2*pi_*abs(tau))")
    model = ot.StationaryFunctionalCovarianceModel([1], [1], rho)
    x = ot.Normal().getSample(20)
    x.setDescription(["J0"])
    y = x + ot.Normal(0, 0.1).getSample(20)
    y.setDescription(["G0"])

    fit_algo = GaussianProcessFitter(x, y, model, ot.LinearBasisFactory().build())
    # set sensible optimization bounds and estimate hyper parameters
    fit_algo.run()

    # perform an evaluation
    fit_result = fit_algo.getResult()
    algo = GaussianProcessRegression(fit_result)

    algo.run()
    result = algo.getResult()
    mm = result.getMetaModel()
    ott.assert_almost_equal(mm([5.5]), [5.58283])


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
    # GPR (comparable with test_one_input_one_output)
    algo = GaussianProcessRegression(X, Y, covarianceModel, trend_function)
    algo.run()
    result = algo.getResult()
    Yhat = result.getMetaModel()(X)
    ott.assert_almost_equal(Yhat, Y)
    # Prediction accuracy
    ott.assert_almost_equal(Y2, result.getMetaModel()(X2), 0.3, 0.0)


if __name__ == "__main__":
    test_one_input_one_output()
    test_two_inputs_one_output()
    test_two_outputs()
    test_stationary_fun()
    test_gpr_no_opt()
