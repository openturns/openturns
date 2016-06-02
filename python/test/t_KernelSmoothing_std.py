#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    dim = 2
    meanPoint = NumericalPoint(dim, 1.0)
    meanPoint[0] = 0.5
    meanPoint[1] = -0.5
    sigma = NumericalPoint(dim, 1.0)
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
    kernels.add(Beta(2.0, 4.0, -1.0, 1.0))
    kernels.add(Beta(3.0, 6.0, -1.0, 1.0))
    meanExact = distribution.getMean()
    covarianceExact = distribution.getCovariance()
    for i in range(kernels.getSize()):
        kernel = kernels[i]
        print("kernel=", kernel.getName())
        smoother = KernelSmoothing(kernel)
        smoothed = smoother.build(sample)
        bw = smoother.getBandwidth()
        print("kernel bandwidth=[ %.12g" % bw[0], ",  %.12g" % bw[1], "]")
        meanSmoothed = smoothed.getMean()
        print("mean(smoothed)=[ %.12g" % meanSmoothed[0], ",  %.12g" % meanSmoothed[
              1], "] mean(exact)=[", meanExact[0], ", ", meanExact[1], "]")
        covarianceSmoothed = smoothed.getCovariance()
        print("covariance=", repr(covarianceSmoothed),
              " covariance(exact)=", repr(covarianceExact))
        # Define a point
        point = NumericalPoint(smoothed.getDimension(), 0.0)

        # Show PDF and CDF of point point
        pointPDF = smoothed.computePDF(point)
        pointCDF = smoothed.computeCDF(point)
        print("Point= ", repr(point))
        print(" pdf(smoothed)=%.6f" % pointPDF)
        print(" pdf(exact)=%.6f" % distribution.computePDF(point))
        print(" cdf(smoothed)=%.6f" % pointCDF)
        print(" cdf(exact)=%.6f" % distribution.computeCDF(point))

    # Test for boundary correction
    distributionCollection = DistributionCollection(2)
    distributionCollection[0] = Normal(0.0, 1.0)
    distributionCollection[1] = Beta(0.7, 1.6, -1.0, 2.0)
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
                point = NumericalPoint(smoothed.getDimension(), -0.9)

                # Show PDF and CDF of point point
                pointPDF = smoothed.computePDF(point)
                pointCDF = smoothed.computeCDF(point)
                print(" pdf(smoothed)=  %.12g" % pointPDF, " pdf(exact)= %.12g" %
                      distributionCollection[j].computePDF(point))
                print(" cdf(smoothed)=  %.12g" % pointCDF, " cdf(exact)= %.12g" %
                      distributionCollection[j].computeCDF(point))

    sample = Normal().getSample(5000)
    ks1 = KernelSmoothing(Normal(), True, 64).build(sample)
    ks2 = KernelSmoothing(Normal(), True, 1024).build(sample)
    ks3 = KernelSmoothing(Normal(), False).build(sample)
    point = 0.3
    print("with low  bin count, pdf=%.6f" % ks1.computePDF(point))
    print("with high bin count, pdf=%.6f" % ks2.computePDF(point))
    print("without   binning,   pdf=%.6f" % ks3.computePDF(point))

except:
    import sys
    print("t_KernelSmoothing_std.py", sys.exc_info()[0], sys.exc_info()[1])
