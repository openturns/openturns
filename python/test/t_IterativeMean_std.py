#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

ot.RandomGenerator.SetSeed(0)

# We create a Sample
point1 = ot.Point([10.,20.])
point2 = ot.Point([11.,21.])
point3 = ot.Point([12.,22.])
sample1 = ot.Sample([point1,point2,point3])
referencemean = sample1.computeMean()

# Iterative mean, one point at a time
print("Iterative mean, one point at a time")
dimension = 2
itermeanPoint = ot.IterativeMean(dimension)
itermeanPoint.increment(point1)
itermeanPoint.increment(point2)
itermeanPoint.increment(point3)
print("result=",itermeanPoint)
computedmean = itermeanPoint.getMean()
assert(referencemean == computedmean)
iteration = itermeanPoint.getIteration()
assert(iteration == 3)

# Iterative mean, one single sample
print("Iterative mean, one single sample")
itermeanSample = ot.IterativeMean(dimension)
itermeanSample.increment(sample1)
print("result=",itermeanSample)
computedmean = itermeanSample.getMean()
assert(referencemean == computedmean)
iteration = itermeanSample.getIteration()
assert(iteration == 3)

# Iterative mean, one single sample, then one point at a time
print("Iterative mean, one single sample, then one point at a time")
itermeanMixed = ot.IterativeMean(dimension)
itermeanMixed.increment(sample1)
itermeanMixed.increment(point1)
itermeanMixed.increment(point2)
itermeanMixed.increment(point3)
print("result=",itermeanMixed)
computedmean = itermeanMixed.getMean()
assert(referencemean == computedmean)
iteration = itermeanMixed.getIteration()
assert(iteration == 6)
