#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


def cleanScalar(inScalar):
    if abs(inScalar) < 1.0e-10:
        inScalar = 0.0
    return inScalar


def cleanPoint(inPoint):
    dim = inPoint.getDimension()
    for i in range(dim):
        if abs(inPoint[i]) < 1.0e-10:
            inPoint[i] = 0.0
    return inPoint


ot.PlatformInfo.SetNumericalPrecision(5)
# Instantiate one distribution object
for dim in range(1, 5):
    meanPoint = [0.0] * dim
    sigma = [1.0 + i for i in range(dim)]
    R = ot.CorrelationMatrix(dim)
    for i in range(1, dim):
        R[i, i - 1] = 0.5

    distribution = ot.Normal(meanPoint, sigma, R)

    distribution.setName("A normal distribution")
    description = ["Marginal " + str(1 + i) for i in range(dim)]
    distribution.setDescription(description)

    print("Parameters collection=", repr(distribution.getParametersCollection()))
    print("Standard representative=", distribution.getStandardRepresentative())

    print("Distribution ", repr(distribution))
    print("Distribution ", distribution)
    print("Covariance ", repr(distribution.getCovariance()))

    # Is this distribution elliptical ?
    print("Elliptical = ", distribution.isElliptical())

    # Is this distribution continuous ?
    print("Continuous = ", distribution.isContinuous())

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    print("oneRealization=", repr(oneRealization))

    # Define a point
    point = ot.Point(distribution.getDimension(), 0.5)
    print("Point= ", repr(point))

    # Show PDF and CDF of point
    eps = 1e-5

    # derivative of PDF with respect to its arguments
    DDF = distribution.computeDDF(point)
    print("ddf     =", repr(cleanPoint(DDF)))

    # PDF value
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=%.6f" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     =%.6f" % PDF)

    CF = distribution.computeCharacteristicFunction(point)
    print("characteristic function=%.6f+%.6fi" % (CF.real, CF.imag))
    LCF = distribution.computeLogCharacteristicFunction(point)
    print("log characteristic function=%.6f+%.6fi" % (LCF.real, LCF.imag))
    CDF = distribution.computeCDF(point)
    print("cdf=%.6f" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=%.6f" % CCDF)
    PDFgr = distribution.computePDFGradient(point)
    print("pdf gradient     =", repr(cleanPoint(PDFgr)))

    # quantile
    if dim < 4:
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
    standardDeviation = distribution.getStandardDeviation()
    print("standard deviation=", repr(standardDeviation))
    skewness = distribution.getSkewness()
    print("skewness=", repr(skewness))
    kurtosis = distribution.getKurtosis()
    print("kurtosis=", repr(kurtosis))
    roughness = distribution.getRoughness()
    print("roughness=%.6f" % roughness)
    covariance = distribution.getCovariance()
    print("covariance=", repr(covariance))
    parameters = distribution.getParametersCollection()
    print("parameters=", repr(parameters))

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
    print(
        "density generator derivative (FD)=%.6f"
        % cleanScalar(
            (
                distribution.computeDensityGenerator(beta + eps)
                - distribution.computeDensityGenerator(beta - eps)
            )
            / (2.0 * eps)
        )
    )
    densityGeneratorSecondDerivative = (
        distribution.computeDensityGeneratorSecondDerivative(beta)
    )
    print(
        "density generator second derivative     =%.6f"
        % densityGeneratorSecondDerivative
    )
    print(
        "density generator second derivative (FD)=%.6f"
        % cleanScalar(
            (
                distribution.computeDensityGeneratorDerivative(beta + eps)
                - distribution.computeDensityGeneratorDerivative(beta - eps)
            )
            / (2.0 * eps)
        )
    )

    # Compute the radial CDF
    radius = 2.0
    print(
        "Radial CDF(%.6f" % radius,
        ")=%.6f" % distribution.computeRadialDistributionCDF(radius),
    )

    x = 0.6
    y = [0.2] * (dim - 1)
    print("conditional PDF=%.6f" % distribution.computeConditionalPDF(x, y))
    print("conditional CDF=%.6f" % distribution.computeConditionalCDF(x, y))
    print("conditional quantile=%.6f" % distribution.computeConditionalQuantile(x, y))
    pt = ot.Point([i + 1.5 for i in range(dim)])
    print(
        "sequential conditional PDF=",
        distribution.computeSequentialConditionalPDF(point),
    )
    resCDF = distribution.computeSequentialConditionalCDF(pt)
    print("sequential conditional CDF(", pt, ")=", resCDF)
    print(
        "sequential conditional quantile(",
        resCDF,
        ")=",
        distribution.computeSequentialConditionalQuantile(resCDF),
    )

    # Extract the marginals
    for i in range(dim):
        margin = distribution.getMarginal(i)
        print("margin=", repr(margin))
        print("margin PDF=%.6f" % margin.computePDF([0.5]))
        print("margin CDF=%.6f" % margin.computeCDF([0.5]))
        print("margin quantile=", repr(margin.computeQuantile(0.95)))
        print("margin realization=", repr(margin.getRealization()))
    if dim >= 2:
        # Extract a 2-D marginal
        indices = [1, 0]
        print("indices=", repr(indices))
        margins = distribution.getMarginal(indices)
        print("margins=", repr(margins))
        print("margins PDF=%.6f" % margins.computePDF([0.5] * 2))
        print("margins CDF=%.6f" % margins.computeCDF([0.5] * 2))
        quantile = margins.computeQuantile(0.95)
        print("margins quantile=", repr(quantile))
        print("margins CDF(qantile)=%.6f" % margins.computeCDF(quantile))
        print("margins realization=", repr(margins.getRealization()))

    chol = distribution.getCholesky()
    invChol = distribution.getInverseCholesky()
    print("chol=", repr(chol))
    print("invchol=", repr(invChol))
    print("chol*t(chol)=", repr((chol * chol.transpose())))
    ott.assert_almost_equal((chol * invChol), ot.IdentityMatrix(dim))

    ot.Log.Show(ot.Log.TRACE)
    checker = ott.DistributionChecker(distribution)
    if dim > 2:
        checker.skipMoments()  # slow
        checker.skipCorrelation()  # slow
    checker.run()

# non-spd cov
dist = ot.Normal(
    [0] * 3, ot.CovarianceMatrix([[1.0, 1.0, 0.0], [1.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
)
sample = dist.getSample(10)
