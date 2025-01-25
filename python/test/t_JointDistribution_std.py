#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import openturns.experimental as otexp

ot.TESTPREAMBLE()

mean = [3.0, 2.0, 1.0]
sigma = [2.0, 3.0, 4.0]

refCovariances = list()
refCovariances.append(
    ot.CovarianceMatrix(3, [4.0, 0.0, 0.0, 0.0, 9.0, 0.0, 0.0, 0.0, 16.0])
)
refCovariances.append(
    ot.CovarianceMatrix(3, [4.0, 1.5, 0.0, 1.5, 9.0, 3.0, 0.0, 3.0, 16.0])
)
refCovariances.append(
    ot.CovarianceMatrix(3, [4.0, 1.125, 0.0, 1.125, 9.0, 2.25, 0.0, 2.25, 16.0])
)
refCovariances.append(
    ot.CovarianceMatrix(
        3,
        [
            4.0,
            2.0696999,
            -4.403889,
            2.0696999,
            9.0,
            4.1393998,
            -4.403889,
            4.1393998,
            16.0,
        ],
    )
)
refCovariances.append(
    ot.CovarianceMatrix(
        3, [0.39606657, 0.0, 0.0, 0.0, 0.891149785, 0.0, 0.0, 0.0, 1.584266284]
    )
)

refStandardDeviation = [
    [2, 3, 4],
    [2, 3, 4],
    [2, 3, 4],
    [1.49595080640498, 2.00948748222124, 2.99190161280996],
    [0.62933820074628, 0.94400730111948, 1.25867640149264],
]
refSkewness = [
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [-0.213157049688829, 0.0, 0.213157049689032],
    [0.22711106425, 0.22711106425, 0.22711106425],
]
refKurtosis = [
    [3, 3, 3],
    [3, 3, 3],
    [3, 3, 3],
    [3.11664895604121, 3.03472746922749, 3.11664895604127],
    [2.439305739629, 2.439305739629, 2.439305739629],
]

# Create a collection of distribution attente TUI
aCollection = []
try:
    aCollection[10] = ot.Normal()
except Exception:
    pass

dim = len(mean)
# Create a collection of distribution
aCollection = list()
marginal = ot.Normal(mean[0], sigma[0])
marginal.setName("First")
marginal.setDescription(["One"])
aCollection.append(marginal)
marginal = ot.Normal(mean[1], sigma[1])
marginal.setName("Second")
marginal.setDescription(["Two"])
aCollection.append(marginal)
marginal = ot.Normal(mean[2], sigma[2])
marginal.setName("Third")
marginal.setDescription(["Three"])
aCollection.append(marginal)

# Create a copula
aCopula = ot.IndependentCopula(dim)
aCopula.setName("Independent copula")
cores = [aCopula]
# With a Normal copula
correlation = ot.CorrelationMatrix(dim)
for i in range(1, dim):
    correlation[i - 1, i] = 0.25
anotherCopula = ot.NormalCopula(correlation)
anotherCopula.setName("Normal copula")
cores.append(anotherCopula)
# With a copula which is not a copula by type
atoms = [aCopula, anotherCopula]
cores.append(ot.Mixture(atoms, [0.25, 0.75]))
# With a non-copula core
cores.append(otexp.UniformOrderStatistics(dim))
# With a core which support is strictly included in the unit cube
cores.append(ot.KernelMixture(ot.Beta(2.0, 3.0, 0.2, 0.8), [1.0] * dim, [[0.0] * dim]))
ot.ResourceMap.SetAsBool("JointDistribution-UseGenericCovarianceAlgorithm", True)
for nCore in range(len(cores)):
    print("\n\n")
    # Instantiate one distribution object
    distribution = ot.JointDistribution(aCollection, cores[nCore])
    distribution.setName("myDist")
    print("Distribution", repr(distribution))
    print("Distribution")
    print(distribution)
    print("Distribution (Markdown)")
    print(distribution.__repr_markdown__())
    print("Parameters", distribution.getParametersCollection())
    print("nCore=", nCore)

    # too slow for Mixture/KernelMixture
    if "Mixture" not in distribution.getCore().getImplementation().getName():
        print("entropy=%.5e" % distribution.computeEntropy())
        print(
            "entropy (MC)=%.5e"
            % -distribution.computeLogPDF(
                distribution.getSample(1000000)
            ).computeMean()[0]
        )
    print("Mean ", distribution.getMean())
    precision = ot.PlatformInfo.GetNumericalPrecision()
    print("Covariance ")
    if nCore != 3:
        covariance = distribution.getCovariance()
        covariance.checkSymmetry()
        ott.assert_almost_equal(covariance, refCovariances[nCore])
    # Is this distribution an elliptical distribution?
    print("Elliptical distribution= ", distribution.isElliptical())

    # Has this distribution an elliptical copula?
    print("Elliptical copula= ", distribution.hasEllipticalCopula())

    # Has this distribution an independent copula?
    print("Independent copula= ", distribution.hasIndependentCopula())

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    print("oneRealization=", oneRealization)
    # Test for sampling
    size = 10
    oneSample = distribution.getSample(size)
    print("oneSample=", oneSample)

    # Test for sampling
    size = 10000
    anotherSample = distribution.getSample(size)
    print("anotherSample mean=", anotherSample.computeMean())
    print("anotherSample covariance=", anotherSample.computeCovariance())

    # Define a point
    zero = [0.0] * dim

    # Show PDF and CDF of zero point
    zeroPDF = distribution.computePDF(zero)
    zeroCDF = distribution.computeCDF(zero)
    print("Zero point= ", zero, " pdf=%.5e" % zeroPDF, " cdf=%.5e" % zeroCDF)
    # Get 95% quantile
    quantile = distribution.computeQuantile(0.95)
    print("Quantile=", quantile)
    print("CDF(quantile)=%.5e" % distribution.computeCDF(quantile))

    # Extract the marginals
    for i in range(dim):
        margin = distribution.getMarginal(i)
        print("margin=", margin)
        print("margin PDF=%.5e" % margin.computePDF([0.0]))
        print("margin CDF=%.5e" % margin.computeCDF([0.0]))
        print("margin quantile=", margin.computeQuantile(0.95))
        print("margin realization=", margin.getRealization())

    # Extract a 2-D marginal
    indices = [1, 0]
    print("indices=", indices)
    margins = distribution.getMarginal(indices)
    print("margins=", margins)
    print("margins PDF=%.5e" % margins.computePDF([0.0] * 2))
    print("margins CDF=%.5e" % margins.computeCDF([0.0] * 2))
    quantile = margins.computeQuantile(0.5)
    print("margins quantile=", quantile)
    print("margins CDF(quantile)=%.5e" % margins.computeCDF(quantile))
    print("margins realization=", margins.getRealization())
    x = 0.6
    y = [0.2] * (dim - 1)
    print("conditional PDF=%.5e" % distribution.computeConditionalPDF(x, y))
    print("conditional CDF=%.5e" % distribution.computeConditionalCDF(x, y))
    print("conditional quantile=%.5e" % distribution.computeConditionalQuantile(x, y))
    pt = [i + 1.5 for i in range(dim)]
    print(
        "sequential conditional PDF=", distribution.computeSequentialConditionalPDF(pt)
    )
    resCDF = distribution.computeSequentialConditionalCDF(pt)
    print("sequential conditional CDF(", pt, ")=", resCDF)
    print(
        "sequential conditional quantile(",
        resCDF,
        ")=",
        distribution.computeSequentialConditionalQuantile(resCDF),
    )

    # Confidence regions
    if distribution.getDimension() <= 2:
        (
            levelSet,
            threshold,
        ) = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
        print("Minimum volume interval=", levelSet)
        print("threshold=%.5e" % threshold)
        levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
        print("Minimum volume level set=", levelSet)
        print("beta=%.5e" % beta)
        (
            interval,
            beta,
        ) = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
        print("Bilateral confidence interval=", interval)
        print("beta=%.5e" % beta)
        (
            interval,
            beta,
        ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
            0.95, False
        )
        print("Unilateral confidence interval (lower tail)=", interval)
        print("beta=%.5e" % beta)
        (
            interval,
            beta,
        ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
            0.95, True
        )
        print("Unilateral confidence interval (upper tail)=", interval)
        print("beta=%.5e" % beta)
    # Moments other than mean and covariance
    standardDeviation = distribution.getStandardDeviation()
    ott.assert_almost_equal(standardDeviation, refStandardDeviation[nCore])
    skewness = distribution.getSkewness()
    ott.assert_almost_equal(skewness, refSkewness[nCore])
    kurtosis = distribution.getKurtosis()
    ott.assert_almost_equal(kurtosis, refKurtosis[nCore])
    anotherSample = distribution.getSample(size)
    print("anotherSample mean=", anotherSample.computeMean())
    print("anotherSample covariance=", anotherSample.computeCovariance())

# Create and print a complex distribution
aCollection2 = list()
marginalN = ot.Normal(0.0, 1.0)
marginalN.setName("First")
marginalN.setDescription(["One"])
aCollection2.append(marginalN)
marginalU = ot.Uniform(12345.6, 123456.7)
marginalU.setName("Second")
marginalU.setDescription(["Two"])
aCollection2.append(marginalU)
marginalTN = ot.TruncatedDistribution(ot.Normal(2.0, 1.5), 1.0, 4.0)
marginalTN.setName("Third")
marginalTN.setDescription(["Three"])
aCollection2.append(marginalTN)
distribution2 = ot.JointDistribution(aCollection2)
distribution2.setName("myDist2")
print("Distribution ")
print(distribution2)
print("Distribution (Markdown)")
print(distribution2.__repr_markdown__())

# test transfo
sample = distribution.getSample(10)
print(sample)
sample_iso = distribution.getIsoProbabilisticTransformation()(sample)
print(sample_iso)
sample_inv = distribution.getInverseIsoProbabilisticTransformation()(sample_iso)
print(sample_inv)
x = 0.6
y = [0.2] * (dim - 1)
print("conditional PDF=%.6f" % distribution.computeConditionalPDF(x, y))
print("conditional CDF=%.6f" % distribution.computeConditionalCDF(x, y))
print("conditional quantile=%.6f" % distribution.computeConditionalQuantile(x, y))
pt = [i + 1.5 for i in range(dim)]
print("sequential conditional PDF=", distribution.computeSequentialConditionalPDF(pt))
resCDF = distribution.computeSequentialConditionalCDF(pt)
print("sequential conditional CDF(", pt, ")=", resCDF)
print(
    "sequential conditional quantile(",
    resCDF,
    ")=",
    distribution.computeSequentialConditionalQuantile(resCDF),
)

# comparison
d = ot.JointDistribution([ot.Uniform()] * 2, ot.ClaytonCopula(2.5))
d1 = ot.MaximumDistribution(d)
d2 = ot.MaximumDistribution(d)
assert d1.getDistribution() == d2.getDistribution(), "comp1"
assert d1 == d2, "comp2"

# check for lost description
dist_a = ot.Normal()
dist_a.setDescription(["a"])
dist_b = ot.Normal()
dist_b.setDescription(["b"])
dist_list = [dist_a, dist_b] + [ot.Normal()] * 3
composed = ot.JointDistribution(dist_list)
assert composed.getDescription() == ["a", "b", "X0", "X1", "X2"], "wrong description"

# Create and print a composed distribution with different
# complexities and print them
aCollection = [
    ot.Uniform(),
    ot.TruncatedDistribution(ot.Normal(2.0, 1.5), 1.0, 4.0),
    ot.Normal(),
]
distribution = ot.JointDistribution(aCollection)
print("Distribution = ")
print(distribution)
print("Distribution (Markdown) = ")
print(distribution.__repr_markdown__())
print("Distribution (HTML) = ")
print(distribution._repr_html_())

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()

# Check if one can detect if the distribution is elliptical
# Normal marginals, independent copula
distribution = ot.JointDistribution([ot.Normal(1.0, 2.0), ot.Normal(-2.0, 1.0)])
print("Normal marginals, independent copula, isElliptical?", distribution.isElliptical())
# Normal marginals, normal copula
R = ot.CorrelationMatrix(2, [1.0, 0.5, 0.5, 1.0])
distribution = ot.JointDistribution([ot.Normal(1.0, 2.0), ot.Normal(-2.0, 1.0)], ot.NormalCopula(R))
print("Normal marginals, normal copula, isElliptical?", distribution.isElliptical())
# Normal marginals, non-normal copula
distribution = ot.JointDistribution([ot.Normal(1.0, 2.0), ot.Normal(-2.0, 1.0)], ot.ClaytonCopula(1.0))
print("Normal marginals, non-normal copula, isElliptical?", distribution.isElliptical())
# Student marginals, Student copula, all with the same nu
nu = 3.5
distribution = ot.JointDistribution([ot.Student(nu, 1.0, 2.0), ot.Student(nu, -2.0, 1.0)], ot.StudentCopula(nu, R))
print("Student marginals, Student copula, same nu, isElliptical?", distribution.isElliptical())
distribution = ot.JointDistribution([ot.Student(nu, 1.0, 2.0), ot.Student(nu, -2.0, 1.0)], ot.StudentCopula(nu + 1.0, R))
print("Student marginals same nu, Student copula different nu, isElliptical?", distribution.isElliptical())
distribution = ot.JointDistribution([ot.Student(nu + 1.0, 1.0, 2.0), ot.Student(nu, -2.0, 1.0)], ot.StudentCopula(nu + 1.0, R))
print("Student marginals different nu, Student copula same nu as first marginal, isElliptical?", distribution.isElliptical())
