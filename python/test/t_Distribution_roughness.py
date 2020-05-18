#! /usr/bin/env python
from __future__ import print_function
import openturns as ot
import math as m
import openturns.testing as ott

ot.PlatformInfo.SetNumericalPrecision(5)
ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

def compute_roughness_sampling(distribution, size=500000):
    """
    Sampling method for computing Roughness
    This allows comparing sampling & integrating methods
    """
    dimension = distribution.getDimension()
    uniformNd = ot.ComposedDistribution([ot.Uniform(0, 1) for i in range(dimension)])
    sequence = ot.SobolSequence(dimension)
    experiment = ot.LowDiscrepancyExperiment(sequence, distribution, size, False)
    sample = experiment.generate()
    pdf = distribution.computePDF(sample)
    return pdf.computeMean()[0]


class Quartic(ot.PythonDistribution):
    def __init__(self):
        super(Quartic, self).__init__(1)
        self.c = 15.0 / 16

    def computeCDF(self, x):
        u = x[0]
        if u <= -1:
            p = 0.0
        elif u >= 1:
            p = 1.0
        else:
            p = 0.5 + 15.0 / 16.0 * u \
                - 5.0 / 8.0 * pow(u, 3) \
                + 3.0 / 16.0 * pow(u, 5)
        return p

    def computePDF(self, x):
        u = x[0]
        if u < -1 or u > 1:
            y = 0.0
        else:
            y = self.c * (1 - u**2)**2
        return y

    def getRange(self):
        return ot.Interval(-1.0, 1.0)

# Using some reference values
# See https://en.wikipedia.org/wiki/Kernel_(statistics)#Kernel_functions_in_common_use
# First Normal dist with default ctor
distribution = ot.Normal()
ott.assert_almost_equal(distribution.getRoughness(),
                        0.5 /m.sqrt(m.pi))

# Dimension 2 (Fix https://github.com/openturns/openturns/issues/1485)
# Indep copula : product of integrales
distribution = ot.Normal(2)
ott.assert_almost_equal(distribution.getRoughness(),
                        compute_roughness_sampling(distribution))

# 2D Normal with scale & correlation
# This allows checking that Normal::getRoughness is well implemented
corr = ot.CorrelationMatrix(2)
corr[1, 0] = 0.3
distribution = ot.Normal([0, 0], [1, 2], corr)
ott.assert_almost_equal(distribution.getRoughness(),
                        compute_roughness_sampling(distribution))

distribution = ot.Epanechnikov()
ott.assert_almost_equal(distribution.getRoughness(), 3/5)

distribution = ot.Triangular()
ott.assert_almost_equal(distribution.getRoughness(), 2/3)

distribution = ot.Distribution(Quartic())
ott.assert_almost_equal(distribution.getRoughness(), 5/7)

# Testing Histogram ==> getSingularities
distribution = ot.HistogramFactory().buildAsHistogram(ot.Uniform(0, 1).getSample(100000))
ott.assert_almost_equal(distribution.getRoughness(), 1.0, 5e-4, 1e-5)
# Compute the roughness using width and height
width = distribution.getWidth()
height = distribution.getHeight()
roughness = sum([width[i] * height[i]**2 for i in range(len(height))])
ott.assert_almost_equal(distribution.getRoughness(), roughness)
