#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.Normal(4)
size = 10
weightingDistribution = ot.Normal(distribution)
weightingDistribution.setMean(ot.Point(4, 1.0))
experiment = ot.ImportanceSamplingExperiment(distribution, weightingDistribution, size)
print("experiment = ", experiment)
sample, weights = experiment.generateWithWeights()
print("sample = ", repr(sample))
print("weights = ", repr(weights))
