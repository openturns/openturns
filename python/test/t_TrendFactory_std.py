#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


inVar = ['t']
functions = []
functions.append(ot.SymbolicFunction(inVar, ['1']))
functions.append(ot.SymbolicFunction(inVar, ['cos(2 * t)']))
functions.append(ot.SymbolicFunction(inVar, ['sin(2 * t)']))

# We build the weights
coefficients = ot.Sample(0, 2)
p = ot.Point(2)
p[0] = 1.5
p[1] = 2.5
coefficients.add(p)
p[0] = -0.5
p[1] = 0.5
coefficients.add(p)
p[0] = 1.
p[1] = 1.
coefficients.add(p)

# Third, build the function
myFunction = ot.DualLinearCombinationFunction(functions, coefficients)

# Fourth : we build a time series for estimation
# it issued from a white noise
dimension = 2

# Fix the realization as a Normal
noiseDistribution = ot.Normal(dimension)

# TimeGrid parameters
N = 1000
timeStart = 0.
timeStep = 0.1
timeGrid = ot.RegularGrid(timeStart, timeStep, N)

# White noise
myWhiteNoise = ot.WhiteNoise(noiseDistribution, timeGrid)

realization = ot.TimeSeries(myWhiteNoise.getRealization())
print("White noise realization = ", realization)

# We make a trend transform to the time series
# We get a time series which contains values of time
myTransformFunction = ot.TrendTransform(myFunction, timeGrid)
myTimeSeries = ot.TimeSeries(
    timeGrid, myTransformFunction(realization.getValues()))
print("myTimeSeries = ", myTimeSeries)

# We wants to get the coefficients using a factory
# Build a factory using default constructor
myDefaultFactory = ot.TrendFactory()
print("myDefaultFactory = ", myDefaultFactory)

myEstimateTrend = myDefaultFactory.build(myTimeSeries, ot.Basis(functions))
print("myEstimateTrend = ", myEstimateTrend)

# We fix a new fitting algorithm
myDefaultFactory.setFittingAlgorithm(ot.KFold())
print("myDefaultFactory = ", myDefaultFactory)
myNewEstimateTrend = myDefaultFactory.build(myTimeSeries, ot.Basis(functions))
print("myNewEstimateTrend = ", myNewEstimateTrend)
