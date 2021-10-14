#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Set hmat
ot.ResourceMap.SetAsString("KrigingAlgorithm-LinearAlgebra", "HMAT")


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

    # create algorithm
    basis = ot.ConstantBasisFactory(dimension).build()
    covarianceModel = ot.SquaredExponential([1e-02], [4.50736])

    algo = ot.KrigingAlgorithm(X, Y, covarianceModel, basis)
    algo.run()

    # perform an evaluation
    result = algo.getResult()

    ott.assert_almost_equal(result.getMetaModel()(X), Y, 1e-2)
    ott.assert_almost_equal(result.getResiduals(), [0.0], 0.0, 1e-2)
    ott.assert_almost_equal(result.getRelativeErrors(), [0.0], 0.0, 1e-5)

    # Kriging variance is 0 on learning points
    covariance = result.getConditionalCovariance(X)
    covariancePoint = ot.Point(covariance.getImplementation())
    theoricalVariance = ot.Point(sampleSize * sampleSize)
    ott.assert_almost_equal(covariance,
                            ot.Matrix(sampleSize, sampleSize),
                            0.0, 1e-1)

    # Covariance per marginal & extract variance component
    coll = result.getConditionalMarginalCovariance(X)
    var = [mat[0, 0] for mat in coll]
    ott.assert_almost_equal(var, [0]*sampleSize, 0.0, 1e-1)

    # Variance per marginal
    var = result.getConditionalMarginalVariance(X)
    ott.assert_almost_equal(var, ot.Sample(sampleSize, 1), 0.0, 1e-1)


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
    covarianceModel = ot.SquaredExponential([5.33532, 2.61534], [1.61536])

    # 3) Basis definition
    basis = ot.ConstantBasisFactory(inputDimension).build()
    # Kriging algorithm
    algo = ot.KrigingAlgorithm(inputSample, outputSample,
                               covarianceModel, basis)
    algo.run()
    result = algo.getResult()
    # Get meta model
    metaModel = result.getMetaModel()
    outData = metaModel(inputValidSample)

    # 4) Errors
    # Interpolation
    ott.assert_almost_equal(
        outputSample,  metaModel(inputSample), 1, 3.0e-5)

    # 5) Kriging variance is 0 on learning points
    covariance = result.getConditionalCovariance(inputSample)
    ott.assert_almost_equal(
        covariance, ot.SquareMatrix(len(inputSample)), 0.0, 1e-3)

    # Covariance per marginal & extract variance component
    coll = result.getConditionalMarginalCovariance(inputSample)
    var = [mat[0, 0] for mat in coll]
    ott.assert_almost_equal(var, [0]*len(var), 0.0, 1e-3)

    # Variance per marginal
    var = result.getConditionalMarginalVariance(inputSample)
    ott.assert_almost_equal(var, ot.Sample(inputSample.getSize(), 1), 0.0, 1e-3)
    # Estimation
    ott.assert_almost_equal(outputValidSample,  metaModel(
        inputValidSample), 1.e-1, 1e-1)

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
    ott.assert_almost_equal(variance, ot.Sample(len(x), 1), 2e-6, 2e-6)

if __name__ == "__main__":
    test_one_input_one_output()
    test_two_inputs_one_output()
    test_stationary_fun()
