#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    dim = 2
    meanPoint = Point(dim, 1.0)
    meanPoint[0] = 0.5
    meanPoint[1] = -0.5
    sigma = Point(dim, 1.0)
    sigma[0] = 2.0
    sigma[1] = 3.0
    R = CorrelationMatrix(dim)
    for i in range(1, dim):
        R[i, i - 1] = 0.5

    distribution = Normal(meanPoint, sigma, R)
    discretization = 100
    kernel = KernelSmoothing()
    sample = distribution.getSample(discretization)
    kernels = DistributionCollection(0)
    kernels.add(Normal())
    kernels.add(Epanechnikov())
    kernels.add(Uniform())
    kernels.add(Triangular())
    kernels.add(Logistic())
    kernels.add(Beta(2.0, 2.0, -1.0, 1.0))
    kernels.add(Beta(3.0, 3.0, -1.0, 1.0))
    meanExact = distribution.getMean()
    covarianceExact = distribution.getCovariance()
    for i in range(kernels.getSize()):
        kernel = kernels[i]
        print("kernel=", kernel.getName())
        smoother = KernelSmoothing(kernel)
        smoothed = smoother.build(sample)
        bw = smoother.getBandwidth()
        print("kernel bandwidth=[ %.6g" % bw[0], ",  %.6g" % bw[1], "]")
        meanSmoothed = smoothed.getMean()
        print("mean(smoothed)=[ %.6g" % meanSmoothed[0], ",  %.6g" % meanSmoothed[
              1], "] mean(exact)=[", meanExact[0], ", ", meanExact[1], "]")
        covarianceSmoothed = smoothed.getCovariance()
        print("covariance=", repr(covarianceSmoothed),
              " covariance(exact)=", repr(covarianceExact))
        # Define a point
        point = Point(smoothed.getDimension(), 0.0)

        # Show PDF and CDF of point point
        pointPDF = smoothed.computePDF(point)
        pointCDF = smoothed.computeCDF(point)
        print("Point= ", repr(point))
        print(" pdf(smoothed)=%.6g" % pointPDF)
        print(" pdf(exact)=%.6g" % distribution.computePDF(point))
        print(" cdf(smoothed)=%.6g" % pointCDF)
        print(" cdf(exact)=%.6g" % distribution.computeCDF(point))

    # Test for boundary correction
    distributionCollection = DistributionCollection(2)
    distributionCollection[0] = Normal(0.0, 1.0)
    distributionCollection[1] = Beta(0.7, 0.9, -1.0, 2.0)
    sampleCollection = [distributionCollection[0].getSample(
        discretization), distributionCollection[1].getSample(discretization)]
    for i in range(kernels.getSize()):
        kernel = kernels[i]
        print("kernel=", kernel.getName())
        smoother = KernelSmoothing(kernel)
        for j in range(2):
            for corr in [False, True]:
                smoother.setBoundaryCorrection(corr)
                smoothed = smoother.build(sampleCollection[j])
                print("Bounded underlying distribution? ", j ==
                      1, " bounded reconstruction? ", corr)
                # Define a point
                point = Point(smoothed.getDimension(), -0.9)

                # Show PDF and CDF of point point
                pointPDF = smoothed.computePDF(point)
                pointCDF = smoothed.computeCDF(point)
                print(" pdf(smoothed)=  %.6g" % pointPDF, " pdf(exact)= %.6g" %
                      distributionCollection[j].computePDF(point))
                print(" cdf(smoothed)=  %.6g" % pointCDF, " cdf(exact)= %.6g" %
                      distributionCollection[j].computeCDF(point))

    sample = Normal().getSample(5000)
    ks1 = KernelSmoothing(Normal(), True, 64).build(sample)
    ks2 = KernelSmoothing(Normal(), True, 1024).build(sample)
    ks3 = KernelSmoothing(Normal(), False).build(sample)
    point = 0.3
    print("with low  bin count, pdf=%.6g" % ks1.computePDF(point))
    print("with high bin count, pdf=%.6g" % ks2.computePDF(point))
    print("without   binning,   pdf=%.6g" % ks3.computePDF(point))
    # Test with varying boundary corrections
    left = [-0.9]
    right = [0.9]
    sample = Uniform().getSample(500)
    algo1 = KernelSmoothing(Normal(), False)
    algo1.setBoundingOption(KernelSmoothing.NONE)
    ks1 = algo1.build(sample)
    print("with no boundary correction, pdf(left)=%.6g" %
          ks1.computePDF(left), ", pdf(right)=%.6g" % ks1.computePDF(right))

    algo2 = KernelSmoothing(Normal(), False)
    algo2.setBoundingOption(KernelSmoothing.LOWER)
    algo2.setAutomaticLowerBound(True)
    ks2 = algo2.build(sample)
    print("with automatic lower boundary correction, pdf(left)=%.6g" %
          ks2.computePDF(left), ", pdf(right)=%.6g" % ks2.computePDF(right))

    algo3 = KernelSmoothing(Normal(), False)
    algo3.setBoundingOption(KernelSmoothing.LOWER)
    algo3.setLowerBound(-1.0)
    algo3.setAutomaticLowerBound(False)
    ks3 = algo3.build(sample)
    print("with user defined lower boundary correction, pdf(left)=%.6g" %
          ks3.computePDF(left), ", pdf(right)=%.6g" % ks3.computePDF(right))

    algo4 = KernelSmoothing(Normal(), False)
    algo4.setBoundingOption(KernelSmoothing.UPPER)
    algo4.setAutomaticUpperBound(True)
    ks4 = algo4.build(sample)
    print("with automatic upper boundary correction, pdf(left)=%.6g" %
          ks4.computePDF(left), ", pdf(right)=%.6g" % ks4.computePDF(right))

    algo5 = KernelSmoothing(Normal(), False)
    algo5.setBoundingOption(KernelSmoothing.UPPER)
    algo5.setUpperBound(1.0)
    algo5.setAutomaticLowerBound(False)
    ks5 = algo5.build(sample)
    print("with user defined upper boundary correction, pdf(left)=%.6g" %
          ks5.computePDF(left), ", pdf(right)=%.6g" % ks5.computePDF(right))

    algo6 = KernelSmoothing(Normal(), False)
    algo6.setBoundingOption(KernelSmoothing.BOTH)
    ks6 = algo6.build(sample)
    print("with automatic boundaries correction, pdf(left)=%.6g" %
          ks6.computePDF(left), ", pdf(right)=%.6g" % ks6.computePDF(right))

    algo7 = KernelSmoothing(Normal(), False)
    algo7.setBoundingOption(KernelSmoothing.BOTH)
    algo7.setLowerBound(-1.0)
    ks7 = algo7.build(sample)
    print("with user defined lower/automatic upper boundaries correction, pdf(left)=%.6g" %
          ks7.computePDF(left), ", pdf(right)=%.6g" % ks7.computePDF(right))

    algo8 = KernelSmoothing(Normal(), False)
    algo8.setBoundingOption(KernelSmoothing.BOTH)
    algo8.setUpperBound(1.0)
    ks8 = algo8.build(sample)
    print("with automatic lower/user defined upper boundaries correction, pdf(left)=%.6g" %
          ks8.computePDF(left), ", pdf(right)=%.6g" % ks8.computePDF(right))

    algo9 = KernelSmoothing(Normal(), False)
    algo9.setBoundingOption(KernelSmoothing.BOTH)
    algo9.setLowerBound(-1.0)
    algo9.setUpperBound(1.0)
    ks9 = algo9.build(sample)
    print("with user defined boundaries correction, pdf(left)=%.6g" %
          ks9.computePDF(left), ", pdf(right)=%.6g" % ks9.computePDF(right))

except:
    import sys
    print("t_KernelSmoothing_std.py", sys.exc_info()[0], sys.exc_info()[1])
