#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import fabs

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    continuousDistributionCollection = DistributionCollection()
    discreteDistributionCollection = DistributionCollection()
    distributionCollection = DistributionCollection()

    beta = Beta(2.0, 3.0, 0.0, 1.0)
    distributionCollection.add(beta)
    continuousDistributionCollection.add(beta)

    gamma = Gamma(1.0, 2.0, 3.0)
    distributionCollection.add(gamma)
    continuousDistributionCollection.add(gamma)

    gumbel = Gumbel(1.0, 2.0)
    distributionCollection.add(gumbel)
    continuousDistributionCollection.add(gumbel)

    lognormal = LogNormal(1.0, 1.0, 2.0)
    distributionCollection.add(lognormal)
    continuousDistributionCollection.add(lognormal)

    logistic = Logistic(1.0, 1.0)
    distributionCollection.add(logistic)
    continuousDistributionCollection.add(logistic)

    normal = Normal(1.0, 2.0)
    distributionCollection.add(normal)
    continuousDistributionCollection.add(normal)

    truncatednormal = TruncatedNormal(1.0, 1.0, 0.0, 3.0)
    distributionCollection.add(truncatednormal)
    continuousDistributionCollection.add(truncatednormal)

    student = Student(10.0, 10.0)
    distributionCollection.add(student)
    continuousDistributionCollection.add(student)

    triangular = Triangular(-1.0, 2.0, 4.0)
    distributionCollection.add(triangular)
    continuousDistributionCollection.add(triangular)

    uniform = Uniform(1.0, 2.0)
    distributionCollection.add(uniform)
    continuousDistributionCollection.add(uniform)

    weibull = Weibull(1.0, 1.0, 2.0)
    distributionCollection.add(weibull)
    continuousDistributionCollection.add(weibull)

    geometric = Geometric(0.5)
    distributionCollection.add(geometric)
    discreteDistributionCollection.add(geometric)

    poisson = Poisson(2.0)
    distributionCollection.add(poisson)
    discreteDistributionCollection.add(poisson)

    x = [[1.0], [2.0], [3.0]]
    p = [0.3, 0.2, 0.5]
    userdefined = UserDefined(x, p)
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

    # We create a collection of NumericalSample of size "size" and of
    # dimension 1 (scalar values) : the collection has distributionNumber
    # NumericalSamples

    sampleCollection = [NumericalSample(size, 1)
                        for i in range(distributionNumber)]
    # We create a collection of NumericalSample of size "size" and of
    # dimension 1 (scalar values) : the collection has
    # continuousDistributionNumber NumericalSamples
    continuousSampleCollection = [NumericalSample(size, 1)
                                  for i in range(continuousDistributionNumber)]
    # We create a collection of NumericalSample of size "size" and of
    # dimension 1 (scalar values) : the collection has
    # discreteDistributionNumber NumericalSamples
    discreteSampleCollection = [NumericalSample(size, 1)
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

    factoryCollection = DistributionFactoryCollection(3)
    factoryCollection[0] = UniformFactory()
    factoryCollection[1] = BetaFactory()
    factoryCollection[2] = NormalFactory()
    aSample = Uniform(-1.5, 2.5).getSample(size)
    model = FittingTest.BestModelBIC(aSample, factoryCollection)
    print("best model BIC=", repr(model))
    model, best_result = FittingTest.BestModelKolmogorov(
        aSample, factoryCollection)
    print("best model Kolmogorov=", repr(model))

    # BIC ranking
    resultBIC = SquareMatrix(distributionNumber)
    for i in range(distributionNumber):
        for j in range(distributionNumber):
            value = FittingTest.BIC(
                sampleCollection[i], distributionCollection[j], 0)
            resultBIC[i, j] = value
    print("resultBIC=", repr(resultBIC))

    # Kolmogorov ranking
    resultKolmogorov = SquareMatrix(continuousDistributionNumber)
    for i in range(continuousDistributionNumber):
        for j in range(continuousDistributionNumber):
            value = FittingTest.Kolmogorov(continuousSampleCollection[
                                           i], continuousDistributionCollection[j], 0.95, 0).getPValue()
            if (fabs(value) < 1.0e-6):
                value = 0.0
            resultKolmogorov[i, j] = value
    print("resultKolmogorov=", repr(resultKolmogorov))

    # ChiSquared ranking
    resultChiSquared = SquareMatrix(discreteDistributionNumber - 1)
    for i in range(discreteDistributionNumber - 1):
        for j in range(discreteDistributionNumber - 1):
            value = FittingTest.ChiSquared(discreteSampleCollection[
                                           i], discreteDistributionCollection[j], 0.95, 0).getPValue()
            if (fabs(value) < 1.0e-6):
                value = 0.0
            resultChiSquared[i, j] = value
    print("resultChiSquared=", repr(resultChiSquared))


except:
    import sys
    print("t_FittingTest_std.py", sys.exc_info()[0], sys.exc_info()[1])
