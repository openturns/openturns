#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Instantiate one distribution object
dim = 2
meanPoint = [0.5, -0.5]
sigma = [2.0, 3.0]
R = ot.CorrelationMatrix(dim)
for i in range(1, dim):
    R[i, i - 1] = 0.5

distribution = ot.Normal(meanPoint, sigma, R)
discretization = 100
kernel = ot.KernelSmoothing()
sample = distribution.getSample(discretization)
kernels = ot.DistributionCollection(0)
kernels.add(ot.Normal())
kernels.add(ot.Epanechnikov())
kernels.add(ot.Uniform())
kernels.add(ot.Triangular())
kernels.add(ot.Logistic())
kernels.add(ot.Beta(2.0, 2.0, -1.0, 1.0))
kernels.add(ot.Beta(3.0, 3.0, -1.0, 1.0))
meanExact = distribution.getMean()
covarianceExact = distribution.getCovariance()
for i in range(kernels.getSize()):
    kernel = kernels[i]
    print("kernel=", kernel.getName())
    smoother = ot.KernelSmoothing(kernel)
    smoothed = smoother.build(sample)
    bw = smoother.getBandwidth()
    print("kernel bandwidth=[ %.6g" % bw[0], ",  %.6g" % bw[1], "]")
    meanSmoothed = smoothed.getMean()
    print(
        "mean(smoothed)=[ %.6g" % meanSmoothed[0],
        ",  %.6g" % meanSmoothed[1],
        "] mean(exact)=[",
        meanExact[0],
        ", ",
        meanExact[1],
        "]",
    )
    covarianceSmoothed = smoothed.getCovariance()
    print(
        "covariance=",
        repr(covarianceSmoothed),
        " covariance(exact)=",
        repr(covarianceExact),
    )
    # Define a point
    point = ot.Point(smoothed.getDimension(), 0.0)

    # Show PDF and CDF of point point
    pointPDF = smoothed.computePDF(point)
    pointCDF = smoothed.computeCDF(point)
    print("Point= ", repr(point))
    print(" pdf(smoothed)=%.6g" % pointPDF)
    print(" pdf(exact)=%.6g" % distribution.computePDF(point))
    print(" cdf(smoothed)=%.6g" % pointCDF)
    print(" cdf(exact)=%.6g" % distribution.computeCDF(point))

# Test for boundary correction
distributionCollection = ot.DistributionCollection(2)
distributionCollection[0] = ot.Normal(0.0, 1.0)
distributionCollection[1] = ot.Beta(0.7, 0.9, -1.0, 2.0)
sampleCollection = [
    distributionCollection[0].getSample(discretization),
    distributionCollection[1].getSample(discretization),
]
for i in range(kernels.getSize()):
    kernel = kernels[i]
    print("kernel=", kernel.getName())
    smoother = ot.KernelSmoothing(kernel)
    for j in range(2):
        for corr in [False, True]:
            smoother.setBoundaryCorrection(corr)
            smoothed = smoother.build(sampleCollection[j])
            print(
                "Bounded underlying distribution? ",
                j == 1,
                " bounded reconstruction? ",
                corr,
            )
            # Define a point
            point = ot.Point(smoothed.getDimension(), -0.9)

            # Show PDF and CDF of point point
            pointPDF = smoothed.computePDF(point)
            pointCDF = smoothed.computeCDF(point)
            print(
                " pdf(smoothed)=  %.6g" % pointPDF,
                " pdf(exact)= %.6g" % distributionCollection[j].computePDF(point),
            )
            print(
                " cdf(smoothed)=  %.6g" % pointCDF,
                " cdf(exact)= %.6g" % distributionCollection[j].computeCDF(point),
            )

sample = ot.Normal().getSample(5000)
ks1 = ot.KernelSmoothing(ot.Normal(), True, 64).build(sample)
ks2 = ot.KernelSmoothing(ot.Normal(), True, 1024).build(sample)
ks3 = ot.KernelSmoothing(ot.Normal(), False).build(sample)
point = 0.3
print("with low  bin count, pdf=%.6g" % ks1.computePDF(point))
print("with high bin count, pdf=%.6g" % ks2.computePDF(point))
print("without   binning,   pdf=%.6g" % ks3.computePDF(point))
# Test with varying boundary corrections
coll = [ot.Uniform(-1.0, 1.0), ot.Uniform(0.0, 2.0)]
left = [-0.9, 0.1]
right = [0.9, 1.9]
for nDist in range(len(coll)):
    baseDist = coll[nDist]
    sample = baseDist.getSample(500)
    algo1 = ot.KernelSmoothing(ot.Normal(), False)
    algo1.setBoundingOption(ot.KernelSmoothing.NONE)
    ks1 = algo1.build(sample)
    print(
        "with no boundary correction, pdf(left)=%.6g" % ks1.computePDF(left[nDist]),
        ", pdf(right)=%.6g" % ks1.computePDF(right[nDist]),
    )

    algo2 = ot.KernelSmoothing(ot.Normal(), False)
    algo2.setBoundingOption(ot.KernelSmoothing.LOWER)
    algo2.setAutomaticLowerBound(True)
    ks2 = algo2.build(sample)
    print(
        "with automatic lower boundary correction, pdf(left)=%.6g"
        % ks2.computePDF(left[nDist]),
        ", pdf(right)=%.6g" % ks2.computePDF(right[nDist]),
    )

    algo3 = ot.KernelSmoothing(ot.Normal(), False)
    algo3.setBoundingOption(ot.KernelSmoothing.LOWER)
    algo3.setLowerBound(baseDist.getRange().getLowerBound()[0])
    algo3.setAutomaticLowerBound(False)
    ks3 = algo3.build(sample)
    print(
        "with user defined lower boundary correction, pdf(left)=%.6g"
        % ks3.computePDF(left[nDist]),
        ", pdf(right)=%.6g" % ks3.computePDF(right[nDist]),
    )

    algo4 = ot.KernelSmoothing(ot.Normal(), False)
    algo4.setBoundingOption(ot.KernelSmoothing.UPPER)
    algo4.setAutomaticUpperBound(True)
    ks4 = algo4.build(sample)
    print(
        "with automatic upper boundary correction, pdf(left)=%.6g"
        % ks4.computePDF(left[nDist]),
        ", pdf(right)=%.6g" % ks4.computePDF(right[nDist]),
    )

    algo5 = ot.KernelSmoothing(ot.Normal(), False)
    algo5.setBoundingOption(ot.KernelSmoothing.UPPER)
    algo5.setUpperBound(baseDist.getRange().getUpperBound()[0])
    algo5.setAutomaticLowerBound(False)
    ks5 = algo5.build(sample)
    print(
        "with user defined upper boundary correction, pdf(left)=%.6g"
        % ks5.computePDF(left[nDist]),
        ", pdf(right)=%.6g" % ks5.computePDF(right[nDist]),
    )

    algo6 = ot.KernelSmoothing(ot.Normal(), False)
    algo6.setBoundingOption(ot.KernelSmoothing.BOTH)
    ks6 = algo6.build(sample)
    print(
        "with automatic boundaries correction, pdf(left)=%.6g"
        % ks6.computePDF(left[nDist]),
        ", pdf(right)=%.6g" % ks6.computePDF(right[nDist]),
    )

    algo7 = ot.KernelSmoothing(ot.Normal(), False)
    algo7.setBoundingOption(ot.KernelSmoothing.BOTH)
    algo7.setLowerBound(baseDist.getRange().getLowerBound()[0])
    ks7 = algo7.build(sample)
    print(
        "with user defined lower/automatic upper boundaries correction, pdf(left)=%.6g"
        % ks7.computePDF(left[nDist]),
        ", pdf(right)=%.6g" % ks7.computePDF(right[nDist]),
    )

    algo8 = ot.KernelSmoothing(ot.Normal(), False)
    algo8.setBoundingOption(ot.KernelSmoothing.BOTH)
    algo8.setUpperBound(baseDist.getRange().getUpperBound()[0])
    ks8 = algo8.build(sample)
    print(
        "with automatic lower/user defined upper boundaries correction, pdf(left)=%.6g"
        % ks8.computePDF(left[nDist]),
        ", pdf(right)=%.6g" % ks8.computePDF(right[nDist]),
    )

    algo9 = ot.KernelSmoothing(ot.Normal(), False)
    algo9.setBoundingOption(ot.KernelSmoothing.BOTH)
    algo9.setLowerBound(baseDist.getRange().getLowerBound()[0])
    algo9.setUpperBound(baseDist.getRange().getUpperBound()[0])
    ks9 = algo9.build(sample)
    print(
        "with user defined boundaries correction, pdf(left)=%.6g"
        % ks9.computePDF(left[nDist]),
        ", pdf(right)=%.6g" % ks9.computePDF(right[nDist]),
    )

# full degenerate case
sample = ot.JointDistribution(
    [ot.Dirac(-7.0), ot.Dirac(0.0), ot.Dirac(8.0)]
).getSample(50)
smoothed = ot.KernelSmoothing().build(sample)
print(smoothed.getSample(3))

# n-d degenerate case
sample = ot.JointDistribution(
    [ot.Dirac(-7.0), ot.Arcsine(2.0, 3.0), ot.Dirac(8.0)]
).getSample(50)
sample.setDescription(["d7", "a23", "d8"])
smoothed = ot.KernelSmoothing().build(sample)
print(smoothed.getSample(3))

# Test with reduced Cutoff - generates non positive phiGammaH
distribution = ot.Normal()
kernel = ot.Normal()
factory = ot.KernelSmoothing(kernel)
ot.ResourceMap.SetAsScalar("KernelSmoothing-CutOffPlugin", 3.0)
ot.RandomGenerator.SetSeed(8457)
sample = distribution.getSample(30)
h = factory.computePluginBandwidth(sample)[0]
print("with reduced cutoff. h=%.6g" % (h))
