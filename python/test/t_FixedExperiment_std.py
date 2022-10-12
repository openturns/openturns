#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

size = 5
dim = 6
refSample = ot.Sample(size, dim)
for i in range(size):
    p = ot.Point(dim)
    for j in range(dim):
        p[j] = i + j
    refSample[i] = p
print("ref. sample=", refSample)
experiment = ot.FixedExperiment(refSample)
print("experiment = ", experiment)
sample, weights = experiment.generateWithWeights()
print("sample = ", repr(sample))
print("weights = ", repr(weights))
