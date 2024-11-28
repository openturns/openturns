#!/usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

R = ot.CorrelationMatrix(3)
R[1, 0] = R[2, 0] = R[2, 1] = 0.9
copula = ot.NormalCopula(R)
distribution = otexp.PointConditionalDistribution(copula, [1], [0.8])

print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Get mean and covariance
print("Mean= ", repr(distribution.getMean()))
print("Covariance= ", repr(distribution.getCovariance()))

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", repr(oneRealization))

print("dim=", distribution.getDimension())
print("range=", distribution.getRange())
print("mean=", distribution.getMean())
print("cov=", distribution.getCovariance())
print("marginal=", distribution.getMarginal([1]))

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipMoments()  # slow
validation.skipCorrelation()  # slow
validation.skipGradient()  # slow
validation.setCDFSamplingSize(1)
validation.run()

# no conditioning
distribution = otexp.PointConditionalDistribution(copula, [], [])
simplified = distribution.getSimplifiedVersion()
print(distribution.getSimplifiedVersion())
ott.assert_almost_equal(simplified, copula)

# discrete case
discrete = ot.Multinomial(5, [0.25] * 3)
distribution = otexp.PointConditionalDistribution(discrete, [1], [1.0])
print("dim=", distribution.getDimension())
assert distribution.getDimension() == 2, "wrong dimension"
print("range=", distribution.getRange())
print("mean=", distribution.getMean())
print("cov=", distribution.getCovariance())
print("marginal=", distribution.getMarginal([1]))
print("support=", distribution.getSupport())
assert distribution.isDiscrete(), "wrong isDiscrete"
ott.assert_almost_equal(distribution.getMean(), [4 / 3] * 2)
validation = ott.DistributionValidation(distribution)
validation.skipMoments()  # slow
validation.skipCorrelation()  # slow
validation.skipGradient()  # slow
validation.skipParameters()  # integer parameter
validation.setCDFSamplingSize(1)
validation.run()

# special case for Normal
normal = ot.Normal([0.0] * 3, R)
distribution = otexp.PointConditionalDistribution(normal, [1], [2.0])
simplified = distribution.getSimplifiedVersion()
print(simplified)
simplified_ref = ot.Normal([1.8] * 2, [0.43589] * 2, ot.CorrelationMatrix([[1.0, 0.473684], [0.473684, 1.0]]))
ott.assert_almost_equal(simplified, simplified_ref)

# ensure services are ok
mean_ref = simplified_ref.getMean()
ott.assert_almost_equal(distribution.getMean(), mean_ref)
ott.assert_almost_equal(distribution.getStandardDeviation(), simplified_ref.getStandardDeviation())
ott.assert_almost_equal(distribution.computePDF(mean_ref), simplified_ref.computePDF(mean_ref))
ott.assert_almost_equal(distribution.computeCDF(mean_ref), simplified_ref.computeCDF(mean_ref))

# special case for Student
student = ot.Student(2.5, [0.4] * 3, [1.2] * 3, R)
distribution = otexp.PointConditionalDistribution(student, [1], [2.0])
simplified = distribution.getSimplifiedVersion()
print(simplified)
ott.assert_almost_equal(simplified.getMarginal(0), ot.Student(3.5, 1.84, 1.07564))
ott.assert_almost_equal(distribution.computePDF(distribution.getMean()), 0.156194)

# special case for Mixture
mixture = ot.Mixture([ot.Normal(2), ot.Normal(2)], [0.3, 0.7])
distribution = otexp.PointConditionalDistribution(mixture, [1], [2.0])
simplified = distribution.getSimplifiedVersion()
print(simplified)
assert simplified.getName() == "Mixture", "wrong type"
ott.assert_almost_equal(distribution.computePDF(distribution.getMean()), 0.398942)

# special case for KernelMixture
kernel = ot.Uniform()
sample = ot.Normal(2).getSample(5)
bandwidth = [1.0] * 2
kernelMixture = ot.KernelMixture(kernel, bandwidth, sample)
distribution = otexp.PointConditionalDistribution(kernelMixture, [1], [1.0])
simplified = distribution.getSimplifiedVersion()
print(simplified)
assert simplified.getName() == "Mixture", "wrong type"
ott.assert_almost_equal(distribution.computePDF([0]), 0.25)

# ratio of uniform method initialization is harder for Dirichlet
R = ot.CorrelationMatrix(3, [1.0, 0.5, 0.1, 0.5, 1.0, 0.2, 0.1, 0.2, 1.0])
core = ot.Dirichlet([1, 2, 3, 4])
joint = ot.JointDistribution([ot.Exponential()] * 3, core)
distribution = otexp.PointConditionalDistribution(joint, [1], [2.0])
sample = distribution.getSample(10)

# special case for EmpiricalBernsteinCopula
sample = normal.getSample(10)
bernstein = ot.EmpiricalBernsteinCopula(sample, 5)
distribution = otexp.PointConditionalDistribution(bernstein, [1], [0.5])
simplified = distribution.getSimplifiedVersion()
print(simplified)
assert simplified.getName() == "Mixture", "wrong type"

# special case for BlockIndependentDistribution
blockIndep = ot.BlockIndependentDistribution([normal, student, mixture])
# test a conditioning which remove the first block, modify the second block and let the last block unchanged
distribution = otexp.PointConditionalDistribution(blockIndep, [1, 3, 0, 2], [0.5, 0.5, 0.5, 0.5])
simplified = distribution.getSimplifiedVersion()
print(simplified)
assert simplified.getName() == "BlockIndependentDistribution", "wrong type"
ott.assert_almost_equal(distribution.computePDF(distribution.getMean()), 0.0248766)
# test a conditioning which remove all but the last block
# As the last block can be simplified, it is its actual simplification which is used
distribution = otexp.PointConditionalDistribution(blockIndep, [0, 1, 2, 3, 4, 5, 6], [0.5] * 7)
simplified = distribution.getSimplifiedVersion()
print(simplified)
assert simplified.getName() == "Mixture", "wrong type"
ott.assert_almost_equal(distribution.computePDF(distribution.getMean()), 0.398942)

# special case for BlockIndependentCopula
R = ot.CorrelationMatrix(3)
R[1, 0] = R[2, 0] = R[2, 1] = 0.9
blockIndep = ot.BlockIndependentCopula([ot.GumbelCopula(), ot.NormalCopula(R)])
# test a conditioning which remove the first block, modify the second block and let the last block unchanged
distribution = otexp.PointConditionalDistribution(blockIndep, [1, 3], [0.4, 0.5])
simplified = distribution.getSimplifiedVersion()
print(simplified)
assert simplified.getName() == "BlockIndependentDistribution", "wrong type"
ott.assert_almost_equal(distribution.computePDF(distribution.getMean()), 9.01202)

# negative bound sampling case
ot.ResourceMap.SetAsBool("PointConditionalDistribution-UseSimplifiedVersion", False)
distribution = otexp.PointConditionalDistribution(copula - 1.0, [1], [-0.8])
ott.assert_almost_equal(distribution.getSample(1000).computeMean(), [-0.75] * 2, 1e-2, 1e-2)
