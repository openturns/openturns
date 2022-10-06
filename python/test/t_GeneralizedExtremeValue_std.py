#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

all_cases = [ot.GeneralizedExtremeValue(2.0, 1.5, -0.15),
             ot.GeneralizedExtremeValue(2.0, 1.5,  0.0),
             ot.GeneralizedExtremeValue(2.0, 1.5,  0.15)]
for i in range(len(all_cases)):
    distribution = all_cases[i]
    print("#" * 50)
    print("Distribution ", distribution)

    for dist in ['WeibullMax', 'Frechet', 'Gumbel']:
        try:
            eval("print('conversion as ', distribution.as" + dist + "())")
        except:
            pass

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
    size = 100
    for i in range(2):
        print(
            "Kolmogorov test for the generator, sample size=", size, " is ",
            ot.FittingTest.Kolmogorov(distribution.getSample(size), distribution).getBinaryQualityMeasure())
        size *= size

    # Define a point
    point = [1.0] * distribution.getDimension()
    print("Point= ", point)

    # Show PDF and CDF of point
    eps = 1e-5
    DDF = distribution.computeDDF(point)
    print("ddf     =", DDF)
    print(
        "ddf (FD)=", ot.Point(1, (distribution.computePDF(point[0] + eps) - distribution.computePDF(point[0] - eps)) / (2.0 * eps)))
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=", ot.Point(1, LPDF))
    PDF = distribution.computePDF(point)
    print("pdf     =", ot.Point(1, PDF))
    print(
        "pdf (FD)=", ot.Point(1, (distribution.computeCDF(point[0] + eps) - distribution.computeCDF(point[0] - eps)) / (2.0 * eps)))
    CDF = distribution.computeCDF(point)
    print("cdf=", ot.Point(1, CDF))
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=", ot.Point(1, CCDF))
    Survival = distribution.computeSurvivalFunction(point)
    print("survival=", ot.Point(1, Survival))
    InverseSurvival = distribution.computeInverseSurvivalFunction(0.95)
    print("Inverse survival=", InverseSurvival)
    print("Survival(inverse survival)=",
          ot.Point(1, distribution.computeSurvivalFunction(InverseSurvival)))
    PDFgr = distribution.computePDFGradient(point)
    print("pdf gradient     =", PDFgr)
    PDFgrFD = ot.Point(3)
    PDFgrFD[0] = (ot.GeneralizedExtremeValue(distribution.getMu() + eps, distribution.getSigma(), distribution.getXi()).computePDF(point) -
                  ot.GeneralizedExtremeValue(distribution.getMu() - eps, distribution.getSigma(), distribution.getXi()).computePDF(point)) / (2.0 * eps)
    PDFgrFD[1] = (ot.GeneralizedExtremeValue(distribution.getMu(), distribution.getSigma() + eps, distribution.getXi()).computePDF(point) -
                  ot.GeneralizedExtremeValue(distribution.getMu(), distribution.getSigma() - eps, distribution.getXi()).computePDF(point)) / (2.0 * eps)
    PDFgrFD[2] = (ot.GeneralizedExtremeValue(distribution.getMu(), distribution.getSigma(), distribution.getXi() + eps).computePDF(point) -
                  ot.GeneralizedExtremeValue(distribution.getMu(), distribution.getSigma(), distribution.getXi() - eps).computePDF(point)) / (2.0 * eps)
    print("pdf gradient (FD)=", PDFgrFD)
    CDFgr = distribution.computeCDFGradient(point)
    print("cdf gradient     =", CDFgr)
    CDFgrFD = ot.Point(3)
    CDFgrFD[0] = (ot.GeneralizedExtremeValue(distribution.getMu() + eps, distribution.getSigma(), distribution.getXi()).computeCDF(point) -
                  ot.GeneralizedExtremeValue(distribution.getMu() - eps, distribution.getSigma(), distribution.getXi()).computeCDF(point)) / (2.0 * eps)
    CDFgrFD[1] = (ot.GeneralizedExtremeValue(distribution.getMu(), distribution.getSigma() + eps, distribution.getXi()).computeCDF(point) -
                  ot.GeneralizedExtremeValue(distribution.getMu(), distribution.getSigma() - eps, distribution.getXi()).computeCDF(point)) / (2.0 * eps)
    CDFgrFD[2] = (ot.GeneralizedExtremeValue(distribution.getMu(), distribution.getSigma(), distribution.getXi() + eps).computeCDF(point) -
                  ot.GeneralizedExtremeValue(distribution.getMu(), distribution.getSigma(), distribution.getXi() - eps).computeCDF(point)) / (2.0 * eps)
    print("cdf gradient (FD)=", CDFgrFD)
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", quantile)
    print("cdf(quantile)=", distribution.computeCDF(quantile))
    print("entropy=%.6f" % distribution.computeEntropy())
    # Confidence regions
    prob, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
        0.95)
    print("Minimum volume interval=", prob)
    print("threshold=", threshold)
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(
        0.95)
    print("Minimum volume level set=", levelSet)
    print("beta=", ot.Point(1, beta))
    interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
        0.95)
    print("Bilateral confidence interval=", interval)
    print("beta=", ot.Point(1, beta))
    interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False)
    print("Unilateral confidence interval (lower tail)=", interval)
    print("beta=", ot.Point(1, beta))
    interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True)
    print("Unilateral confidence interval (upper tail)=", interval)
    print("beta=", ot.Point(1, beta))
    mean = distribution.getMean()
    print("mean=", mean)
    standardDeviation = distribution.getStandardDeviation()
    print("standard deviation=", standardDeviation)
    skewness = distribution.getSkewness()
    print("skewness=", skewness)
    kurtosis = distribution.getKurtosis()
    print("kurtosis=", kurtosis)
    covariance = distribution.getCovariance()
    print("covariance=", covariance)
    correlation = distribution.getCorrelation()
    print("correlation=", correlation)
    spearman = distribution.getSpearmanCorrelation()
    print("spearman=", spearman)
    kendall = distribution.getKendallTau()
    print("kendall=", kendall)
    parameters = distribution.getParametersCollection()
    print("parameters=", parameters)
    print("Standard representative=",
          distribution.getStandardRepresentative())
    print("mu=", distribution.getMu())
    print("sigma=", distribution.getSigma())
    print("xi=", distribution.getXi())
    print("Actual distribution=", distribution.getActualDistribution())
    distribution.setActualDistribution(
        distribution.getActualDistribution())
    print("Distribution from actual distribution=", distribution)
