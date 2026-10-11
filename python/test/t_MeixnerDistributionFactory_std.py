#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

ot.ResourceMap.SetAsUnsignedInteger("MeixnerDistribution-CDFDiscretization", 100)
ot.ResourceMap.SetAsScalar("MeixnerDistribution-MaximumAbsoluteError", 1.0e-6)
ot.ResourceMap.SetAsScalar("MeixnerDistribution-MaximumRelativeError", 1.0e-6)
ot.ResourceMap.SetAsScalar("MeixnerDistribution-MaximumConstraintError", 1.0e-6)
ot.ResourceMap.SetAsScalar("MeixnerDistribution-MaximumObjectiveError", 1.0e-6)
distribution = ot.MeixnerDistribution(1.5, 0.5, 2.5, -0.5)
size = 1000
sample = distribution.getSample(size)
factory = ot.MeixnerDistributionFactory()
covariance = ot.CovarianceMatrix()
# Distribution estimatedDistribution(factory.build(sample, covariance))
estimatedDistribution = factory.build(sample)
print("Distribution          =", distribution)
print("Estimated distribution=", estimatedDistribution)
# print "Covariance=", covariance
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedMeixnerDistribution = factory.buildAsMeixnerDistribution(sample)
print("MeixnerDistribution          =", distribution)
print("Estimated meixnerDistribution=", estimatedMeixnerDistribution)
estimatedMeixnerDistribution = factory.buildAsMeixnerDistribution()
print("Default meixnerDistribution=", estimatedMeixnerDistribution)
estimatedMeixnerDistribution = factory.buildAsMeixnerDistribution(
    distribution.getParameter()
)
print("MeixnerDistribution from parameters=", estimatedMeixnerDistribution)

# Moment matching preserves the sample variance and mean by construction
momentEstimate = factory.buildAsMeixnerDistribution(sample)
ott.assert_almost_equal(
    momentEstimate.getCovariance()[0, 0], sample.computeVariance()[0], 1e-9, 0.0
)
ott.assert_almost_equal(
    momentEstimate.getMean()[0], sample.computeMean()[0], 1e-9, 0.0
)
