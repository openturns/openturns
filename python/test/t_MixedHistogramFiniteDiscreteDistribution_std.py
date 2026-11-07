#! /usr/bin/env python

import math as m
import os
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

ticksCollection = [[0.0, 1.0, 3.0, 6.0], [1.0, 2.0, 3.0]]

kind = [
    ot.MixedHistogramFiniteDiscreteDistribution.CONTINUOUS,
    ot.MixedHistogramFiniteDiscreteDistribution.DISCRETE,
]
probabilityTable = [0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45]
distribution = ot.MixedHistogramFiniteDiscreteDistribution(
    ticksCollection, kind, probabilityTable
)
distribution.setDescription(["A", "B"])
assert distribution.getClassName() == "MixedHistogramFiniteDiscreteDistribution"
assert distribution.getDimension() == 2

# Get mean and covariance
ott.assert_almost_equal(distribution.getMean(), [2.6, 2.4], 1e-12, 0.0)
covarianceRef = ot.CovarianceMatrix(2)
covarianceRef[0, 0] = 3.173333333333333
covarianceRef[0, 1] = -0.106666666666666
covarianceRef[1, 1] = 0.506666666666667
ott.assert_almost_equal(distribution.getCovariance(), covarianceRef, 1e-8, 1e-8)

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [1.27055270, 2.0], 1e-6, 1e-8)

# Test for sampling: empirical moments match the theoretical ones
size = 10000
oneSample = distribution.getSample(size)
assert oneSample.getSize() == size
assert oneSample.getDimension() == distribution.getDimension()
ott.assert_almost_equal(oneSample.computeMean(), distribution.getMean(), 0.0, 5e-2)
ott.assert_almost_equal(
    oneSample.computeCovariance(), distribution.getCovariance(), 2e-1, 5e-3
)

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
ott.assert_almost_equal(point, [1.0, 1.0], 0.0, 0.0)

# PDF value
LPDF = distribution.computeLogPDF(point)
ott.assert_almost_equal(LPDF, -3.806662, 1e-6, 1e-6)
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.0222222, 1e-6, 1e-6)

# CDF value
CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.0222222, 1e-6, 1e-6)
CCDF = distribution.computeComplementaryCDF(point)
ott.assert_almost_equal(CCDF, 0.9777778, 1e-6, 1e-6)

# quantile
quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [5.625, 3.0], 1e-12, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-12, 0.0)
ott.assert_almost_equal(distribution.getMean(), [2.6, 2.4], 1e-12, 0.0)
ott.assert_almost_equal(
    distribution.getStandardDeviation(), [1.78139, 0.711805], 1e-5, 0.0
)
ott.assert_almost_equal(distribution.getSkewness(), [0.278086, -0.754198], 1e-5, 0.0)
ott.assert_almost_equal(distribution.getKurtosis(), [1.81163, 2.29778], 1e-5, 0.0)
assert (
    distribution.getStandardRepresentative().getImplementation().getClassName()
    == "MixedHistogramFiniteDiscreteDistribution"
)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipCorrelation()  # slow
validation.skipConditional()  # slow
# validation.skipMinimumVolumeLevelSet()  # slow
validation.run()


# higher dimension
dim = 8
ticksCollection = [[0.0, 1.0, 3.0, 6.0]] * dim
kind = [ot.MixedHistogramFiniteDiscreteDistribution.DISCRETE] * dim
kind[dim - 2] = ot.MixedHistogramFiniteDiscreteDistribution.CONTINUOUS
ot.RandomGenerator.SetSeed(0)
probabilityTable = ot.RandomGenerator.Generate(4 ** (dim - 1) * 3)
assert len(probabilityTable) == 4 ** (dim - 1) * 3
x = [3.0] * dim
x[dim - 2] = 5.0
distribution = ot.MixedHistogramFiniteDiscreteDistribution(
    ticksCollection, kind, probabilityTable
)
assert distribution.getDimension() == dim
pdf = distribution.computePDF(x)
cdf = distribution.computeCDF(x)
ott.assert_almost_equal(pdf, 1.0066960082001637e-05, 1e-8, 1e-12)
ott.assert_almost_equal(cdf, 0.11961497954111239, 1e-8, 1e-12)
assert m.isfinite(pdf)
assert 0.0 <= cdf <= 1.0

# test default ctor
ot.MixedHistogramFiniteDiscreteDistribution().getSample(5)

# --- MixedHistogramFiniteDiscreteDistribution: marginal masses, load rebuild ---
studyFile = "myStudyBugFixMixedHistogram.xml"
ticks = [[0.0, 1.0], [0.0, 1.0], [0.0, 1.0]]
kind = [ot.MixedHistogramFiniteDiscreteDistribution.DISCRETE] * 3
mixed = ot.MixedHistogramFiniteDiscreteDistribution(ticks, kind, [0.125] * 8)
marginal01 = mixed.getMarginal([0, 1])
totalMass = sum(marginal01.computePDF([i, j]) for i in [0.0, 1.0] for j in [0.0, 1.0])
ott.assert_almost_equal(totalMass, 1.0, 1e-12, 0.0)
study = ot.Study(studyFile)
study.add("mixed", mixed)
study.save()
study2 = ot.Study(studyFile)
study2.load()
ticksR = [[0.0, 1.0], [0.0, 1.0], [0.0, 1.0]]
mixedR = ot.MixedHistogramFiniteDiscreteDistribution(ticksR, kind, [0.125] * 8)
study2.fillObject("mixed", mixedR)
os.remove(studyFile)
assert mixedR.getRealization().getSize() == 3
assert m.isfinite(mixedR.computePDF([0.0, 0.0, 0.0]))
