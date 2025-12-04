#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
from math import sin, sqrt

ot.TESTPREAMBLE()


# lm build
print("Fit y ~ 3 - 2 x + 0.05 * sin(x) model using 20 points (sin(x) ~ noise)")
size = 20
oneSample = ot.Sample(size, 1)
twoSample = ot.Sample(size, 1)
for i in range(size):
    oneSample[i, 0] = 7.0 * sin(-3.5 + (6.5 * i) / (size - 1.0)) + 2.0
    twoSample[i, 0] = -2.0 * oneSample[i, 0] + 3.0 + 0.05 * sin(oneSample[i, 0])

test = ot.LinearModelAlgorithm(oneSample, twoSample)
result = ot.LinearModelResult(test.getResult())
analysis = ot.LinearModelAnalysis(result)
print(analysis)
# Compute confidence level (95%) for coefficients estimate
alpha = 0.95
# interval confidence bounds
interval = analysis.getCoefficientsConfidenceInterval(alpha)
print("confidence intervals with level=%1.2f : %s" % (alpha, interval))

print("")
print("")
print("")
print("Fit y ~ 1 + 0.1 x + 10 x^2 model using 100 points")
ot.RandomGenerator.SetSeed(0)
size = 100
# Define a linespace from 0 to 10 with size points
# We use a Box experiment ==> remove 0 & 1 points
experiment = ot.Box([size - 2])
X = experiment.generate()
# X is defined in [0,1]
X *= [10]
# Stack X2
X2 = ot.Sample(X)
for i in range(size):
    X2[i, 0] = X[i, 0] * X2[i, 0]
X.stack(X2)
# Define y
Y = ot.Sample(size, 1)
for i in range(size):
    Y[i, 0] = (
        1.0 + 0.1 * X[i, 0] + 10.0 * X[i, 0] * X[i, 0] + 0.1 * ot.DistFunc.rNormal()
    )
test = ot.LinearModelAlgorithm(X, Y)
result = test.getResult()
analysis = ot.LinearModelAnalysis(result)
print(analysis)
# Compute confidence level (95%) for coefficients estimate
alpha = 0.95
# interval confidence bounds
interval = analysis.getCoefficientsConfidenceInterval(alpha)
print("confidence intervals with level=%1.2f : %s" % (alpha, interval))

# https://github.com/openturns/openturns/issues/1729
ot.RandomGenerator.SetSeed(1789)
sample_size = 14605  # 14604 : OK, 14605 : Fail
a0 = 0.6
a1 = 2.0
x = ot.Normal(0.5, 0.2).getSample(sample_size)
epsilon = ot.Exponential(0.1).getSample(sample_size)
y = ot.Sample([[a0]] * sample_size) + a1 * x + epsilon
algo = ot.LinearModelAlgorithm(x, y)
result = algo.getResult()
analysis = ot.LinearModelAnalysis(result)
results = str(analysis)

# Fix #1820
ot.RandomGenerator.SetSeed(0)
distribution = ot.Normal()
sample = distribution.getSample(30)
func = ot.SymbolicFunction("x", "2 * x + 1")
firstSample = sample
secondSample = func(sample) + ot.Normal().getSample(30)
linear_model = ot.LinearModelAlgorithm(firstSample, secondSample)
linear_result = linear_model.getResult()
test_result = ot.LinearModelTest.LinearModelFisher(
    firstSample, secondSample, linear_result
)
linear_model_analysis = ot.LinearModelAnalysis(linear_result)
ott.assert_almost_equal(
    test_result.getStatistic(), linear_model_analysis.getFisherScore(), 1e-13, 0
)
ott.assert_almost_equal(
    test_result.getPValue(), linear_model_analysis.getFisherPValue(), 0, 0
)

# Tests for asymptotic distributions

ot.RandomGenerator.SetSeed(0)
sample_size = 1000
true_standard_deviation = 0.1
coefficients = ot.Point([3.0, 2.0, -1.0])
f = ot.SymbolicFunction(["x1", "x2", "a0", "a1", "a2"], ["a0 + a1 * x1 + a2 * x2"])
model = ot.ParametricFunction(f, [2, 3, 4], coefficients)
number_of_parameters = len(coefficients)
input_dimension = number_of_parameters - 1
distribution = ot.Normal(input_dimension)
error_distribution = ot.Normal(0.0, true_standard_deviation)
input_sample = distribution.getSample(sample_size)
output_sample = model(input_sample)
error_sample = error_distribution.getSample(sample_size)
noisy_output_sample = output_sample + error_sample
algo = ot.LinearModelAlgorithm(input_sample, noisy_output_sample)
result = algo.getResult()
analysis = ot.LinearModelAnalysis(result)

# Asymptotic coefficients distribution
coefficients_distribution = analysis.getCoefficientsDistribution()
assert coefficients_distribution.getClassName() == "Normal"
assert coefficients_distribution.getDimension() == 3
computed_parameters0 = coefficients_distribution.getMarginal(0).getParameter()
computed_parameters1 = coefficients_distribution.getMarginal(1).getParameter()
computed_parameters2 = coefficients_distribution.getMarginal(2).getParameter()
"""
true_variance = true_standard_deviation**2
designMatrix = result.getDesign()
gramMatrix = designMatrix.computeGram()
inverseGramMatrix = gramMatrix.inverse()
covarianceMatrix = ot.CovarianceMatrix(true_variance * inverseGramMatrix)
expected_distribution = ot.Normal(coefficients, covarianceMatrix)
expected_parameters0 = expected_distribution.getMarginal(0).getParameter()
expected_parameters1 = expected_distribution.getMarginal(1).getParameter()
expected_parameters2 = expected_distribution.getMarginal(2).getParameter()
"""
expected_parameters0 = ot.Point([3.0, 0.00316346])
expected_parameters1 = ot.Point([2.0, 0.00320948])
expected_parameters2 = ot.Point([-1.0, 0.00316354])
atol = 2.0e-1 / sqrt(sample_size)
ott.assert_almost_equal(computed_parameters0, expected_parameters0, 0.0, atol)
atol = 2.0e-3 / sqrt(sample_size)
ott.assert_almost_equal(computed_parameters1, expected_parameters1, 0.0, atol)
atol = 7.0e-2 / sqrt(sample_size)
ott.assert_almost_equal(computed_parameters2, expected_parameters2, 0.0, atol)

# Asymptotic variance distribution (Gaussian noise)
variance_distribution = analysis.getVarianceDistribution(True)
assert variance_distribution.getImplementation().getClassName() == "Gamma"
computed_parameters = variance_distribution.getParameter()
"""
true_variance = true_standard_deviation**2
shape_parameter = (sample_size - number_of_parameters) / 2
scale_parameter = 2 * true_variance / (sample_size - number_of_parameters)
rate_parameter = 1.0 / scale_parameter
location_parameter = 0.0
expected_parameters = ot.Point([shape_parameter, rate_parameter, location_parameter])
"""
expected_parameters = ot.Point([498.5, 49850.0, 0.0])
atol = 5.0e4 / sqrt(sample_size)
ott.assert_almost_equal(computed_parameters, expected_parameters, 0.0, atol)

# Asymptotic variance distribution (arbitrary noise)
variance_distribution = analysis.getVarianceDistribution(False)
assert variance_distribution.getImplementation().getClassName() == "Normal"
computed_parameters = variance_distribution.getParameter()
"""
true_variance = true_standard_deviation**2
distribution_mean = true_variance
distribution_variance = 2 * true_variance**2 / (sample_size - number_of_parameters)
distribution_std = sqrt(distribution_variance)
expected_parameters = ot.Point([distribution_mean, distribution_std])
"""
expected_parameters = ot.Point([0.01, 0.000447886])
atol = 9.0e-3 / sqrt(sample_size)
ott.assert_almost_equal(computed_parameters, expected_parameters, 0.0, atol)

# Asymptotic prediction distribution
x0 = ot.Point([1.5, 2.5])
prediction_distribution = analysis.getPredictionDistribution(x0)
assert prediction_distribution.getClassName() == "Normal"
computed_parameters = prediction_distribution.getParameter()
"""
true_variance = true_standard_deviation**2
x = ot.Matrix(number_of_parameters, 1)
basis = result.getBasis()
for i in range(number_of_parameters):
    f = basis.build(i)
    x[i, 0] = f(x0)[0]
prediction = x.transpose() * coefficients
designMatrix = result.getDesign()
gramMatrix = designMatrix.computeGram()
inverseGramMatrix = gramMatrix.inverse()
m = x.transpose() * inverseGramMatrix * x
sigma = sqrt(m[0, 0] * true_variance)
expected_parameters = ot.Point([prediction[0], sigma])
"""
expected_parameters = ot.Point([3.5, 0.00978034])
atol = 4.0e-1 / sqrt(sample_size)
ott.assert_almost_equal(computed_parameters, expected_parameters, 0.0, atol)

# Asymptotic observation distribution
x0 = ot.Point([1.5, 2.5])
observation_distribution = analysis.getOutputObservationDistribution(x0)
assert observation_distribution.getClassName() == "Normal"
computed_parameters = observation_distribution.getParameter()
"""
true_variance = true_standard_deviation**2
x = ot.Matrix(number_of_parameters, 1)
basis = result.getBasis()
for i in range(number_of_parameters):
    f = basis.build(i)
    x[i, 0] = f(x0)[0]
prediction = x.transpose() * coefficients
designMatrix = result.getDesign()
gramMatrix = designMatrix.computeGram()
inverseGramMatrix = gramMatrix.inverse()
m = x.transpose() * inverseGramMatrix * x
sigma = sqrt((1 + m[0, 0]) * true_variance)
expected_parameters = ot.Point([prediction[0], sigma])
"""
expected_parameters = ot.Point([3.5, 0.100477])
atol = 4.0e-1 / sqrt(sample_size)
ott.assert_almost_equal(computed_parameters, expected_parameters, 0.0, atol)
