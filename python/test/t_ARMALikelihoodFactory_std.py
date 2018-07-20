#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

# ARMA(p, q)
p = 2
q = 1
dim = 2

# Make a realization of an ARMA model
# Tmin , Tmax and N points for TimeGrid
dt = 1.0
size = 400
timeGrid = ot.RegularGrid(0.0, dt, size)

# white noise
cov = ot.CovarianceMatrix([[0.1, 0.0], [0.0, 0.2]])
whiteNoise = ot.WhiteNoise(ot.Normal([0.0] * dim, cov), timeGrid)

# AR/MA coefficients
ar = ot.ARMACoefficients(p, dim)
ar[0] = ot.SquareMatrix([[-0.5, -0.1], [-0.4, -0.5]])
ar[1] = ot.SquareMatrix([[0.0, 0.0], [-0.25, 0.0]])

ma = ot.ARMACoefficients(q, dim)
ma[0] = ot.SquareMatrix([[-0.4, 0.0], [0.0, -0.4]])

# ARMA model creation
myARMA = ot.ARMA(ar, ma, whiteNoise)

# Create a realization
timeSeries = ot.TimeSeries(myARMA.getRealization())

cov[0, 0] += 0.01 * ot.DistFunc.rNormal()
cov[1, 1] += 0.01 * ot.DistFunc.rNormal()

alpha = ot.SquareMatrix(dim)

for k in range(p):
    for j in range(dim):
        for i in range(dim):
            alpha[i, j] = 0.01 * ot.DistFunc.rNormal()
    ar[k] = ar[k] + alpha

for k in range(q):
    for j in range(dim):
        for i in range(dim):
            alpha[i, j] = 0.01 * ot.DistFunc.rNormal()
    ma[k] = ma[k] + alpha

factory = ot.ARMALikelihoodFactory(p, q, dim)
print('factory=', factory)
factory.setInitialConditions(ar, ma, cov)

result = ot.ARMA(factory.build(timeSeries))
print('original process = ', myARMA)
#print('Estimated ARMA= ', result)
