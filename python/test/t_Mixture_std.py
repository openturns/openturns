#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
dimension = 3
meanPoint = ot.Point(dimension, 1.0)
meanPoint[0] = 0.5
meanPoint[1] = -0.5
sigma = ot.Point(dimension, 1.0)
sigma[0] = 2.0
sigma[1] = 3.0
R = ot.CorrelationMatrix(dimension)
for i in range(1, dimension):
    R[i, i - 1] = 0.5

# Create a collection of distribution
aCollection = ot.DistributionCollection()

aCollection.add(ot.Normal(meanPoint, sigma, R))
meanPoint += ot.Point(meanPoint.getDimension(), 1.0)
aCollection.add(ot.Normal(meanPoint, sigma, R))
meanPoint += ot.Point(meanPoint.getDimension(), 1.0)
aCollection.add(ot.Normal(meanPoint, sigma, R))

# Instantiate one distribution object
distribution = ot.Mixture(aCollection, ot.Point(aCollection.getSize(), 2.0))
print("Distribution ", repr(distribution))
print("Weights = ", repr(distribution.getWeights()))
weights = distribution.getWeights()
weights[0] = 2.0 * weights[0]
distribution.setWeights(weights)
print("After update, new weights = ", repr(distribution.getWeights()))
distribution = ot.Mixture(aCollection)
print("Distribution ", repr(distribution))

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", repr(oneRealization))

# Test for sampling
size = 1000
oneSample = distribution.getSample(size)
print("oneSample first=", repr(oneSample[0]), " last=", repr(oneSample[size - 1]))
print("mean=", repr(oneSample.computeMean()))
print("covariance=", repr(oneSample.computeCovariance()))

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
print("Point= ", repr(point))

# derivative of PDF with regards its arguments
DDF = distribution.computeDDF(point)
print("ddf     =", repr(DDF))

# PDF value
LPDF = distribution.computeLogPDF(point)
print("log pdf=%.6f" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     =%.6f" % PDF)

x = 0.6
y = [0.2] * (dimension - 1)
print("conditional PDF%.6f" % distribution.computeConditionalPDF(x, y))
print("conditional CDF%.6f" % distribution.computeConditionalCDF(x, y))
print("conditional quantile%.6f" % distribution.computeConditionalQuantile(x, y))
pt = ot.Point([i + 1.5 for i in range(dimension)])
print(
    "sequential conditional PDF=", distribution.computeSequentialConditionalPDF(point)
)
resCDF = distribution.computeSequentialConditionalCDF(pt)
print("sequential conditional CDF(", pt, ")=", resCDF)
print(
    "sequential conditional quantile(",
    resCDF,
    ")=",
    distribution.computeSequentialConditionalQuantile(resCDF),
)
# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
print("cdf=%.6f" % CDF)
CCDF = distribution.computeComplementaryCDF(point)
print("ccdf=%.6f" % CCDF)
PDFgr = distribution.computePDFGradient(point)
assert distribution.computePDFGradient([point]).getDimension() == len(PDFgr)
print("pdf gradient=", PDFgr)

# derivative of the PDF with regards the parameters of the distribution
CDFgr = distribution.computeCDFGradient(point)
print("cdf gradient=", CDFgr)

# quantile
quantile = distribution.computeQuantile(0.95)
print("quantile=", repr(quantile))
print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
print("InverseSurvival=", repr(inverseSurvival))
print(
    "Survival(inverseSurvival)=%.6f"
    % distribution.computeSurvivalFunction(inverseSurvival)
)
# Confidence regions
if distribution.getDimension() <= 2:
    (
        interval,
        threshold,
    ) = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
    print("Minimum volume interval=", interval)
    print("threshold=", ot.Point(1, threshold))
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
    print("Minimum volume level set=", levelSet)
    print("beta=", ot.Point(1, beta))
    (
        interval,
        beta,
    ) = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
    print("Bilateral confidence interval=", interval)
    print("beta=", ot.Point(1, beta))
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False
    )
    print("Unilateral confidence interval (lower tail)=", interval)
    print("beta=", ot.Point(1, beta))
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True
    )
    print("Unilateral confidence interval (upper tail)=", interval)
    print("beta=", ot.Point(1, beta))

mean = distribution.getMean()
print("mean=", repr(mean))
covariance = distribution.getCovariance()
print("covariance=", repr(covariance))
parameters = distribution.getParametersCollection()
print("parameters=", repr(parameters))
parameter = distribution.getParameter()
print("parameter=", parameter)
parameterDescription = distribution.getParameterDescription()
print("parameter description=", parameterDescription)
distribution.setParameter(parameter)
print("Standard representative=", distribution.getStandardRepresentative())

# Constructor with separate weights. Also check small weights removal
weights = [1.0e-20, 2.5, 32.0]
atoms = ot.DistributionCollection(
    [ot.Normal(1.0, 1.0), ot.Normal(2.0, 2.0), ot.Normal(3.0, 3.0)]
)
newMixture = ot.Mixture(atoms, weights)
print("newMixture pdf= %.12g" % newMixture.computePDF(2.5))
print("atoms kept in mixture=", newMixture.getDistributionCollection())
print("newMixture=", newMixture)

ot.Log.Show(ot.Log.TRACE)
checker = ott.DistributionChecker(distribution)
checker.skipMoments()  # slow
checker.skipCorrelation()  # slow
checker.run()

# check for non stricly increasing CDF: must return the inf of [1, 2]
distribution = ot.Mixture([ot.Uniform(0.0, 1.0), ot.Uniform(2.0, 3.0)])
q = distribution.computeQuantile(0.5)[0]
print(f"q={q:.6f}")
