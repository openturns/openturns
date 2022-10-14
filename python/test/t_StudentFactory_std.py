#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(3)


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
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedStudent = factory.buildAsStudent(sample)
print("Student          =", distribution)
print("Estimated student=", estimatedStudent)
estimatedStudent = factory.buildAsStudent()
print("Default student=", estimatedStudent)
