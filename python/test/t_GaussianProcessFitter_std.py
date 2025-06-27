import openturns as ot
from openturns.experimental import GaussianProcessFitter
import openturns.testing as ott

ot.PlatformInfo.SetNumericalPrecision(4)


ot.TESTPREAMBLE()


def use_case_1(X, Y):
    """
    optim problem (scale)
    Dirac model
    """
    basis = ot.LinearBasisFactory(inputDimension).build()
    # Case of a misspecified covariance model
    covarianceModel = ot.DiracCovarianceModel(inputDimension)
    algo = GaussianProcessFitter(X, Y, covarianceModel, basis)
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
    basis = ot.LinearBasisFactory(inputDimension).build()
    # Case of a misspecified covariance model
    covarianceModel = ot.DiracCovarianceModel(inputDimension)
    algo = GaussianProcessFitter(X, Y, covarianceModel, basis)
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
    basis = ot.LinearBasisFactory(inputDimension).build()
    # Case of a misspecified covariance model
    covarianceModel = ot.AbsoluteExponential(inputDimension)
    algo = GaussianProcessFitter(X, Y, covarianceModel, basis)
    assert algo.getOptimizeParameters()
    algo.setKeepCholeskyFactor(False)
    algo.run()
    cov_param = [0.1327, 0.1956]
    trend_coefficients = [-0.1034, 1.0141]
    result = algo.getResult()
    assert (
        algo.getOptimizationAlgorithm().getImplementation().getClassName() == "Cobyla"
    )
    ott.assert_almost_equal(
        result.getCovarianceModel().getParameter(), cov_param, 1e-4, 1e-4
    )
    ott.assert_almost_equal(
        result.getTrendCoefficients(), trend_coefficients, 1e-4, 1e-4
    )


def use_case_4(X, Y):
    """
    optim problem (scale)
    Biased variance estimate
    """
    ot.ResourceMap.SetAsBool("GeneralLinearModelAlgorithm-UnbiasedVariance", False)
    basis = ot.LinearBasisFactory(inputDimension).build()
    # Case of a misspecified covariance model
    covarianceModel = ot.AbsoluteExponential(inputDimension)
    algo = GaussianProcessFitter(X, Y, covarianceModel, basis)
    assert algo.getKeepCholeskyFactor()
    assert algo.getOptimizeParameters()
    algo.setKeepCholeskyFactor(False)
    algo.run()
    cov_param = [0.1327, 0.1956]
    trend_coefficients = [-0.1034, 1.0141]
    result = algo.getResult()
    assert (
        algo.getOptimizationAlgorithm().getImplementation().getClassName() == "Cobyla"
    )
    ott.assert_almost_equal(
        result.getCovarianceModel().getParameter(), cov_param, 1e-4, 1e-4
    )
    ott.assert_almost_equal(
        result.getTrendCoefficients(), trend_coefficients, 1e-4, 1e-4
    )


def use_case_5(X, Y):
    """
    full optim problem (scale, amplitude)
    """
    ot.ResourceMap.SetAsBool("GaussianProcessFitter-UnbiasedVariance", False)
    ot.ResourceMap.SetAsBool(
        "GaussianProcessFitter-UseAnalyticalAmplitudeEstimate", False
    )
    basis = ot.LinearBasisFactory(inputDimension).build()
    # Case of a misspecified covariance model
    covarianceModel = ot.AbsoluteExponential(inputDimension)

    algo = GaussianProcessFitter(X, Y, covarianceModel, basis)
    assert algo.getKeepCholeskyFactor()
    assert algo.getOptimizeParameters()
    algo.setKeepCholeskyFactor(False)
    bounds = ot.Interval([1e-2] * 2, [100] * 2)
    algo.setOptimizationBounds(bounds)
    algo.run()

    cov_param = [0.1327, 0.19068]
    trend_coefficients = [-0.1034, 1.0141]
    result = algo.getResult()
    assert (
        algo.getOptimizationAlgorithm().getImplementation().getClassName() == "Cobyla"
    )
    ott.assert_almost_equal(
        result.getCovarianceModel().getParameter(), cov_param, 1e-4, 1e-4
    )
    ott.assert_almost_equal(
        result.getTrendCoefficients(), trend_coefficients, 1e-4, 1e-4
    )


def use_case_6(X, Y):
    ot.RandomGenerator.SetSeed(0)
    covarianceModel = ot.AbsoluteExponential()
    algo = GaussianProcessFitter(X, Y, covarianceModel)
    assert algo.getKeepCholeskyFactor()
    algo.setKeepCholeskyFactor(False)
    assert algo.getOptimizeParameters()
    algo.run()
    result = algo.getResult()
    cov_param = [15.6, 2.3680]
    assert (
        algo.getOptimizationAlgorithm().getImplementation().getClassName() == "Cobyla"
    )
    ott.assert_almost_equal(
        result.getCovarianceModel().getParameter(), cov_param, 1e-4, 1e-4
    )
    ott.assert_almost_equal(result.getTrendCoefficients(), [])


def bugfix_optim_no_feasible():
    from openturns.usecases.fire_satellite import FireSatelliteModel

    m = FireSatelliteModel()

    model = m.model
    inputDistribution = m.inputDistribution

    ot.RandomGenerator.SetSeed(0)
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
    optimal_cov_parameter = [1.05e+07, 1500, 1500, 50.7, 5.119, 4.807,
                             5.238, 2.45, 2.302, 5.058, 7.188, 4.915]
    covarianceModel = ot.TensorizedCovarianceModel([myCov1, myCov2, myCov3])

    scaleOptimizationBounds = ot.Interval(
        [1.0e6, 1.0e3, 1.0e3, 1.0, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1],
        [2.0e7, 2.0e3, 2.0e3, 1e2, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0],
    )

    algo = GaussianProcessFitter(
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
        result.getCovarianceModel().getParameter(), optimal_cov_parameter, 5e-2, 1e-3
    )
    ott.assert_almost_equal(squared_epsilon, [1.932e-05, 49.7, 1.861], 5e-1, 1e-3)


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
    # fix https://github.com/openturns/openturns/issues/2953
    bugfix_optim_no_feasible()
