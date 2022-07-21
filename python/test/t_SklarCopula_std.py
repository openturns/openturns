#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Instantiate one distribution object
dim = 3
R = ot.CorrelationMatrix(dim)
for i in range(dim - 1):
    R[i, i + 1] = 0.25
copula = ot.SklarCopula(
    ot.Distribution(ot.Normal([1.0, 2.0, 3.0], [2.0, 3.0, 1.0], R)))
copulaRef = ot.NormalCopula(R)
print("Copula ", repr(copula))
print("Copula ", copula)
print("Mean      =", repr(copula.getMean()))
print("Mean (ref)=", repr(copulaRef.getMean()))
ot.ResourceMap.SetAsUnsignedInteger("GaussKronrod-MaximumSubIntervals", 20)
ot.ResourceMap.SetAsScalar("GaussKronrod-MaximumError",  1.0e-4)
print("Covariance      =", repr(copula.getCovariance()))
ot.ResourceMap.SetAsUnsignedInteger("GaussKronrod-MaximumSubIntervals", 100)
ot.ResourceMap.SetAsScalar("GaussKronrod-MaximumError",  1.0e-12)
print("Covariance (ref)=", repr(copulaRef.getCovariance()))

# Is this copula an elliptical distribution?
print("Elliptical distribution= ", copula.isElliptical())

# Is this copula elliptical ?
print("Elliptical copula= ", copula.hasEllipticalCopula())

# Is this copula independent ?
print("Independent copula= ", copula.hasIndependentCopula())

# Test for realization of distribution
oneRealization = copula.getRealization()
print("oneRealization=", repr(oneRealization))

# Test for sampling
size = 10
oneSample = copula.getSample(size)
print("oneSample=", repr(oneSample))

# Test for sampling
size = 1000
anotherSample = copula.getSample(size)
print("anotherSample mean=", repr(anotherSample.computeMean()))
print("anotherSample covariance=", repr(anotherSample.computeCovariance()))

# Define a point
point = ot.Point(dim, 0.2)

# Show PDF and CDF of point
pointPDF = copula.computePDF(point)
pointCDF = copula.computeCDF(point)
pointPDFRef = copulaRef.computePDF(point)
pointCDFRef = copulaRef.computeCDF(point)
print("Point = ", repr(point), " pdf      =%.6f" %
      pointPDF, " cdf      =%.6f" % pointCDF)
print("Point = ", repr(point), " pdf (ref)=%.6f" %
      pointPDFRef, " cdf (ref)=%.6f" % pointCDFRef)

# Get 50% quantile
quantile = copula.computeQuantile(0.5)
quantileRef = copulaRef.computeQuantile(0.5)
print("Quantile      =", repr(quantile))
print("Quantile (ref)=", repr(quantileRef))
print("CDF(quantile)=%.6f" % copula.computeCDF(quantile))
# Get 95% survival function
inverseSurvival = ot.Point(copula.computeInverseSurvivalFunction(0.95))
print("InverseSurvival=", repr(inverseSurvival))
print("Survival(inverseSurvival)=%.6f" %
      copula.computeSurvivalFunction(inverseSurvival))
# Takes too much time
# print("entropy=%.6f" % copula.computeEntropy())
# Confidence regions
if copula.getDimension() <= 2:
    threshold = ot.Point()
    print("Minimum volume interval=",
          copula.computeMinimumVolumeInterval(0.95, threshold))
    print("threshold=", threshold)
    beta = ot.Point()
    levelSet = copula.computeMinimumVolumeLevelSet(0.95, beta)
    print("Minimum volume level set=", levelSet)
    print("beta=", beta)
    print("Bilateral confidence interval=",
          copula.computeBilateralConfidenceInterval(0.95, beta))
    print("beta=", beta)
    print("Unilateral confidence interval (lower tail)=",
          copula.computeUnilateralConfidenceInterval(0.95, False, beta))
    print("beta=", beta)
    print("Unilateral confidence interval (upper tail)=",
          copula.computeUnilateralConfidenceInterval(0.95, True, beta))
    print("beta=", beta)

# Extract the marginals
for i in range(dim):
    margin = copula.getMarginal(i)
    marginRef = copulaRef.getMarginal(i)
    print("margin=", repr(margin))
    print("margin PDF      =%.6f" %
          margin.computePDF(ot.Point(1, 0.25)))
    print("margin PDF (ref)=%.6f" %
          marginRef.computePDF(ot.Point(1, 0.25)))
    print("margin CDF      =%.6f" %
          margin.computeCDF(ot.Point(1, 0.25)))
    print("margin CDF (ref)=%.6f" %
          marginRef.computeCDF(ot.Point(1, 0.25)))
    print("margin quantile      =", repr(margin.computeQuantile(0.95)))
    print("margin quantile (ref)=", repr(marginRef.computeQuantile(0.95)))
    print("margin realization=", repr(margin.getRealization()))

# Extract a 2-D marginal
indices = ot.Indices([1, 0])
print("indices=", repr(indices))
margin = copula.getMarginal(indices)
marginRef = copulaRef.getMarginal(indices)
print("margin=", repr(margin))
print("margin PDF      =%.6f" % margin.computePDF(ot.Point(2, 0.25)))
print("margin PDF (ref)=%.6f" %
      marginRef.computePDF(ot.Point(2, 0.25)))
print("margin CDF      =%.6f" % margin.computeCDF(ot.Point(2, 0.25)))
print("margin CDF (ref)=%.6f" %
      marginRef.computeCDF(ot.Point(2, 0.25)))
print("margin quantile      =", repr(margin.computeQuantile(0.95)))
print("margin quantile (ref)=", repr(marginRef.computeQuantile(0.95)))
print("margin realization=", repr(margin.getRealization()))

# tbb nested parallelism issue
student = ot.Student(3.0, [1.0]*2, [3.0]*2, ot.CorrelationMatrix(2))
copula = ot.SklarCopula(student)
pdf_graph = copula.drawPDF()
cdf_graph = copula.drawCDF()
