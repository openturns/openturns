#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Test 1


def test_one_input_one_output():
    sampleSize = 6
    dimension = 1

    f = ot.SymbolicFunction(['x0'], ['x0 * sin(x0)'])

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
    algo = ot.KrigingAlgorithm(X, Y, covarianceModel, basis)

    # set sensible optimization bounds and estimate hyperparameters
    algo.setOptimizationBounds(ot.Interval(X.getMin(), X.getMax()))
    algo.run()

    # perform an evaluation
    result = algo.getResult()

    ott.assert_almost_equal(result.getMetaModel()(X), Y)
    ott.assert_almost_equal(result.getResiduals(), [1.32804e-07], 1e-3, 1e-3)
    ott.assert_almost_equal(result.getRelativeErrors(), [5.20873e-21])

    # Kriging variance is 0 on learning points
    covariance = result.getConditionalCovariance(X)
    nullMatrix = ot.Matrix(sampleSize, sampleSize)
    ott.assert_almost_equal(covariance, nullMatrix, 0.0, 1e-13)

    # Kriging variance is non-null on validation points
    validCovariance = result.getConditionalCovariance(X2)
    values = ot.Matrix([[0.81942182, -0.35599947, -0.17488593, 0.04622401, -0.03143555, 0.04054783],
                        [-0.35599947, 0.20874735, 0.10943841, -
                            0.03236419, 0.02397483, -0.03269184],
                        [-0.17488593, 0.10943841, 0.05832917, -
                            0.01779918, 0.01355719, -0.01891618],
                        [0.04622401, -0.03236419, -0.01779918,
                            0.00578327, -0.00467674, 0.00688697],
                        [-0.03143555, 0.02397483, 0.01355719, -
                            0.00467674, 0.0040267, -0.00631173],
                        [0.04054783, -0.03269184, -0.01891618, 0.00688697, -0.00631173, 0.01059488]])
    ott.assert_almost_equal(validCovariance - values, nullMatrix, 0.0, 1e-7)

    # Covariance per marginal & extract variance component
    coll = result.getConditionalMarginalCovariance(X)
    var = [mat[0, 0] for mat in coll]
    ott.assert_almost_equal(var, [0]*sampleSize, 1e-14, 1e-13)

    # Variance per marginal
    var = result.getConditionalMarginalVariance(X)
    ott.assert_almost_equal(var, ot.Sample(sampleSize, 1), 1e-14, 1e-13)

    # Prediction accuracy
    ott.assert_almost_equal(Y2, result.getMetaModel()(X2), 0.3, 0.0)

# Test 2


def test_two_inputs_one_output():
    # Kriging use case
    inputDimension = 2

    # Learning data
    levels = [8, 5]
    box = ot.Box(levels)
    inputSample = box.generate()
    # Scale each direction
    inputSample *= 10.0

    model = ot.SymbolicFunction(['x', 'y'], ['cos(0.5*x) + sin(y)'])
    outputSample = model(inputSample)

    # Validation
    sampleSize = 10
    inputValidSample = ot.ComposedDistribution(
        2 * [ot.Uniform(0, 10.0)]).getSample(sampleSize)
    outputValidSample = model(inputValidSample)

    # 2) Definition of exponential model
    # The parameters have been calibrated using TNC optimization
    # and AbsoluteExponential models
    scales = [5.33532, 2.61534]
    amplitude = [1.61536]
    covarianceModel = ot.SquaredExponential(scales, amplitude)

    # 3) Basis definition
    basis = ot.ConstantBasisFactory(inputDimension).build()

    # 4) Kriging algorithm
    algo = ot.KrigingAlgorithm(inputSample, outputSample,
                               covarianceModel, basis)
    algo.run()

    result = algo.getResult()
    # Get meta model
    metaModel = result.getMetaModel()
    outData = metaModel(inputValidSample)

    # 5) Errors
    # Interpolation
    ott.assert_almost_equal(
        outputSample,  metaModel(inputSample), 3.0e-5, 3.0e-5)

    # 6) Kriging variance is 0 on learning points
    covariance = result.getConditionalCovariance(inputSample)
    ott.assert_almost_equal(
        covariance, ot.SquareMatrix(len(inputSample)), 7e-7, 7e-7)

    # Covariance per marginal & extract variance component
    coll = result.getConditionalMarginalCovariance(inputSample)
    var = [mat[0, 0] for mat in coll]
    ott.assert_almost_equal(var, [0]*len(var), 0.0, 1e-13)

    # Variance per marginal
    var = result.getConditionalMarginalVariance(inputSample)
    ott.assert_almost_equal(var, ot.Sample(
        inputSample.getSize(), 1), 0.0, 1e-13)
    # Estimation
    ott.assert_almost_equal(outputValidSample, outData, 1.e-1, 1e-1)


def test_two_outputs():
    f = ot.SymbolicFunction(['x'], ['x * sin(x)', 'x * cos(x)'])
    sampleX = [[1.0], [2.0], [3.0], [4.0], [5.0], [6.0], [7.0], [8.0]]
    sampleY = f(sampleX)
    basis = ot.Basis([ot.SymbolicFunction(['x'], ['x']),
                      ot.SymbolicFunction(['x'], ['x^2'])])
    covarianceModel = ot.SquaredExponential([1.0])
    covarianceModel.setActiveParameter([])
    algo = ot.KrigingAlgorithm(sampleX, sampleY, covarianceModel, basis)
    algo.run()
    result = algo.getResult()
    mm = result.getMetaModel()
    assert mm.getOutputDimension() == 2, "wrong output dim"
    ott.assert_almost_equal(mm(sampleX), sampleY)
    # Check the conditional covariance
    reference_covariance = ot.Matrix([[4.4527, 0.0, 8.34404, 0.0],
                                      [0.0, 2.8883, 0.0, 5.41246],
                                      [8.34404, 0.0, 15.7824, 0.0],
                                      [0.0, 5.41246, 0.0, 10.2375]])
    ott.assert_almost_equal(result([[9.5], [10.0]]).getCovariance(
    ) - reference_covariance, ot.Matrix(4, 4), 0.0, 1e-2)


def test_stationary_fun():
    # fix https://github.com/openturns/openturns/issues/1861
    ot.RandomGenerator.SetSeed(0)
    rho = ot.SymbolicFunction("tau", "exp(-abs(tau))*cos(2*pi_*abs(tau))")
    model = ot.StationaryFunctionalCovarianceModel([1], [1], rho)
    x = ot.Normal().getSample(20)
    y = x + ot.Normal(0, 0.1).getSample(20)

    algo = ot.KrigingAlgorithm(x, y, model, ot.LinearBasisFactory().build())
    algo.run()
    result = algo.getResult()
    variance = result.getConditionalMarginalVariance(x)
    ott.assert_almost_equal(variance, ot.Sample(len(x), 1), 1e-16, 1e-16)


if __name__ == "__main__":
    test_one_input_one_output()
    test_two_inputs_one_output()
    test_two_outputs()
    test_stationary_fun()
