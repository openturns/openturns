#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Instantiate one distribution object
distribution = ot.Multinomial(5, ot.Point(3, 0.25))
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", oneRealization)

print("support=\n" + str(distribution.getSupport()))
interval = ot.Interval(
    ot.Point(distribution.getDimension(), 1.0),
    ot.Point(distribution.getDimension(), 3.0),
)
print(
    "support restricted to the interval=\n"
    + str(interval)
    + " gives=\n"
    + str(distribution.getSupport(interval))
)

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
print("Point= ", repr(point))

# Show PDF and CDF at point
LPDF = distribution.computeLogPDF(point)
print("log pdf=%.6f" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     =%.6f" % PDF)
CDF = distribution.computeCDF(point)
print("cdf=%.5f" % CDF)
proba = distribution.computeProbability(
    ot.Interval(
        [i for i in range(distribution.getDimension())],
        [i + 1.0 for i in range(distribution.getDimension())],
    )
)
print("probability=%.5f" % proba)
quantile = distribution.computeQuantile(0.95)
print("quantile=", repr(quantile))
print("cdf(quantile)= %.6f" % distribution.computeCDF(quantile))
print("entropy=%.6f" % distribution.computeEntropy())

mean = distribution.getMean()
print("mean=", repr(mean))
covariance = distribution.getCovariance()
print("covariance=", repr(covariance))
parameters = distribution.getParametersCollection()
print("parameters=", repr(parameters))
parameter = distribution.getParameter()
print("parameter=", repr(parameter))
print("parameterDesc=", distribution.getParameterDescription())
distribution.setParameter(parameter)

ot.Log.Show(ot.Log.TRACE)
checker = ott.DistributionChecker(distribution)
checker.skipMoments()  # slow
checker.skipCorrelation()  # slow
checker.run()
