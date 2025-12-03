import openturns as ot
import openturns.viewer as otv

class SampleEstimatorLinearRegression(ot.PythonRandomVector):
    def __init__(self, sample_size, standard_deviation, coefficients, estimator, x0=None):
        """
        Create a RandomVector for an estimator from a linear regression model.
        """
        super(SampleEstimatorLinearRegression, self).__init__(1)
        self.sample_size = sample_size
        number_of_parameters = coefficients.getDimension()
        input_dimension = number_of_parameters - 1  # Because of the intercept
        center_coefficient = [0.0] * input_dimension
        constant_coefficient = [coefficients[0]]
        linear_coefficient = ot.Matrix([coefficients[1:]])
        self.linear_model = ot.LinearFunction(center_coefficient, constant_coefficient, linear_coefficient)
        self.distribution = ot.Normal(input_dimension)
        self.error_distribution = ot.Normal(0.0, standard_deviation)
        self.estimator = estimator
        self.x0 = x0
        # In classical linear regression, the input is deterministic
        # Here, we set it once and for all
        self.input_sample = self.distribution.getSample(self.sample_size)
        self.output_sample = self.linear_model(self.input_sample)

    def getRegressionResult(self):
        error_sample = self.error_distribution.getSample(self.sample_size)
        noisy_output_sample = self.output_sample + error_sample
        algo = ot.LinearModelAlgorithm(self.input_sample, noisy_output_sample)
        return algo.getResult()

    def getRealization(self):
        if self.estimator == "variance":
            result = self.getRegressionResult()
            return [result.getResidualsVariance()]
        elif self.estimator.startswith("coefficient"):
            result = self.getRegressionResult()
            index = int(self.estimator[-1])
            return [result.getCoefficients()[index]]
        elif self.estimator == "prediction":
            result = self.getRegressionResult()
            metamodel = result.getMetaModel()
            return metamodel(self.x0)
        elif self.estimator == "observation":
            error = self.error_distribution.getRealization()
            return self.linear_model(self.x0) + error

def center_distribution(distribution, mean):
    """
    Change the mean of the distribution but keep its variance.
    """
    if distribution.getClassName() == "Distribution":
        distribution = distribution.getImplementation()
    if distribution.getClassName() == "Normal":
        return ot.Normal(mean, distribution.getSigma())
    elif distribution.getClassName() == "Gamma":
        variance = distribution.getCovariance()[0, 0]
        corrected_rate = mean[0] / variance
        corrected_shape = corrected_rate * mean[0]
        return ot.Gamma(corrected_shape, corrected_rate)
    else:
        raise ValueError("Unsupported distribution type: %s" % distribution.getClassName())

def test_estimator(sample_size, standard_deviation, coefficients, repetitions_size, estimator, gaussian=None, x0=None):
    # Compute the asymptotic distribution of the estimator
    pyRV = SampleEstimatorLinearRegression(sample_size, standard_deviation, coefficients, estimator, x0)
    result = pyRV.getRegressionResult()
    analysis = ot.LinearModelAnalysis(result)
    if estimator == "variance":
        distribution = analysis.getVarianceDistribution(gaussian)
    elif estimator.startswith("coefficient"):
        index = int(estimator[-1])
        distribution = analysis.getCoefficientsDistribution().getMarginal(index)
    elif estimator == "prediction":
        distribution = analysis.getPredictionDistribution(x0)
    elif estimator == "observation":
        distribution = analysis.getOutputObservationDistribution(x0)
    else:
        raise ValueError("Unknown estimator: %s" % estimator)

    # Get an estimator sample
    RV = ot.RandomVector(pyRV)
    sample_estimator = RV.getSample(repetitions_size)

    # Center the distribution on the center of the data to correct for sample bias
    # This is because we are only interested in the dispersion, not the center of the distribution
    distribution = center_distribution(distribution, sample_estimator.computeMean())

    # Perform a Kolmogorov-Smirnov test
    test_result = ot.FittingTest.Kolmogorov(sample_estimator, distribution)
    print("p-value %s: %f" % (estimator, test_result.getPValue()))

    # Draw the QQ-plot
    graph = ot.VisualTest.DrawQQplot(sample_estimator, distribution)
    graph.setTitle("QQ-plot (%s)" % estimator)
    otv.View(graph)


sample_size = 1000
standard_deviation = 0.1
coefficients = ot.Point([3.0, 2.0, -1.0])
repetitions_size = 100
x0 = ot.Point([1.0, 2.0])

test_estimator(sample_size, standard_deviation, coefficients, repetitions_size, "variance", gaussian=True)
test_estimator(sample_size, standard_deviation, coefficients, repetitions_size, "coefficient_0")
test_estimator(sample_size, standard_deviation, coefficients, repetitions_size, "coefficient_1")
test_estimator(sample_size, standard_deviation, coefficients, repetitions_size, "coefficient_2")
test_estimator(sample_size, standard_deviation, coefficients, repetitions_size, "prediction", x0=x0)
test_estimator(sample_size, standard_deviation, coefficients, repetitions_size, "observation", x0=x0)

otv.View.ShowAll()
