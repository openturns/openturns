#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
mesher = ot.LevelSetMesher([25] * 2)
function = ot.SymbolicFunction(["x0", "x1"], ["sin(x0) * sin(x1)"])
level = 0.2
domain = ot.LevelSet(function, ot.LessOrEqual(), level)
lower = [-5.0] * 2
upper = [5.0] * 2
mesh = mesher.build(domain, ot.Interval(lower, upper), False)
dist = ot.Normal([1.0] * 2, [2.0] * 2)
distribution = ot.TruncatedOverMesh(dist, mesh)
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", oneRealization)
mean = distribution.getMean()
print("mean=", mean)

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
print("oneSample first=", oneSample[0], " last=", oneSample[size - 1])
print("mean=", oneSample.computeMean())
print("covariance=", oneSample.computeCovariance())

# Define a point
point = [3.0] * 2
print("Point= ", point)

# Show PDF and CDF of point
DDF = distribution.computeDDF(point)
print("ddf     =", DDF)
LPDF = distribution.computeLogPDF(point)
print("log pdf=%.5g" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     =%.5g" % PDF)
CDF = distribution.computeCDF(point)
print("cdf=%.5g" % CDF)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
# validation.skipMoments()
validation.skipDependenceMeasures()  # slow
validation.skipConditional()  # slow
validation.run()
ot.ResourceMap.SetAsBool("TruncatedOverMesh-UseRejection", True)
validation = ott.DistributionValidation(distribution)
validation.skipDependenceMeasures()  # slow
validation.skipConditional()  # slow
validation.run()
