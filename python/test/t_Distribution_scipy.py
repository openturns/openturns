#!/usr/bin/env python

import openturns as ot
import scipy.stats as st
import numpy as np

for scipy_dist in [st.uniform(-1.0, 4.0), st.johnsonsu(2.55, 2.25),
                   st.binom(10, 0.5), st.poisson(0.6)]:
    np.random.seed(42)

    # create an openturns distribution
    py_dist = ot.SciPyDistribution(scipy_dist)
    distribution = ot.Distribution(py_dist)

    print('distribution=', distribution)
    print('continuous?', distribution.isContinuous())
    print('discrete?', distribution.isDiscrete())
    print('integral?', distribution.isIntegral())
    print('realization=', distribution.getRealization())
    sample = distribution.getSample(10000)
    print('sample=', sample[0:5])
    print('pdf@0.1= %.6g' % (distribution.computePDF([0.1])))
    point = [0.0]
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
    print('std(sampling)=', sample.computeStandardDeviation())
    print('skewness=', distribution.getSkewness())
    print('skewness(sampling)=', sample.computeSkewness())
    print('kurtosis=', distribution.getKurtosis())
    print('kurtosis(sampling)=', sample.computeKurtosis())
    print('range=', distribution.getRange())
    if distribution.isDiscrete():
        print('support=', distribution.getSupport())
    parameter = distribution.getParameter()
    print('parameter=', distribution.getParameter())
    if len(parameter) > 0:
        parameter[0] = 3.5
    distribution.setParameter(parameter)
    print('parameter=', distribution.getParameter())
    print('parameterDesc=', distribution.getParameterDescription())
