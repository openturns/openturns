#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Force the use of the approximation to avoid timeout
ot.ResourceMap.SetAsBool(
    "MaximumEntropyOrderStatisticsDistribution-UseApproximation", True
)

# Instantiate one copula object
copula = ot.MaximumEntropyOrderStatisticsCopula(
    [
        ot.Trapezoidal(-2.0, -1.1, -1.0, 1.0),
        ot.LogUniform(1.0, 1.2),
        ot.Triangular(3.0, 4.5, 5.0),
        ot.Beta(2.5, 3.5, 4.7, 5.2),
    ]
)

dim = copula.getDimension()
print("Copula ", copula)

# Is this copula an elliptical copula?
print("Elliptical copula= ", copula.isElliptical())

# Is this copula elliptical ?
print("Elliptical copula= ", copula.hasEllipticalCopula())

# Is this copula independent ?
print("Independent copula= ", copula.hasIndependentCopula())

# Test for realization of copula
oneRealization = copula.getRealization()
print("oneRealization=", repr(oneRealization))

# Test for sampling
size = 10000
oneSample = copula.getSample(size)
print("oneSample first=", repr(oneSample[0]), " last=", repr(oneSample[size - 1]))
print("mean=", repr(oneSample.computeMean()))
print("covariance=", repr(oneSample.computeCovariance()))

# Define a point
point = ot.Point([0.35, 0.15, 0.02, 0.33])
print("Point= ", point)

# derivative of PDF with regards its arguments
DDF = copula.computeDDF(point)
print("ddf     =", repr(DDF))
# PDF value
LPDF = copula.computeLogPDF(point)
print("log pdf=%.6f" % LPDF)
PDF = copula.computePDF(point)
print("pdf     =%.6f" % PDF)
condPDF = copula.computeConditionalPDF(0.6, point[:3])
print("condPDF     =%.6f" % condPDF)

# derivative of the PDF with regards the parameters of the copula
CDF = copula.computeCDF(point)
print("cdf=%.6f" % CDF)
CCDF = copula.computeComplementaryCDF(point)
print("ccdf=%.6f" % CCDF)
condCDF = copula.computeConditionalCDF(0.6, point[:3])
print("condCDF     =%.6f" % condCDF)
quantile = copula.computeQuantile(0.95)
print("quantile=", repr(quantile))
print("cdf(quantile)=%.6f" % copula.computeCDF(quantile))

condQuantile = copula.computeConditionalQuantile(0.6, point[:3])
print("condQuantile     =%.6f" % condQuantile)
mean = copula.getMean()
print("mean=", repr(mean))
standardDeviation = copula.getStandardDeviation()
print("standard deviation=", repr(standardDeviation))
skewness = copula.getSkewness()
print("skewness=", repr(skewness))
kurtosis = copula.getKurtosis()
print("kurtosis=", repr(kurtosis))
ot.ResourceMap.SetAsUnsignedInteger("GaussKronrod-MaximumSubIntervals", 20)
ot.ResourceMap.SetAsScalar("GaussKronrod-MaximumError", 1.0e-4)
covariance = copula.getCovariance()
print("covariance=", repr(covariance))
correlation = copula.getCorrelation()
print("correlation=", repr(correlation))
spearman = copula.getSpearmanCorrelation()
print("spearman=", repr(spearman))
ot.ResourceMap.SetAsUnsignedInteger("GaussKronrod-MaximumSubIntervals", 100)
ot.ResourceMap.SetAsScalar("GaussKronrod-MaximumError", 1.0e-12)
parameters = copula.getParametersCollection()
print("parameters=", repr(parameters))

# Extract the marginals
for i in range(dim):
    margin = copula.getMarginal(i)
    print("margin=", repr(margin))
    print("margin PDF=%.6f" % margin.computePDF(point[i]))
    print("margin CDF=%.6f" % margin.computeCDF(point[i]))
    print("margin quantile=", repr(margin.computeQuantile(0.5)))
    print("margin realization=", repr(margin.getRealization()))
    print("margin range=", repr(margin.getRange()))

# Extract a 2-D marginal
indices = [0, 1]
print("indices=", repr(indices))
margins = copula.getMarginal(indices)
print("margins=", repr(margins))
print("margins PDF=%.6f" % margins.computePDF([point[i] for i in indices]))
print("margins CDF=%.6f" % margins.computeCDF([point[i] for i in indices]))
quantile = ot.Point(margins.computeQuantile(0.8))
print("margins quantile=", repr(quantile))
print("margins CDF(qantile)=%.6f" % margins.computeCDF(quantile))
print("margins realization=", repr(margins.getRealization()))


ot.Log.Show(ot.Log.TRACE)
checker = ott.DistributionChecker(copula)
checker.skipMoments()  # slow
checker.skipCorrelation()  # slow
checker.run()
