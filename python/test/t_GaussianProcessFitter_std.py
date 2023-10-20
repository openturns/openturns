import openturns as ot
from openturns.experimental import GaussianProcessFitter
import openturns.testing as ott


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
    algo.run()
    cov_amplitude = [0.19574963607179355]
    trend_coefficients = [-0.11094267255, 1.01498279538]
    result = algo.getResult()
    ott.assert_almost_equal(result.getCovarianceModel().getAmplitude(), cov_amplitude, 1e-5, 1e-5)
    ott.assert_almost_equal(result.getTrendCoefficients(), trend_coefficients, 1e-5, 1e-5)


def use_case_2(X, Y):
    """
    No optim with Dirac model
    """
    basis = ot.LinearBasisFactory(inputDimension).build()
    # Case of a misspecified covariance model
    covarianceModel = ot.DiracCovarianceModel(inputDimension)
    algo = GaussianProcessFitter(X, Y, covarianceModel, basis)
    algo.setOptimizeParameters(False)
    algo.run()
    cov_amplitude = [1]
    trend_coefficients = [-0.11094267255, 1.01498279538]
    result = algo.getResult()
    ott.assert_almost_equal(result.getCovarianceModel().getAmplitude(), cov_amplitude, 1e-5, 1e-5)
    ott.assert_almost_equal(result.getTrendCoefficients(), trend_coefficients, 1e-5, 1e-5)


def use_case_3(X, Y):
    """
    full optim problem (scale)
    analytical variance estimate
    """
    basis = ot.LinearBasisFactory(inputDimension).build()
    # Case of a misspecified covariance model
    covarianceModel = ot.AbsoluteExponential(inputDimension)
    algo = GaussianProcessFitter(X, Y, covarianceModel, basis)
    algo.run()
    cov_param = [0.132772599575, 0.195632030235]
    trend_coefficients = [-0.10342495271, 1.01410795541]
    result = algo.getResult()
    ott.assert_almost_equal(result.getCovarianceModel().getParameter(), cov_param, 1e-5, 1e-5)
    ott.assert_almost_equal(result.getTrendCoefficients(), trend_coefficients, 1e-5, 1e-5)


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
    algo.run()
    cov_param = [0.132772599575, 0.195632030235]
    trend_coefficients = [-0.10342495271, 1.01410795541]
    result = algo.getResult()
    ott.assert_almost_equal(result.getCovarianceModel().getParameter(), cov_param, 1e-5, 1e-5)
    ott.assert_almost_equal(result.getTrendCoefficients(), trend_coefficients, 1e-5, 1e-5)


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
    bounds = ot.Interval([1e-2] * 2, [100] * 2)
    algo.setOptimizationBounds(bounds)
    algo.run()

    cov_param = [0.132775532444, 0.190698691094]
    trend_coefficients = [-0.103424639518, 1.01410791907]
    result = algo.getResult()
    ott.assert_almost_equal(result.getCovarianceModel().getParameter(), cov_param, 1e-5, 1e-5)
    ott.assert_almost_equal(result.getTrendCoefficients(), trend_coefficients, 1e-5, 1e-5)


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
