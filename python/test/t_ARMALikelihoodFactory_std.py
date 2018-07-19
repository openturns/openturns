#! /usr/bin/env python

from __future__ import print_function
from openturns import *


# ARMA(p, q)
p = 2
q = 1
dimension = 2

# ARMACoefficients initializing
# Overwide bug 471
ResourceMap.SetAsScalar('BoxCox-RootEpsilon', 1.0e-6)

# Make a realization of an ARMA model
# Tmin , Tmax and N points for TimeGrid
dt = 1.0
size = 400
timeGrid = RegularGrid(0.0, dt, size)

# Fixing the distributions for the WhiteNoise
sigma = 0.1
cov = CovarianceMatrix(dimension)
cov[0, 0] = sigma
cov[1, 1] = 2.0 * sigma
whiteNoiseDistribution = Normal(Point(dimension, 0.0), cov)

# Building a process from a White Noise
whiteNoise = WhiteNoise(whiteNoiseDistribution)
whiteNoise.setTimeGrid(timeGrid)

arCoefficients = SquareMatrixCollection(p)
maCoefficients = SquareMatrixCollection(q)

alpha = SquareMatrix(dimension)
alpha[0, 0] = -0.5
alpha[0, 1] = -0.1
alpha[1, 0] = -0.4
alpha[1, 1] = -0.5

arCoefficients[0] = alpha

alpha[0, 0] = 0.0
alpha[0, 1] = 0.0
alpha[1, 0] = -0.25
alpha[1, 1] = 0.0

arCoefficients[1] = alpha

alpha[0, 0] = -0.4
alpha[0, 1] = 0.0
alpha[1, 0] = 0.0
alpha[1, 1] = -0.4

maCoefficients[0] = alpha

phi = ARMACoefficients(arCoefficients)
theta = ARMACoefficients(maCoefficients)

# ARMA model creation
myARMA = ARMA(phi, theta, whiteNoise)

# Create a realization
timeSeries = TimeSeries(myARMA.getRealization())

cov[0, 0] += 0.01 * DistFunc.rNormal()
cov[1, 1] += 0.01 * DistFunc.rNormal()
for k in range(p):
    for j in range(dimension):
        for i in range(dimension):
            alpha[i, j] = 0.01 * DistFunc.rNormal()
    phi[k] = phi[k] + alpha

#
for k in range(q):
    for j in range(dimension):
        for i in range(dimension):
            alpha[i, j] = 0.01 * DistFunc.rNormal()
    theta[k] = theta[k] + alpha

factory = ARMALikelihoodFactory(p, q, dimension)
print('factory=', factory)
factory.setInitialConditions(phi, theta, cov)

result = ARMA(factory.build(timeSeries))
print('original process = ', myARMA)
# print('Estimated ARMA= ', result)
