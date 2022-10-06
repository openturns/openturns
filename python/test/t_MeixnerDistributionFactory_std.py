#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

ot.ResourceMap.SetAsUnsignedInteger(
    "MeixnerDistribution-CDFIntegrationNodesNumber", 8)
ot.ResourceMap.SetAsUnsignedInteger(
    "MeixnerDistribution-CDFDiscretization", 100)
ot.ResourceMap.SetAsScalar(
    "MeixnerDistribution-MaximumAbsoluteError", 1.0e-6)
ot.ResourceMap.SetAsScalar(
    "MeixnerDistribution-MaximumRelativeError", 1.0e-6)
ot.ResourceMap.SetAsScalar(
    "MeixnerDistribution-MaximumConstraintError", 1.0e-6)
ot.ResourceMap.SetAsScalar(
    "MeixnerDistribution-MaximumObjectiveError", 1.0e-6)
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
estimatedDistribution = factory.build(
    distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedMeixnerDistribution = factory.buildAsMeixnerDistribution(sample)
print("MeixnerDistribution          =", distribution)
print("Estimated meixnerDistribution=", estimatedMeixnerDistribution)
estimatedMeixnerDistribution = factory.buildAsMeixnerDistribution()
print("Default meixnerDistribution=", estimatedMeixnerDistribution)
estimatedMeixnerDistribution = factory.buildAsMeixnerDistribution(
    distribution.getParameter())
print("MeixnerDistribution from parameters=", estimatedMeixnerDistribution)

