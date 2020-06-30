#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from math import fabs
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)


continuousDistributionCollection = ot.DistributionCollection()
discreteDistributionCollection = ot.DistributionCollection()
distributionCollection = ot.DistributionCollection()

beta = ot.Beta(2.0, 1.0, 0.0, 1.0)
distributionCollection.add(beta)
continuousDistributionCollection.add(beta)

gamma = ot.Gamma(1.0, 2.0, 3.0)
distributionCollection.add(gamma)
continuousDistributionCollection.add(gamma)

gumbel = ot.Gumbel(1.0, 2.0)
distributionCollection.add(gumbel)
continuousDistributionCollection.add(gumbel)

lognormal = ot.LogNormal(1.0, 1.0, 2.0)
distributionCollection.add(lognormal)
continuousDistributionCollection.add(lognormal)

logistic = ot.Logistic(1.0, 1.0)
distributionCollection.add(logistic)
continuousDistributionCollection.add(logistic)

normal = ot.Normal(1.0, 2.0)
distributionCollection.add(normal)
continuousDistributionCollection.add(normal)

truncatednormal = ot.TruncatedNormal(1.0, 1.0, 0.0, 3.0)
distributionCollection.add(truncatednormal)
continuousDistributionCollection.add(truncatednormal)

student = ot.Student(10.0, 10.0)
distributionCollection.add(student)
continuousDistributionCollection.add(student)

triangular = ot.Triangular(-1.0, 2.0, 4.0)
distributionCollection.add(triangular)
continuousDistributionCollection.add(triangular)

uniform = ot.Uniform(1.0, 2.0)
distributionCollection.add(uniform)
continuousDistributionCollection.add(uniform)

weibull = ot.WeibullMin(1.0, 1.0, 2.0)
distributionCollection.add(weibull)
continuousDistributionCollection.add(weibull)

geometric = ot.Geometric(0.5)
distributionCollection.add(geometric)
discreteDistributionCollection.add(geometric)

binomial = ot.Binomial(10, 0.25)
distributionCollection.add(binomial)
discreteDistributionCollection.add(binomial)

zipf = ot.ZipfMandelbrot(20, 5.25, 2.5)
distributionCollection.add(zipf)
discreteDistributionCollection.add(zipf)

poisson = ot.Poisson(5.0)
distributionCollection.add(poisson)
discreteDistributionCollection.add(poisson)

x = [[1.0], [2.0], [3.0]]
p = [0.3, 0.2, 0.5]
userdefined = ot.UserDefined(x, p)
distributionCollection.add(userdefined)
discreteDistributionCollection.add(userdefined)

size = 100

# Number of continuous distributions
continuousDistributionNumber = continuousDistributionCollection.getSize()
# Number of discrete distributions
discreteDistributionNumber = discreteDistributionCollection.getSize()
# Number of distributions
distributionNumber = continuousDistributionNumber + \
    discreteDistributionNumber

# We create a collection of Sample of size "size" and of
# dimension 1 (scalar values) : the collection has distributionNumber
# Samples

sampleCollection = [ot.Sample(size, 1)
                    for i in range(distributionNumber)]
# We create a collection of Sample of size "size" and of
# dimension 1 (scalar values) : the collection has
# continuousDistributionNumber Samples
continuousSampleCollection = [ot.Sample(size, 1)
                              for i in range(continuousDistributionNumber)]
# We create a collection of Sample of size "size" and of
# dimension 1 (scalar values) : the collection has
# discreteDistributionNumber Samples
discreteSampleCollection = [ot.Sample(size, 1)
                            for i in range(discreteDistributionNumber)]

for i in range(continuousDistributionNumber):
    continuousSampleCollection[
        i] = continuousDistributionCollection[i].getSample(size)
    continuousSampleCollection[i].setName(
        continuousDistributionCollection[i].getName())
    sampleCollection[i] = continuousSampleCollection[i]
for i in range(discreteDistributionNumber):
    discreteSampleCollection[
        i] = discreteDistributionCollection[i].getSample(size)
    discreteSampleCollection[i].setName(
        discreteDistributionCollection[i].getName())
    sampleCollection[
        continuousDistributionNumber + i] = discreteSampleCollection[i]

factoryCollection = ot.DistributionFactoryCollection(3)
factoryCollection[0] = ot.UniformFactory()
factoryCollection[1] = ot.BetaFactory()
factoryCollection[2] = ot.NormalFactory()
aSample = ot.Uniform(-1.5, 2.5).getSample(size)
model, best_bic = ot.FittingTest.BestModelBIC(aSample, factoryCollection)
print("best model BIC=", repr(model))
model, best_result = ot.FittingTest.BestModelKolmogorov(
    aSample, factoryCollection)
print("best model Kolmogorov=", repr(model))

# BIC adequation
resultBIC = ot.SquareMatrix(distributionNumber)
for i in range(distributionNumber):
    for j in range(distributionNumber):
        value = ot.FittingTest.BIC(
            sampleCollection[i], distributionCollection[j], 0)
        resultBIC[i, j] = value
print("resultBIC=", repr(resultBIC))

# Kolmogorov test : case with estimated parameters
print("Kolmogorov test : case with estimated parameters")
distribution = ot.Normal()
sample = distribution.getSample(30)
factory = ot.NormalFactory()
ot.ResourceMap.SetAsUnsignedInteger(
    'FittingTest-KolmogorovSamplingSize', 10000)
fitted_dist, test_result = ot.FittingTest.Kolmogorov(sample, factory)
p_exact = 0.934674  # With a sample size equal to 1000000
pvalue = test_result.getPValue()
rtol = 0.0
atol = 1.e-2
ott.assert_almost_equal(pvalue, p_exact, rtol, atol)

# Kolmogorov test : case with known parameters
print("Kolmogorov test : case with known parameters")
# Data from PlantGrowth$weight in the MASS R package,
# except the first value which generates a tie
distribution = ot.Normal()
data = (5.58, 5.18, 6.11, 4.50, 4.61, 5.17, 4.53, 5.33,
        5.14, 4.81, 4.17, 4.41, 3.59, 5.87, 3.83, 6.03, 4.89,
        4.32, 4.69, 6.31, 5.12, 5.54, 5.50, 5.37, 5.29, 4.92,
        6.15, 5.80, 5.26)
sample = ot.Sample([[x] for x in data])
mean = sample.computeMean()
sample = sample - mean
test_result = ot.FittingTest.Kolmogorov(sample, distribution)
p_exact = 0.8053771610533257963
pvalue = test_result.getPValue()
ott.assert_almost_equal(pvalue, p_exact)
D = test_result.getStatistic()
D_exact = 0.11393533907737134203
ott.assert_almost_equal(D, D_exact)
quality = test_result.getBinaryQualityMeasure()
assert(quality)
threshold = test_result.getThreshold()
defaultLevel = 0.05
assert(threshold == defaultLevel)

# Kolmogorov test : case with known parameters, set the level
print("Kolmogorov test : case with known parameters, set the level")
distribution = ot.Normal()
sample = distribution.getSample(30)
level = 0.01
test_result = ot.FittingTest.Kolmogorov(sample, distribution, level)
threshold = test_result.getThreshold()
assert(threshold == level)

# Kolmogorov adequation
resultKolmogorov = ot.SquareMatrix(continuousDistributionNumber)
for i in range(continuousDistributionNumber):
    for j in range(continuousDistributionNumber):
        sample = continuousSampleCollection[i]
        distribution = continuousDistributionCollection[j]
        test_result = ot.FittingTest.Kolmogorov(sample, distribution, 0.05)
        pvalue = test_result.getPValue()
        if (fabs(pvalue) < 1.0e-6):
            value = 0.0
        resultKolmogorov[i, j] = value
print("resultKolmogorov=", repr(resultKolmogorov))

# ChiSquared adequation
resultChiSquared = ot.SquareMatrix(discreteDistributionNumber)
for i in range(discreteDistributionNumber):
    for j in range(discreteDistributionNumber):
        try:
            sample = continuousSampleCollection[i]
            distribution = continuousDistributionCollection[j]
            test_result = ot.FittingTest.ChiSquared(
                sample, distribution, 0.05, 0)
            pvalue = test_result.getPValue()
            if (fabs(pvalue) < 1.0e-6):
                value = 0.0
            resultChiSquared[i, j] = value
        except:
            print(
                "Sample=", discreteSampleCollection[i],
                " is not compatible with distribution=",
                discreteDistributionCollection[j])
print("resultChiSquared=", repr(resultChiSquared))
# Example taken from the R documentation of chisq.test
s = [[0.0]]*89 + [[1.0]]*37 + [[2.0]]*30 + [[3.0]]*28 + [[4.0]]*2
d = ot.UserDefined([[0.0], [1.0], [2.0], [3.0], [4.0]],
                    [0.4, 0.2, 0.2, 0.15, 0.05])
print("R example p-value=%.5g" %
      ot.FittingTest.ChiSquared(s, d).getPValue())
