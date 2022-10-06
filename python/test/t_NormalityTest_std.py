#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

continuousDistributionCollection = ot.DistributionCollection()
discreteDistributionCollection = ot.DistributionCollection()
distributionCollection = ot.DistributionCollection()

beta = ot.Beta(2., 1., 0., 1.)
distributionCollection.add(beta)
continuousDistributionCollection.add(beta)

gamma = ot.Gamma(1., 2., 3.)
distributionCollection.add(gamma)
continuousDistributionCollection.add(gamma)

gumbel = ot.Gumbel(1., 2.)
distributionCollection.add(gumbel)
continuousDistributionCollection.add(gumbel)

lognormal = ot.LogNormal(1., 1., 2.)
distributionCollection.add(lognormal)
continuousDistributionCollection.add(lognormal)

logistic = ot.Logistic(1., 1.)
distributionCollection.add(logistic)
continuousDistributionCollection.add(logistic)

normal = ot.Normal(1., 2.)
distributionCollection.add(normal)
continuousDistributionCollection.add(normal)

truncatednormal = ot.TruncatedNormal(1., 1., 0., 3.)
distributionCollection.add(truncatednormal)
continuousDistributionCollection.add(truncatednormal)

student = ot.Student(10., 10., 1.)
distributionCollection.add(student)
continuousDistributionCollection.add(student)

triangular = ot.Triangular(-1., 2., 4.)
distributionCollection.add(triangular)
continuousDistributionCollection.add(triangular)

uniform = ot.Uniform(1., 2.)
distributionCollection.add(uniform)
continuousDistributionCollection.add(uniform)

weibull = ot.WeibullMin(1., 1., 2.)
distributionCollection.add(weibull)
continuousDistributionCollection.add(weibull)

geometric = ot.Geometric(.5)
distributionCollection.add(geometric)
discreteDistributionCollection.add(geometric)

poisson = ot.Poisson(2.)
distributionCollection.add(poisson)
discreteDistributionCollection.add(poisson)

x = [[1.0], [2.0], [3.0]]
p = [0.3, 0.2, 0.5]
userdefined = ot.UserDefined(x, p)
distributionCollection.add(userdefined)
discreteDistributionCollection.add(userdefined)

size = 10000

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

# Test the normality of several samples using the Anderson Darling test
andersonDarlingResult = ot.Point(distributionNumber)
for i in range(distributionNumber):
    result = ot.NormalityTest.AndersonDarlingNormal(sampleCollection[i])
    andersonDarlingResult[i] = result.getBinaryQualityMeasure()
    print("sample ", sampleCollection[
          i].getName(), " result=", andersonDarlingResult[i])
print("andersonDarlingResult=", repr(andersonDarlingResult))

# Test the normality of several samples using the Cramer Von Mises test
cramerVonMisesResult = ot.Point(distributionNumber)
for i in range(distributionNumber):
    result = ot.NormalityTest.CramerVonMisesNormal(sampleCollection[i])
    cramerVonMisesResult[i] = result.getBinaryQualityMeasure()
    print("sample ", sampleCollection[
          i].getName(), " result=", cramerVonMisesResult[i])
print("cramerVonMisesResult=", repr(cramerVonMisesResult))
