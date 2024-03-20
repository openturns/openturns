#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.Normal(4)
size = 10
experiment = ot.LHSExperiment(distribution, size)
print("experiment = ", experiment)
sample, weights = experiment.generateWithWeights()
print("sample = ", repr(sample))
print("weights = ", repr(weights))
print("sample2 = ", experiment.generate())
experiment = ot.LHSExperiment(distribution, size, True, True)
print("sample  = ", experiment.generate())
print("sample2 = ", experiment.generate())
experiment = ot.LHSExperiment(distribution, size, False, False)
print("sample  = ", experiment.generate())
print("sample2 = ", experiment.generate())
experiment = ot.LHSExperiment(distribution, size, True, False)
print("sample  = ", experiment.generate())
print("sample2 = ", experiment.generate())
experiment = ot.LHSExperiment(size)
print("sample  = ", experiment.generate())
print("sample2 = ", experiment.generate())

# variable size
experiment = ot.LHSExperiment(ot.Normal(4), 10)
for size in [215, 464]:
    experiment.setSize(size)
    X = experiment.generate()

# with correlation
dimension = 3
mu = ot.Point(dimension)
sigma = ot.CovarianceMatrix(dimension)
sigma[0, 2] = -0.5
distribution = ot.Normal(mu, sigma)
sampleSize = 5
experiment = ot.LHSExperiment(distribution, sampleSize)
sample = experiment.generate()
