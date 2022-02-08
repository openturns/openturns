#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

source = ot.RandomVector(ot.Normal())
initialState = [0.0]
sampler = ot.RandomVectorMetropolisHastings(source, initialState)
sampler.setBurnIn(20)
sampler.setThinning(2)
x = sampler.getSample(10000)
mean = x.computeMean()
stddev = x.computeStandardDeviation()
print(mean, stddev)
#ott.assert_almost_equal(mean, [0.824453, -0.0215115])
#ott.assert_almost_equal(stddev, [0.00197192, 0.968657])

# with link function
slf = ot.SymbolicFunction(['x'], ['0.0', '0.1'])
sampler = ot.RandomVectorMetropolisHastings(source, initialState, [0], slf)
x = sampler.getSample(10000)
mean = x.computeMean()
stddev = x.computeStandardDeviation()
print(mean, stddev)
ott.assert_almost_equal(mean, [0.000629119])
ott.assert_almost_equal(stddev, [0.0991822])
