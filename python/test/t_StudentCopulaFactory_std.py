#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()
if not ot.PlatformInfo.HasFeature("boost"):
    exit(0)

dim = 2
nu = 4.5
R = ot.CorrelationMatrix(dim, [1.0, 0.5, 0.5, 1.0])
distribution = otexp.StudentCopula(nu, R)
size = 100000
sample = distribution.getSample(size)

factory = otexp.StudentCopulaFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
ott.assert_almost_equal(estimatedDistribution.getParameter()[0], nu, 1e-1, 1e-1)
ott.assert_almost_equal(estimatedDistribution.getParameter()[-1], 0.5, 1e-1, 1e-1)
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedStudentCopula = factory.buildAsStudentCopula(sample)
print("StudentCopula          =", distribution)
print("Estimated copula=", estimatedStudentCopula)
estimatedStudentCopula = factory.buildAsStudentCopula()
print("Default copula=", estimatedStudentCopula)
estimatedStudentCopula = factory.buildAsStudentCopula(distribution.getParameter())
print("StudentCopula from parameters=", estimatedStudentCopula)
