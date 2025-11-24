import openturns as ot
import openturns.testing as ott
from openturns.usecases.fire_satellite import FireSatelliteModel

ot.TESTPREAMBLE()


def use_case_1(X, Y):
    """
    optim problem (scale)
    Dirac model
    """
    ot.ResourceMap.Reload()
    basis = ot.LinearBasisFactory(inputDimension).build()
    # Case of a misspecified covariance model
    covarianceModel = ot.DiracCovarianceModel(inputDimension)
    algo = ot.GaussianProcessFitter(X, Y, covarianceModel, basis)
    assert algo.getOptimizeParameters()
    assert algo.getKeepCholeskyFactor()
    algo.setKeepCholeskyFactor(False)
    algo.run()
    cov_amplitude = [0.19575]
    trend_coefficients = [-0.1109, 1.0149]
    result = algo.getResult()
    ott.assert_almost_equal(
        result.getCovarianceModel().getAmplitude(), cov_amplitude, 1e-4, 1e-4
    )
    ott.assert_almost_equal(
        result.getTrendCoefficients(), trend_coefficients, 1e-4, 1e-4
    )


def use_case_2(X, Y):
    """
    No optim with Dirac model
    """
    ot.ResourceMap.Reload()
    basis = ot.LinearBasisFactory(inputDimension).build()
    # Case of a misspecified covariance model
    covarianceModel = ot.DiracCovarianceModel(inputDimension)
    algo = ot.GaussianProcessFitter(X, Y, covarianceModel, basis)
    assert algo.getKeepCholeskyFactor()
    algo.setKeepCholeskyFactor(False)
    algo.setOptimizeParameters(False)
    algo.run()
    cov_amplitude = [1]
    trend_coefficients = [-0.1109, 1.01498]
    result = algo.getResult()
    ott.assert_almost_equal(
        result.getCovarianceModel().getAmplitude(), cov_amplitude, 1e-4, 1e-4
    )
    ott.assert_almost_equal(
        result.getTrendCoefficients(), trend_coefficients, 1e-4, 1e-4
    )


def use_case_3(X, Y):
    """
    full optim problem (scale)
    analytical variance estimate
    """
    ot.ResourceMap.Reload()
    basis = ot.LinearBasisFactory(inputDimension).build()
    # Case of a misspecified covariance model
    covarianceModel = ot.AbsoluteExponential(inputDimension)
    algo = ot.GaussianProcessFitter(X, Y, covarianceModel, basis)
    assert algo.getOptimizeParameters()
    algo.setKeepCholeskyFactor(False)
    algo.run()
    cov_param = [0.0078, 1]
    trend_coefficients = [-0.110943, 1.01498]
    result = algo.getResult()
    assert (
        algo.getOptimizationAlgorithm().getImplementation().getClassName() == "Cobyla"
    )
    print(result.getCovarianceModel().getParameter())
    ott.assert_almost_equal(
        result.getCovarianceModel().getParameter(), cov_param, 1e-4, 1e-4
    )
    print(result.getTrendCoefficients())
    ott.assert_almost_equal(
        result.getTrendCoefficients(), trend_coefficients, 1e-4, 1e-4
    )


def use_case_4(X, Y):
    """
    optim problem (scale)
    Biased variance estimate
    """
    ot.ResourceMap.Reload()
    ot.ResourceMap.SetAsBool("GeneralLinearModelAlgorithm-UnbiasedVariance", False)
    basis = ot.LinearBasisFactory(inputDimension).build()
    # Case of a misspecified covariance model
    covarianceModel = ot.AbsoluteExponential(inputDimension)
    algo = ot.GaussianProcessFitter(X, Y, covarianceModel, basis)
    assert algo.getKeepCholeskyFactor()
    assert algo.getOptimizeParameters()
    algo.setKeepCholeskyFactor(False)
    algo.run()
    result = algo.getResult()
    assert (
        algo.getOptimizationAlgorithm().getImplementation().getClassName() == "Cobyla"
    )
    print(result.getCovarianceModel().getParameter())
    ott.assert_almost_equal(
        result.getCovarianceModel().getParameter(), [0.0078, 1], 1e-4, 1e-4
    )
    print(result.getTrendCoefficients())
    ott.assert_almost_equal(
        result.getTrendCoefficients(), [-0.110943, 1.01498], 1e-4, 1e-4
    )


def use_case_5(X, Y):
    """
    full optim problem (scale, amplitude)
    """
    ot.ResourceMap.Reload()
    ot.ResourceMap.SetAsBool("GaussianProcessFitter-UnbiasedVariance", False)
    ot.ResourceMap.SetAsBool(
        "GaussianProcessFitter-UseAnalyticalAmplitudeEstimate", False
    )
    basis = ot.LinearBasisFactory(inputDimension).build()
    # Case of a misspecified covariance model
    covarianceModel = ot.AbsoluteExponential(inputDimension)

    algo = ot.GaussianProcessFitter(X, Y, covarianceModel, basis)
    assert algo.getKeepCholeskyFactor()
    assert algo.getOptimizeParameters()
    algo.setKeepCholeskyFactor(False)
    bounds = ot.Interval([1e-2] * 2, [100] * 2)
    algo.setOptimizationBounds(bounds)
    algo.run()
    result = algo.getResult()
    assert (
        algo.getOptimizationAlgorithm().getImplementation().getClassName() == "Cobyla"
    )
    print(result.getCovarianceModel().getParameter())
    cov_param = [8.499, 0.9998]
    ott.assert_almost_equal(
        result.getCovarianceModel().getParameter(), cov_param, 1e-3, 1e-3
    )
    print(result.getTrendCoefficients())
    trend_coefficients = [0.09867, 0.995]
    ott.assert_almost_equal(
        result.getTrendCoefficients(), trend_coefficients, 1e-4, 1e-4
    )


def use_case_6(X, Y):
    ot.RandomGenerator.SetSeed(0)
    ot.ResourceMap.Reload()
    ot.ResourceMap.SetAsBool(
        "GaussianProcessFitter-UseAnalyticalAmplitudeEstimate", False
    )
    covarianceModel = ot.AbsoluteExponential()
    algo = ot.GaussianProcessFitter(X, Y, covarianceModel)
    assert algo.getKeepCholeskyFactor()
    algo.setKeepCholeskyFactor(False)
    assert algo.getOptimizeParameters()
    algo.run()
    result = algo.getResult()
    assert (
        algo.getOptimizationAlgorithm().getImplementation().getClassName() == "Cobyla"
    )
    print(result.getCovarianceModel().getParameter())
    cov_param = [2.55922, 1]
    ott.assert_almost_equal(
        result.getCovarianceModel().getParameter(), cov_param, 1e-4, 1e-4
    )
    ott.assert_almost_equal(result.getTrendCoefficients(), [])


def use_case_7(X, Y):
    ot.RandomGenerator.SetSeed(0)
    ot.ResourceMap.Reload()
    ot.ResourceMap.SetAsScalar("GaussianProcessFitter-OptimizationLowerBoundScaleFactor", 0.0)
    covarianceModel = ot.AbsoluteExponential()
    with ott.assert_raises(TypeError):
        algo = ot.GaussianProcessFitter(X, Y, covarianceModel)
        algo.run()


def bugfix_optim_no_feasible():
    ot.RandomGenerator.SetSeed(0)
    ot.ResourceMap.Reload()

    m = FireSatelliteModel()
    model = m.model
    inputDistribution = m.distribution

    experiment = ot.LHSExperiment(inputDistribution, 10 * m.dim)
    inputTrainingSet = experiment.generate()
    outputTrainingSet = model(inputTrainingSet)
    linear_basis = ot.LinearBasisFactory(m.dim).build()
    basis = ot.Basis(
        [
            ot.AggregatedFunction([linear_basis.build(k)] * 3)
            for k in range(linear_basis.getSize())
        ]
    )

    myCov1 = ot.MaternModel([1.0] * m.dim, 2.5)
    myCov2 = ot.SquaredExponential([1.0] * m.dim)
    myCov3 = ot.MaternModel([1.0] * m.dim, 2.5)

    # optimal we should get after the optimization process
    optimal_cov_parameter = [7.874e+06, 1513, 1413, 43.96, 5.509, 4.625,
                             5.159, 0.6612, 3.245, 4.945, 8.074, 5.419]
    covarianceModel = ot.TensorizedCovarianceModel([myCov1, myCov2, myCov3])

    scaleOptimizationBounds = ot.Interval(
        [1.0e6, 1.0e3, 1.0e3, 1.0, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1],
        [2.0e7, 2.0e3, 2.0e3, 1e2, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0],
    )

    algo = ot.GaussianProcessFitter(
        inputTrainingSet, outputTrainingSet, covarianceModel, basis
    )
    algo.setOptimizationBounds(scaleOptimizationBounds)
    algo.setOptimizeParameters(True)
    algo.run()
    # Get result & residual
    result = algo.getResult()
    residual = result.getMetaModel()(inputTrainingSet) - outputTrainingSet
    # Define multivariate square function
    sqr_func = ot.SymbolicFunction(["x", "y", "z"], ["x*x", "y*y", "z*z"])
    # Squared residual
    squared_epsilon = sqr_func(residual).computeMean()
    ott.assert_almost_equal(
        result.getCovarianceModel().getParameter(), optimal_cov_parameter, 1e-1, 1e-3
    )
    ott.assert_almost_equal(squared_epsilon, [7.248e-06, 298.4, 0.9051], 5e-1, 1e-3)


if __name__ == "__main__":

    ot.RandomGenerator.SetSeed(0)
    sampleSize = 40
    inputDimension = 1

    # Create the function to estimate
    model = ot.SymbolicFunction(["x0"], ["x0"])

    X = ot.Sample(sampleSize, inputDimension)
    for i in range(sampleSize):
        X[i, 0] = 3.0 + (8.0 * i) / sampleSize
    Y = model(X)

    # Add a small noise to data
    Y += (
        ot.GaussianProcess(ot.AbsoluteExponential([0.1], [0.2]), ot.Mesh(X))
        .getRealization()
        .getValues()
    )
    use_case_1(X, Y)
    use_case_2(X, Y)
    use_case_3(X, Y)
    use_case_4(X, Y)
    use_case_5(X, Y)
    use_case_6(X, Y)
    use_case_7(X, Y)
    # fix https://github.com/openturns/openturns/issues/2953
    bugfix_optim_no_feasible()
