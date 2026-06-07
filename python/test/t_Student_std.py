#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
allDistributions = [ot.Student(6.5, -0.5, 2.0)]
dim = 2
R = ot.CorrelationMatrix(dim)
mu = list()
sigma = list()
for i in range(dim):
    mu.append(i)
    sigma.append((1.0 + i) / dim)
    for j in range(i):
        R[i, j] = 1.0 / (1.0 + dim + i + j)
allDistributions.append(ot.Student(7.5, mu, sigma, R))
for distribution in allDistributions:
    dim = distribution.getDimension()
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
    size = 10000
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

    CDF = distribution.computeCDF(point)
    print("cdf=%.6f" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=%.6f" % CCDF)

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
print("entropy=%.6f" % distribution.computeEntropy())

# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
print("Minimum volume interval=", interval)
print("threshold=", ot.Point(1, threshold))
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
print("Minimum volume level set=", levelSet)
print("beta=", ot.Point(1, beta))
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
print("Bilateral confidence interval=", interval)
print("beta=", ot.Point(1, beta))
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
print("Unilateral confidence interval (lower tail)=", interval)
print("beta=", ot.Point(1, beta))
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
print("Unilateral confidence interval (upper tail)=", interval)
print("beta=", ot.Point(1, beta))

mean = distribution.getMean()
print("mean=", repr(mean))
standardDeviation = distribution.getStandardDeviation()
print("standard deviation=", repr(standardDeviation))
skewness = distribution.getSkewness()
print("skewness=", repr(skewness))
kurtosis = distribution.getKurtosis()
print("kurtosis=", repr(kurtosis))
covariance = distribution.getCovariance()
print("covariance=", repr(covariance))
parameters = distribution.getParametersCollection()
print("parameters=", repr(parameters))
print("Standard representative=", distribution.getStandardRepresentative())

# Specific to this distribution
beta = point.normSquare()
densityGenerator = distribution.computeDensityGenerator(beta)
print("density generator=%.6f" % densityGenerator)
print(
    "pdf via density generator=%.6f"
    % ot.EllipticalDistribution.computePDF(distribution, point)
)
densityGeneratorDerivative = distribution.computeDensityGeneratorDerivative(beta)
print("density generator derivative     =%.6f" % densityGeneratorDerivative)
eps = 1e-5
print(
    "density generator derivative (FD)=%.6f"
    % (
        (
            distribution.computeDensityGenerator(beta + eps)
            - distribution.computeDensityGenerator(beta - eps)
        )
        / (2.0 * eps)
    )
)
densityGeneratorSecondDerivative = distribution.computeDensityGeneratorSecondDerivative(
    beta
)
print(
    "density generator second derivative     =%.6f" % densityGeneratorSecondDerivative
)
print(
    "density generator second derivative (FD)=%.6f"
    % (
        (
            distribution.computeDensityGeneratorDerivative(beta + eps)
            - distribution.computeDensityGeneratorDerivative(beta - eps)
        )
        / (2.0 * eps)
    )
)

x = 0.6
y = [0.2] * (dim - 1)
print("conditional PDF=%.6f" % distribution.computeConditionalPDF(x, y))
print("conditional CDF=%.6f" % distribution.computeConditionalCDF(x, y))
print("conditional quantile=%.6f" % distribution.computeConditionalQuantile(x, y))
pt = ot.Point([i + 1.5 for i in range(dim)])
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

# Large dimension tests for computeCDF / computeProbability (dim >= 4 uses Genz algorithm)
allNu = [2.5, 5.0, 10.2]
allRho = [0.0, 0.25, 0.5]
a = [-1.0] * 2
b = [2.0] * 2
interval = ot.Interval(a, b)
# Ordered by nu then by rho, GaussLegendre reference
ref_Rect2 = [
    0.5542893382972078,
    0.5638056783688250,
    0.5826850375700735,
    0.6056819561946407,
    0.6165826443659900,
    0.6370666418457498,
    0.6367180611403404,
    0.6484974632474523,
    0.6695958701057342,
]
index = 0
for nu in allNu:
    for rho in allRho:
        R = ot.CorrelationMatrix(
            2, [rho if i != j else 1.0 for i in range(2) for j in range(2)]
        )
        student = ot.Student(nu, [0.0] * 2, [1.0] * 2, R)
        ott.assert_almost_equal(
            student.computeProbability(interval), ref_Rect2[index], 1e-6, 1e-6
        )
        index += 1

a = [-1.0] * 3
b = [2.0] * 3
interval = ot.Interval(a, b)
ref_Rect3 = [
    0.4453154969507699,
    0.4646177063807000,
    0.4986876436159653,
    0.4883397061432511,
    0.5113084397051081,
    0.5501992086166536,
    0.5161696249737660,
    0.5417420244794307,
    0.5833456309432767,
]
index = 0
for nu in allNu:
    for rho in allRho:
        R = ot.CorrelationMatrix(
            3, [rho if i != j else 1.0 for i in range(3) for j in range(3)]
        )
        student = ot.Student(nu, [0.0] * 3, [1.0] * 3, R)
        ott.assert_almost_equal(
            student.computeProbability(interval), ref_Rect3[index], 1e-6, 1e-6
        )
        index += 1
oldDiscretization = ot.ResourceMap.GetAsUnsignedInteger("Genz-DefaultSampleSize")
ot.ResourceMap.SetAsUnsignedInteger("Genz-DefaultSampleSize", 2**12)

point4 = [0.0, 1.0, 2.0, 3.0]
ref_CDF4 = [
    0.3630942967018709,
    0.4027397833526201,
    0.4399797510132942,
    0.3858660420936641,
    0.4212022884795089,
    0.4540149006391191,
    0.3983930597179132,
    0.4309038585416387,
    0.4610285283377433,
]
a4 = [-1.0] * 4
b4 = [2.0] * 4
interval4 = ot.Interval(a4, b4)
ref_Rect4 = [
    0.3680452728592088,
    0.3956311245556713,
    0.4406609666511867,
    0.4004529619859127,
    0.4339815196901536,
    0.4870163625850399,
    0.4221828704444365,
    0.4602190013010164,
    0.5184666129967919,
]
index = 0
for nu in allNu:
    for rho in allRho:
        R = ot.CorrelationMatrix(
            4, [rho if i != j else 1.0 for i in range(4) for j in range(4)]
        )
        student = ot.Student(nu, [0.0] * 4, [1.0] * 4, R)
        ott.assert_almost_equal(student.computeCDF(point4), ref_CDF4[index], 2e-3, 2e-3)
        ott.assert_almost_equal(
            student.computeProbability(interval4), ref_Rect4[index], 2e-3, 2e-3
        )
        index += 1

point5 = [0.0, 1.0, 2.0, 3.0, 4.0]
ref_CDF5 = [
    0.3596048651978545,
    0.4007531741257658,
    0.4390368807799120,
    0.3846376376012848,
    0.4206482558227036,
    0.4538318225999918,
    0.3980308686556492,
    0.4307753740626505,
    0.4610021531685218,
]
a5 = [-1.0] * 5
b5 = [2.0] * 5
interval5 = ot.Interval(a5, b5)
ref_Rect5 = [
    0.3105619255978981,
    0.3447731215647129,
    0.3976389268551727,
    0.3329636646166807,
    0.3749910667117484,
    0.4384938111582938,
    0.3480623084468400,
    0.3962597434856491,
    0.4673136362143094,
]
index = 0
for nu in allNu:
    for rho in allRho:
        R = ot.CorrelationMatrix(
            5, [rho if i != j else 1.0 for i in range(5) for j in range(5)]
        )
        student = ot.Student(nu, [0.0] * 5, [1.0] * 5, R)
        ott.assert_almost_equal(student.computeCDF(point5), ref_CDF5[index], 2e-3, 2e-3)
        ott.assert_almost_equal(
            student.computeProbability(interval5), ref_Rect5[index], 2e-3, 2e-3
        )
        index += 1
ot.ResourceMap.SetAsUnsignedInteger("Genz-DefaultSampleSize", oldDiscretization)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipCorrelation()  # slow due to Spearman's rho
try:
    validation.run()
except Exception:
    pass

# non-spd cov
dist = ot.Student(
    4.2,
    [0] * 3,
    ot.CovarianceMatrix([[1.0, 1.0, 0.0], [1.0, 1.0, 0.0], [0.0, 0.0, 1.0]]),
)
sample = dist.getSample(10)
