#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Instantiate one distribution object
meanPoint = ot.Point(1)
meanPoint[0] = 1.0
sigma = ot.Point(1)
sigma[0] = 1.0
R = ot.CorrelationMatrix(1)
distribution = ot.Normal(meanPoint, sigma, R)
print("Distribution ", repr(distribution))

# We try to set an erroneous covariance matrix (wrong dimension) into
# distribution
try:
    newR = ot.CorrelationMatrix(2)
    distribution.setCorrelation(newR)
    raise
except:
    pass
