#! /usr/bin/env python

import openturns as ot
import scipy as sp
import scipy.stats as st

try:
    # create a scipy distribution
    sp.random.seed(42)
    scipy_dist = st.uniform()

    # create an openturns distribution
    py_dist = ot.SciPyDistribution(scipy_dist)
    distribution = ot.Distribution(py_dist)

    print(('distribution=%s' % str(distribution)))
    print(('realization=%s' % str(distribution.getRealization())))
    print(('sample=%s' % str(distribution.getSample(5))))
    point = ot.Point(1, 0.6)
    print(('pdf=%s' % str(distribution.computePDF(point))))
    print(('cdf=%s' % str(distribution.computeCDF(point))))
    print(('mean=%s' % str(distribution.getMean())))
    print(('std=%s' % str(distribution.getStandardDeviation())))
    print(('skewness=%s' % str(distribution.getSkewness())))
    print(('kurtosis=%s' % str(distribution.getKurtosis())))
    print(('range=%s' % str(distribution.getRange())))

    # create an openturns random vector
    py_rv = ot.SciPyRandomVector(scipy_dist)
    randomVector = ot.RandomVector(py_rv)
    print(('randomVector=%s' % str(randomVector)))
    print(('realization=%s' % str(randomVector.getRealization())))
    print(('sample=%s' % str(randomVector.getSample(5))))
    print(('mean=%s' % str(randomVector.getMean())))
    print(('covariance=%s' % str(randomVector.getCovariance())))


except:
    import sys
    import traceback
    traceback.print_exc(file=sys.stdout)
