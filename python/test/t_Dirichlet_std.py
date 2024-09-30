#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
for dim in range(1, 2):
    theta = ot.Point(dim + 1)
    for i in range(dim + 1):
        theta[i] = (i + 1.0) / 4.0
    distribution = ot.Dirichlet(theta)
    description = [""] * dim
    for j in range(1, dim + 1):
        oss = "Marginal " + str(j)
        description[j - 1] = oss
    distribution.setDescription(description)
    print("Distribution ", repr(distribution))
    print("Distribution ", distribution)

    # Is this distribution elliptical ?
    print("Elliptical = ", distribution.isElliptical())

    # Is this distribution continuous ?
    print("Continuous = ", distribution.isContinuous())

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    print("oneRealization=", repr(oneRealization))

    # Define a point
    point = ot.Point(distribution.getDimension(), 0.5 / distribution.getDimension())
    print("Point= ", repr(point))

    # Show PDF and CDF of point
    LPDF = distribution.computeLogPDF(point)
    print("log pdf= %.8g" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     = %.8g" % PDF)
    CDF = distribution.computeCDF(point)
    print("cdf= %.8g" % CDF)
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", repr(quantile))
    print("cdf(quantile)= %.6f" % distribution.computeCDF(quantile))
    # Get 95% survival function
    inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
    print("InverseSurvival=", repr(inverseSurvival))
    print(
        "Survival(inverseSurvival)=%.6f"
        % distribution.computeSurvivalFunction(inverseSurvival)
    )
    print("entropy=%.6f" % distribution.computeEntropy())

    # Confidence regions
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
    covariance = distribution.getCovariance()
    print("covariance=", repr(covariance))

    x = 0.6
    y = [0.2] * (dim - 1)
    print("conditional PDF=%.6f" % distribution.computeConditionalPDF(x, y))
    print("conditional CDF=%.6f" % distribution.computeConditionalCDF(x, y))
    print("conditional quantile=%.6f" % distribution.computeConditionalQuantile(x, y))
    pt = ot.Point([0.1 * i + 0.15 for i in range(dim)])
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

    # Ticket #2306
    if dim == 2:
        condPDF = distribution.computeConditionalPDF(-0.1, [0.5])
        assert ot.SpecFunc.IsNormal(condPDF), "condPDF is not normal"
        condPDF = distribution.computeConditionalPDF(0.5, [0.5])
        assert ot.SpecFunc.IsNormal(condPDF), "condPDF is not normal"
        condPDF = distribution.computeConditionalPDF(0.6, [0.5])
        assert ot.SpecFunc.IsNormal(condPDF), "condPDF is not normal"
        condPDF = distribution.computeSequentialConditionalPDF([0.5, -0.1])
        assert ot.SpecFunc.IsNormal(condPDF), "condPDF is not normal"
        condPDF = distribution.computeSequentialConditionalPDF([0.5, 0.5])
        assert ot.SpecFunc.IsNormal(condPDF), "condPDF is not normal"
        condPDF = distribution.computeSequentialConditionalPDF([0.5, 0.6])
        assert ot.SpecFunc.IsNormal(condPDF), "condPDF is not normal"

    # Extract the marginals
    for i in range(dim):
        margin = distribution.getMarginal(i)
        print("margin=", margin)
        print("margin PDF= %.8g" % margin.computePDF(ot.Point(1, 0.5)))
        print("margin CDF= %.8g" % margin.computeCDF(ot.Point(1, 0.5)))
        print("margin quantile=", repr(margin.computeQuantile(0.95)))
        print("margin realization=", repr(margin.getRealization()))
    if dim >= 2:
        # Extract a 2-D marginal
        indices = [1, 0]
        print("indices=", indices)
        margins = distribution.getMarginal(indices)
        print("margins=", margins)
        print("margins PDF=", margins.computePDF(ot.Point(2, 0.5)))
        print("margins CDF= %.8g" % margins.computeCDF(ot.Point(2, 0.5)))
        quantile = margins.computeQuantile(0.95)
        print("margins quantile=", repr(quantile))
        print("margins CDF(quantile)= %.6f" % margins.computeCDF(quantile))
        print("margins realization=", repr(margins.getRealization()))

    ot.Log.Show(ot.Log.TRACE)
    checker = ott.DistributionChecker(distribution)
    checker.skipCDF()
    checker.skipGradient()
    checker.skipMoments()
    checker.run()
