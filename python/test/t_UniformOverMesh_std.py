#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
mesher = ot.LevelSetMesher([25] * 2)
function = ot.SymbolicFunction(["x0", "x1"], ["10*(x0^3+x1)^2+x0^2"])
level = 0.5
domain = ot.LevelSet(function, ot.LessOrEqual(), level)
lower = [-0.75, -0.5]
upper = [0.75, 0.5]
mesh = mesher.build(domain, ot.Interval(lower, upper), False)
distribution = ot.UniformOverMesh(mesh)
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", oneRealization)

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
print("oneSample first=", oneSample[0], " last=", oneSample[size - 1])
print("mean=", oneSample.computeMean())
print("covariance=", oneSample.computeCovariance())

# Define a point
point = [0.1] * 2
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
checker = ott.DistributionChecker(distribution)
checker.skipMoments()  # slow
checker.skipCorrelation()  # slow
checker.run()
