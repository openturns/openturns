#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.PlatformInfo.SetNumericalPrecision(3)

size = 10000

distribution = ot.Gumbel(1.5, -0.5)
print('distribution=', distribution)
sample = distribution.getSample(size)
factory = ot.MethodOfMomentsFactory(ot.Gumbel())
inf_distribution = factory.build(sample)
print('estimated distribution=', inf_distribution)

# set (a,b) out of (r, t, a, b)
distribution = ot.Beta(2.3, 2.2, -1.0, 1.0)
print('distribution=', distribution)
sample = distribution.getSample(size)
factory = ot.MethodOfMomentsFactory(ot.Beta())
factory.setKnownParameter([-1.0, 1.0], [2, 3])
inf_distribution = factory.build(sample)
print('estimated distribution=', inf_distribution)

# with bounds
data = [0.6852, 0.9349, 0.5884, 1.727, 1.581,
        0.3193, -0.5701, 1.623, 2.210, -0.3440, -0.1646]
sample = ot.Sample([[x] for x in data])
size = sample.getSize()
xMin = sample.getMin()[0]
xMax = sample.getMax()[0]
delta = xMax - xMin
a = xMin - delta / (size + 2)
b = xMax + delta / (size + 2)
distribution = ot.TruncatedNormal()
factory = ot.MethodOfMomentsFactory(distribution)
factory.setKnownParameter([a, b], [2, 3])
solver = factory.getOptimizationAlgorithm()
sampleMean = sample.computeMean()[0]
sampleSigma = sample.computeStandardDeviationPerComponent()[0]
startingPoint = [sampleMean, sampleSigma]
solver.setStartingPoint(startingPoint)
factory.setOptimizationAlgorithm(solver)
lowerBound = [-1.0, 0]
upperBound = [-1.0, 1.5]
finiteLowerBound = [False, True]
finiteUpperBound = [False, True]
bounds = ot.Interval(lowerBound, upperBound,
                     finiteLowerBound, finiteUpperBound)
factory.setOptimizationBounds(bounds)
inf_distribution = factory.build(sample)
print('estimated distribution=', inf_distribution)
