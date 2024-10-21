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
