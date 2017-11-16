#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import scipy as sp
import scipy.stats as st

for scipy_dist in [st.uniform(), st.johnsonsu(2.55, 2.25)]:
    sp.random.seed(42)

    # create an openturns distribution
    py_dist = ot.SciPyDistribution(scipy_dist)
    distribution = ot.Distribution(py_dist)

    print('distribution=', distribution)
    print('realization=', distribution.getRealization())
    sample = distribution.getSample(10000)
    print('sample=', sample[0:5])
    point = [0.6]
    print('pdf= %.6g' % distribution.computePDF(point))
    cdf = distribution.computeCDF(point)
    print('cdf= %.6g' % cdf)
    print('quantile=', distribution.computeQuantile(cdf))
    print('mean=', distribution.getMean())
    print('mean(sampling)=', sample.computeMean())
    print('std=', distribution.getStandardDeviation())
    print('std(sampling)=', sample.computeStandardDeviationPerComponent())
    print('skewness=', distribution.getSkewness())
    print('skewness(sampling)=', sample.computeSkewness())
    print('kurtosis=', distribution.getKurtosis())
    print('kurtosis(sampling)=', sample.computeKurtosis())
    print('range=', distribution.getRange())
    parameter = distribution.getParameter()
    print('parameter=', distribution.getParameter())
    if len(parameter) > 0:
        parameter[0] = 3.5
    distribution.setParameter(parameter)
    print('parameter=', distribution.getParameter())
    print('parameterDesc=', distribution.getParameterDescription())
