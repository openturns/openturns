#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


distributions = [ot.Student(3.5, 2.5, 1.0)]
R = ot.CorrelationMatrix([[1.0, 0.5], [0.5, 1.0]])
distributions.append(ot.Student(4.5, [2.5] * 2, [1.5] * 2, R))
R = ot.CorrelationMatrix(10)
mu = ot.Point(10)
sigma = ot.Point(10)
for i in range(10):
    mu[i] = 0.5 * i
    sigma[i] = 0.5 + i
    for j in range(i):
        R[i, j] = 1.0 / (i + j + 2.0) ** 2.0
distributions.append(ot.Student(4.5, mu, sigma, R))
size = 10000
factory = ot.StudentFactory()
for distribution in distributions:
    sample = distribution.getSample(size)
    estimatedDistribution = factory.build(sample)
    print("distribution=", distribution)
    print("Estimated distribution=", estimatedDistribution)
    ott.assert_almost_equal(estimatedDistribution.getParameter(), distribution.getParameter(), 0.2, 0.1)
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
ott.assert_almost_equal(estimatedDistribution.getParameter(), [3.0, 0.0, 1.0])
estimatedStudent = factory.buildAsStudent(sample)
assert isinstance(estimatedStudent, ot.Student), "wrong type"
print("Student          =", distribution)
print("Estimated student=", estimatedStudent)
estimatedStudent = factory.buildAsStudent()
print("Default student=", estimatedStudent)
ott.assert_almost_equal(estimatedStudent.getParameter(), [3.0, 0.0, 1.0])

# nu < 2
distribution = ot.Student(0.5, 0.0, 1.0)
sample = distribution.getSample(5000)
estimated = factory.buildAsStudent(sample)
estimatedParams = estimated.getParameter()
print("nu<2 estimated=", estimatedParams)
nu = estimatedParams[0]
ott.assert_almost_equal(nu, 0.5, 0.0, 0.2)

# zero-IQR regression: component values [0,0,0,0,0,1] have q75==q25
# low-dimensional path (d <= 5) must not pass zero sigma to TNC/Student
sample_zero_iqr_low = ot.Sample([[0.0], [0.0], [0.0], [0.0], [0.0], [1.0]])
estimated_low = factory.buildAsStudent(sample_zero_iqr_low)
print("zero-IQR low-dim estimated=", estimated_low.getParameter())
assert estimated_low.getSigma()[0] >= ot.SpecFunc.ScalarEpsilon
assert estimated_low.getSigma()[0] > 0.0
# high-dimensional path (d > 5) must not create degenerate sigma
ot.RandomGenerator.SetSeed(0)
sample_zero_iqr_high = ot.Normal(6).getSample(100)
for i in range(80):
    sample_zero_iqr_high[i, 0] = 0.0
for i in range(80, 100):
    sample_zero_iqr_high[i, 0] = 1.0
estimated_high = factory.buildAsStudent(sample_zero_iqr_high)
print("zero-IQR high-dim estimated=", estimated_high.getParameter())
for s in estimated_high.getSigma():
    assert s >= ot.SpecFunc.ScalarEpsilon
    assert s > 0.0
