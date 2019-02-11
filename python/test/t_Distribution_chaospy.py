#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import chaospy as cp
import numpy as np

# A chaospy Triangle distribution
d0 = cp.Triangle(2.0, 3.5, 4.0)
d1 = cp.Kumaraswamy(2.0, 3.0, -1.0, 4.0)
for chaospy_dist in [d0, d1]:
    np.random.seed(42)

    # create an openturns distribution
    py_dist = ot.ChaospyDistribution(chaospy_dist)
    distribution = ot.Distribution(py_dist)

    print('distribution=', distribution)
    print('realization=', distribution.getRealization())
    sample = distribution.getSample(10)
    print('sample=', sample[0:5])
    point = [2.6]*distribution.getDimension()
    print('pdf= %.6g' % distribution.computePDF(point))
    cdf = distribution.computeCDF(point)
    print('cdf= %.6g' % cdf)
    print('quantile=', distribution.computeQuantile(cdf))
    print('quantile (tail)=', distribution.computeQuantile(cdf, True))
    print('scalar quantile=%.6g' % distribution.computeScalarQuantile(cdf))
    print('scalar quantile (tail)=%.6g' %
          distribution.computeScalarQuantile(cdf, True))
    print('mean=', distribution.getMean())
    print('mean(sampling)=', sample.computeMean())
    print('std=', distribution.getStandardDeviation())
    print('std(sampling)=', sample.computeStandardDeviationPerComponent())
    print('skewness=', distribution.getSkewness())
    print('skewness(sampling)=', sample.computeSkewness())
    print('kurtosis=', distribution.getKurtosis())
    print('kurtosis(sampling)=', sample.computeKurtosis())
    print('range=', distribution.getRange())
