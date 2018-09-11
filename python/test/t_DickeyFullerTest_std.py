#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

size = 100

# ARMA parameters
arcoefficients = ot.ARMACoefficients([0.3])
macoefficients = ot.ARMACoefficients(0)
timeGrid = ot.RegularGrid(0.0, 0.1, size)

# White noise ==> gaussian
whiteNoise = ot.WhiteNoise(ot.Normal(), timeGrid)
myARMA = ot.ARMA(arcoefficients, macoefficients, whiteNoise)

# A realization of the ARMA process
# The realization is supposed to be of a stationnary process
realization = ot.TimeSeries(myARMA.getRealization())

# In the strategy of tests, one has to detect a trend tendency
# We check if the time series writes as x_t = a +b * t + c * x_{t-1}
# H0 = c is equal to one and thus
# p-value threshold : probability of the H0 reject zone : 0.05
# p-value : probability (test variable decision > test variable decision (statistic) evaluated on data)
# Test = True <=> p-value > p-value threshold
test = ot.DickeyFullerTest(realization)
print("Drift and linear trend model=",
      test.testUnitRootInDriftAndLinearTrendModel(0.05))
print("Drift model=", test.testUnitRootInDriftModel(0.05))
print("AR1 model=", test.testUnitRootInAR1Model(0.05))
