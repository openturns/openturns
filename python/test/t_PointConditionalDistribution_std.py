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
ott.assert_almost_equal(simplified.getMarginal(0), ot.Normal(1.8, 0.43589))

# special case for Student
student = ot.Student(2.5, [0.4] * 3, [1.2] * 3, R)
distribution = otexp.PointConditionalDistribution(student, [1], [2.0])
simplified = distribution.getSimplifiedVersion()
print(simplified)
ott.assert_almost_equal(simplified.getMarginal(0), ot.Student(3.5, 1.84, 1.07564))

# special case for JointDistribution
R = ot.CorrelationMatrix(3)
R[1, 0] = R[2, 0] = R[2, 1] = 0.9
copula = ot.NormalCopula(R)
joint = ot.JointDistribution([ot.Exponential(1.0, -2.0)] * R.getDimension(), copula)
distribution = otexp.PointConditionalDistribution(joint, [1], [0.8])
simplified = distribution.getSimplifiedVersion()
print(simplified)
assert simplified.getName() == "JointDistribution", "wrong type"

# special case for Mixture
mixture = ot.Mixture([ot.Normal(2), ot.Normal(2)], [0.3, 0.7])
distribution = otexp.PointConditionalDistribution(mixture, [1], [2.0])
simplified = distribution.getSimplifiedVersion()
print(simplified)
assert simplified.getName() == "Mixture", "wrong type"

# special case for KernelMixture
kernel = ot.Uniform()
sample = ot.Normal(2).getSample(5)
bandwidth = [1.0] * 2
kernelMixture = ot.KernelMixture(kernel, bandwidth, sample)
distribution = otexp.PointConditionalDistribution(kernelMixture, [1], [1.0])
simplified = distribution.getSimplifiedVersion()
print(simplified)
assert simplified.getName() == "Mixture", "wrong type"
