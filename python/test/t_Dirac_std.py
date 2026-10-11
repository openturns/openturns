#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# 1D tests
distribution = ot.Dirac(0.7)
assert distribution.getClassName() == "Dirac"

# Is this distribution elliptical ?
assert distribution.isElliptical()

# Is this distribution continuous ?
assert not distribution.isContinuous()

# Non-integer atoms are not integral
assert not distribution.isIntegral()
assert ot.Dirac(2.0).isIntegral()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [0.7], 0.0, 0.0)

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
ott.assert_almost_equal(oneSample[0], [0.7], 0.0, 0.0)
ott.assert_almost_equal(oneSample[size - 1], [0.7], 0.0, 0.0)
ott.assert_almost_equal(oneSample.computeMean(), [0.7], 1e-12, 0.0)
sampleCovariance = oneSample.computeCovariance()[0, 0]
if abs(sampleCovariance) < 1.0e-16:
    sampleCovariance = 0.0
ott.assert_almost_equal(sampleCovariance, 0.0, 0.0, 0.0)

# Define a point
point = ot.Point(distribution.getDimension(), 0.0)
ott.assert_almost_equal(point, [0.0], 0.0, 0.0)

# Show PDF and CDF of point
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.0, 0.0, 0.0)
ott.assert_almost_equal(
    distribution.computeCDF(point + ot.Point(1, 0))
    - distribution.computeCDF(point + ot.Point(1, -1)),
    0.0,
    0.0,
    0.0,
)
CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.0, 0.0, 0.0)

# Define a point
point = ot.Point(distribution.getSupport(distribution.getRange())[0])
ott.assert_almost_equal(point, [0.7], 0.0, 0.0)

# Show PDF and CDF of point
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 1.0, 0.0, 0.0)
ott.assert_almost_equal(
    distribution.computeCDF(point + ot.Point(1, 0))
    - distribution.computeCDF(point + ot.Point(1, -1)),
    1.0,
    0.0,
    0.0,
)
CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 1.0, 0.0, 0.0)
CF = distribution.computeCharacteristicFunction(0.5)
ott.assert_almost_equal([CF.real], [0.939372712847], 1e-9, 0.0)
ott.assert_almost_equal([CF.imag], [0.342897807455], 1e-9, 0.0)
GF = distribution.computeGeneratingFunction(0.5 + 0.3j)
ott.assert_almost_equal([GF.real], [0.637047453196], 1e-9, 0.0)
ott.assert_almost_equal([GF.imag], [0.253185274612], 1e-9, 0.0)
quantile = distribution.computeQuantile(0.95)
# Test invalid quantile
with ott.assert_raises(TypeError):
    distribution.computeScalarQuantile(1.2)
ott.assert_almost_equal(quantile, [0.7], 0.0, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 1.0, 0.0, 0.0)
ott.assert_almost_equal(distribution.computeEntropy(), 0.0, 0.0, 0.0)
mean = distribution.getMean()
ott.assert_almost_equal(mean, [0.7], 0.0, 0.0)
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [0.0], 0.0, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [0.0], 0.0, 0.0)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [0.0], 0.0, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [0.0]), 0.0, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
ott.assert_almost_equal(parameters[0], [0.7], 0.0, 0.0)
standard = distribution.getStandardRepresentative()
assert standard.getImplementation().getClassName() == "Dirac"

# N-D tests

dim = 4
distribution = ot.Dirac(ot.Point(dim, 2.3))

# Is this distribution elliptical ?
assert distribution.isElliptical()

# Is this distribution continuous ?
assert not distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [2.3, 2.3, 2.3, 2.3], 0.0, 0.0)

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
ott.assert_almost_equal(oneSample[0], [2.3, 2.3, 2.3, 2.3], 0.0, 0.0)
ott.assert_almost_equal(oneSample[size - 1], [2.3, 2.3, 2.3, 2.3], 0.0, 0.0)
ott.assert_almost_equal(oneSample.computeMean(), [2.3, 2.3, 2.3, 2.3], 1e-12, 0.0)
sampleCovariance = oneSample.computeCovariance()
for i in range(dim):
    for j in range(i + 1):
        if abs(sampleCovariance[i, j]) < 1.0e-16:
            sampleCovariance[i, j] = 0.0
ott.assert_almost_equal(sampleCovariance, ot.CovarianceMatrix(dim, [0.0] * 16), 0.0, 0.0)

# Define a point
point = ot.Point(dim, 0.0)

# Show PDF and CDF of point
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.0, 0.0, 0.0)
ott.assert_almost_equal(
    distribution.computeCDF(point + ot.Point(dim, 0))
    - distribution.computeCDF(point + ot.Point(dim, -1)),
    0.0,
    0.0,
    0.0,
)
CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.0, 0.0, 0.0)

# Define a point
point = ot.Point(distribution.getSupport(distribution.getRange())[0])

# Show PDF and CDF of point
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 1.0, 0.0, 0.0)
ott.assert_almost_equal(
    distribution.computeCDF(point + ot.Point(dim, 0))
    - distribution.computeCDF(point + ot.Point(dim, -1)),
    1.0,
    0.0,
    0.0,
)
CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 1.0, 0.0, 0.0)

quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [2.3, 2.3, 2.3, 2.3], 0.0, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 1.0, 0.0, 0.0)
mean = distribution.getMean()
ott.assert_almost_equal(mean, [2.3, 2.3, 2.3, 2.3], 0.0, 0.0)
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [0.0, 0.0, 0.0, 0.0], 0.0, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [0.0, 0.0, 0.0, 0.0], 0.0, 0.0)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [0.0, 0.0, 0.0, 0.0], 0.0, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(dim, [0.0] * 16), 0.0, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == dim
for parameter in parameters:
    ott.assert_almost_equal(parameter, [2.3], 0.0, 0.0)
standard = distribution.getStandardRepresentative()
assert standard.getImplementation().getClassName() == "Dirac"

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipMoments()
validation.skipCorrelation()
validation.skipConditional()  # FIXME
validation.run()
