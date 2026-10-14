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

# Restart behavior, see issue #2740: setting a size restarts the
# sequence when restart is set, successive generations continue it
restartTrue = ot.LowDiscrepancyExperiment(
    ot.SobolSequence(), ot.JointDistribution([ot.Uniform()] * 2), 4, True
)
restartTrue.setSize(4)
firstBatch = restartTrue.generate()
restartTrue.setSize(4)
assert restartTrue.generate() == firstBatch
assert restartTrue.generate() != firstBatch
restartFalse = ot.LowDiscrepancyExperiment(
    ot.SobolSequence(), ot.JointDistribution([ot.Uniform()] * 2), 4, False
)
restartFalse.setSize(4)
assert restartFalse.generate() != restartFalse.generate()
