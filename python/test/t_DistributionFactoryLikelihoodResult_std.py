#! /usr/bin/env python

import os

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Create a distribution factory likelihood result
distribution = ot.GeneralizedExtremeValue(2.0, 1.5, -0.15)
parameterDist = ot.Normal([2.0, 1.5, -0.15], [0.1, 0.1, 0.05])
logLikelihood = -123.456

result = ot.DistributionFactoryLikelihoodResult(distribution, parameterDist, logLikelihood)
print("result=", result)
print("repr=", repr(result))

# Test getDistribution
estimated = result.getDistribution()
ott.assert_almost_equal(estimated.getParameter(), distribution.getParameter())

# Test getParameterDistribution
paramDist = result.getParameterDistribution()
assert paramDist.getDimension() == 3, "wrong dimension"
ott.assert_almost_equal(paramDist.getMean(), [2.0, 1.5, -0.15])

# Test getLogLikelihood / setLogLikelihood
ott.assert_almost_equal(result.getLogLikelihood(), logLikelihood)
result.setLogLikelihood(-999.0)
ott.assert_almost_equal(result.getLogLikelihood(), -999.0)

# Test default constructor
result2 = ot.DistributionFactoryLikelihoodResult()
print("default result=", result2)

# Test clone via copy constructor
result3 = ot.DistributionFactoryLikelihoodResult(result)
ott.assert_almost_equal(result3.getLogLikelihood(), -999.0)

# Test save/load
study = ot.Study("result.xml")
study.add("result", result)
study.save()
study2 = ot.Study("result.xml")
study2.load()
result_loaded = ot.DistributionFactoryLikelihoodResult()
study2.fillObject("result", result_loaded)
print("loaded result=", result_loaded)
ott.assert_almost_equal(result_loaded.getLogLikelihood(), -999.0)
ott.assert_almost_equal(
    result_loaded.getDistribution().getParameter(), distribution.getParameter()
)
os.remove("result.xml")
