#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Test default constructor
print("experiment0=", repr(ot.GaussProductExperiment().generate()))

distribution = ot.ComposedDistribution(
    [ot.Exponential(), ot.Triangular(-1.0, -0.5, 1.0)])
marginalSizes = [3, 6]
# Test the constructor based on marginal degrees
print("experiment1=", ot.GaussProductExperiment(marginalSizes))
# Test the constructor based on distribution
print("experiment2=", ot.GaussProductExperiment(distribution))
# Test the constructor based on marginal degrees and distribution
experiment = ot.GaussProductExperiment(distribution, marginalSizes)
print("experiment = ", experiment)
sample, weights = experiment.generateWithWeights()
print("sample = ", repr(sample))
print("weights = ", repr(weights))
