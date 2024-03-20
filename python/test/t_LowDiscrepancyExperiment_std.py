#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


distribution = ot.Normal(4)
distribution.setMu([5.0] * 4)
size = 16
experiment = ot.LowDiscrepancyExperiment(ot.HaltonSequence(), distribution, size)
print("experiment = ", experiment)
# Test sampling with weights
sample, weights = experiment.generateWithWeights()
print("sample  = ", sample)
print("weights = ", weights)
# Test sampling with reinitialization each time the distribution is
# set (default behaviour)
# sample 2 != sample
print("sample2 = ", experiment.generate())
experiment.setDistribution(distribution)
# sample 3 == sample
print("sample3 = ", experiment.generate())

# Test sampling without reinitialization excepted when distribution
# dimension changes
experiment = ot.LowDiscrepancyExperiment(ot.HaltonSequence(), distribution, size, False)
print("sample  = ", experiment.generate())
# sample 2 != sample
print("sample2 = ", experiment.generate())
experiment.setDistribution(distribution)
# sample 3 != sample && sample 3 != sample 2
print("sample3 = ", experiment.generate())
# Test dimension change
experiment.setDistribution(ot.Normal())
print("sample = ", experiment.generate())

# Test constructor with no distribution and dimension>1
experiment = ot.LowDiscrepancyExperiment(ot.HaltonSequence(2), size)
print("sample = ", experiment.generate())
# Test with dependent marginals
R = ot.CorrelationMatrix(4)
for i in range(1, 4):
    R[i - 1, i] = 0.5
distribution.setR(R)
experiment = ot.LowDiscrepancyExperiment(ot.HaltonSequence(), distribution, size, False)
print("sample = ", experiment.generate())
