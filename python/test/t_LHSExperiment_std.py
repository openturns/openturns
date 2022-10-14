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
