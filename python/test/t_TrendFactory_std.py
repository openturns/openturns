#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    inVar = ['t']
    functions = []
    functions.append(SymbolicFunction(inVar, ['1']))
    functions.append(SymbolicFunction(inVar, ['cos(2 * t)']))
    functions.append(SymbolicFunction(inVar, ['sin(2 * t)']))

    # We build the weights
    coefficients = Sample(0, 2)
    p = Point(2)
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
    myFunction = DualLinearCombinationFunction(functions, coefficients)

    # Fourth : we build a time series for estimation
    # it issued from a white noise
    dimension = 2

    # Fix the realization as a Normal
    noiseDistribution = Normal(dimension)

    # TimeGrid parameters
    N = 1000
    timeStart = 0.
    timeStep = 0.1
    timeGrid = RegularGrid(timeStart, timeStep, N)

    # White noise
    myWhiteNoise = WhiteNoise(noiseDistribution, timeGrid)

    realization = TimeSeries(myWhiteNoise.getRealization())
    print("White noise realization = ", realization)

    # We make a trend transform to the time series
    # We get a time series which contains values of time
    myTransformFunction = TrendTransform(myFunction, timeGrid)
    myTimeSeries = TimeSeries(
        timeGrid, myTransformFunction(realization.getValues()))
    print("myTimeSeries = ", myTimeSeries)

    # We wants to get the coefficients using a factory
    # Build a factory using default constructor
    myDefaultFactory = TrendFactory()
    print("myDefaultFactory = ", myDefaultFactory)

    myEstimateTrend = myDefaultFactory.build(myTimeSeries, Basis(functions))
    print("myEstimateTrend = ", myEstimateTrend)

    # We fix a new fitting algorithm
    myDefaultFactory.setFittingAlgorithm(KFold())
    print("myDefaultFactory = ", myDefaultFactory)
    myNewEstimateTrend = myDefaultFactory.build(myTimeSeries, Basis(functions))
    print("myNewEstimateTrend = ", myNewEstimateTrend)

except:
    import sys
    print("t_TrendTransform_std.py", sys.exc_info()[0], sys.exc_info()[1])
