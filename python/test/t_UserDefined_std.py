#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

# Instanciate one distribution object
x = [[1.0], [2.0], [3.0], [3.0]]
p = [0.3, 0.1, 0.6, 0.6]
distribution = ot.UserDefined(x, p)
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", repr(oneRealization))

# Test for sampling
size = 10
oneSample = distribution.getSample(size)
print("oneSample=Ok", repr(oneSample))

# Define a point
point = ot.Point(distribution.getDimension(), 2.0)

# Show PDF and CDF of a point
pointPDF = distribution.computePDF(point)
pointCDF = distribution.computeCDF(point)
print("point= ", repr(point), " pdf= %.12g" % pointPDF, " cdf=", pointCDF)

# Get 95% quantile
quantile = distribution.computeQuantile(0.95)
print("Quantile=", repr(quantile))
print("entropy=%.6f" % distribution.computeEntropy())

for i in range(6):
    print("standard moment n=", i, " value=",
          distribution.getStandardMoment(i))
print("Standard representative=", distribution.getStandardRepresentative())
print("parameter=", distribution.getParameter())
print("parameterDescription=", distribution.getParameterDescription())
parameter = distribution.getParameter()
parameter[-1] = 0.3
distribution.setParameter(parameter)
print("parameter=", distribution.getParameter())

# To prevent automatic compaction
ot.ResourceMap.SetAsUnsignedInteger("UserDefined-SmallSize", 5)
sample = ot.Sample(40, 3)
for i in range(4):
    for j in range(3):
        sample[i, j] = 10 * (i // 3 + 1) + 0.1 * (j + 1)

multivariateUserDefined = ot.UserDefined(sample)
print("Multivariate UserDefined=", multivariateUserDefined)
print("Marginal 0=", multivariateUserDefined.getMarginal(0))
indices = ot.Indices(2)
indices[0] = 2
indices[1] = 0
print("Marginal (2, 0)=", multivariateUserDefined.getMarginal(indices))
