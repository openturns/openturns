#! /usr/bin/env python

import math
import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Default constructor
distribution = otexp.VonMisesFisher()
assert distribution.getDimension() == 2
assert not distribution.isElliptical()
assert distribution.isContinuous()
assert not distribution.isIntegral()

# Realization
oneRealization = distribution.getRealization()
assert oneRealization.getDimension() == 2

# Point not on unit sphere should give 0 pdf
point = ot.Point([1.0, 1.0])
assert distribution.computePDF(point) == 0.0
assert distribution.computeLogPDF(point) == ot.SpecFunc.LowestScalar

badPoint = ot.Point([0.0, 0.0])
assert distribution.computePDF(badPoint) == 0.0

# Parameters
parameters = distribution.getParametersCollection()
assert len(parameters) == 1

parameterDescription = distribution.getParameterDescription()
assert parameterDescription == ["mu0", "mu1", "kappa"]

# Get mu/kappa
ott.assert_almost_equal(distribution.getMu(), [1.0, 0.0])
assert distribution.getKappa() == 1.0

# Set mu/kappa
mu = ot.Point([1.0, 1.0])
distribution.setMu(mu)
ott.assert_almost_equal(distribution.getMu(), [0.7071067811865475, 0.7071067811865475])
kappa = 5.0
distribution.setKappa(kappa)
assert distribution.getKappa() == 5.0

# PDF at the pole
pointPole = distribution.getMu()
PDFpole = distribution.computePDF(pointPole)
ott.assert_almost_equal(PDFpole, 0.867137, 1e-5, 0.0)
LPDFpole = distribution.computeLogPDF(pointPole)
ott.assert_almost_equal(LPDFpole, -0.142559, 1e-5, 0.0)

# Sample
sample = distribution.getSample(5)
assert sample.getDimension() == 2
assert sample.getSize() == 5

# Mean
mean = distribution.getMean()
ott.assert_almost_equal(mean, [0.631717, 0.631717], 1e-5, 0.0)

# Entropy
entropy = distribution.computeEntropy()
ott.assert_almost_equal(entropy, 0.675643, 1e-5, 0.0)

# Range
range_ = distribution.getRange()
ott.assert_almost_equal(range_.getLowerBound(), [-1.0, -1.0])
ott.assert_almost_equal(range_.getUpperBound(), [1.0, 1.0])

# Standard representative
standardRep = distribution.getStandardRepresentative()
ott.assert_almost_equal(standardRep, distribution)

# Parameter round trip
ott.assert_almost_equal(
    distribution.getParameter(), [0.707107, 0.707107, 5.0], 1e-5, 0.0
)
distribution.setParameter(distribution.getParameter())
ott.assert_almost_equal(
    distribution.getParameter(), [0.707107, 0.707107, 5.0], 1e-5, 0.0
)

# Set with kappa=0 (uniform on sphere)
distUniform = otexp.VonMisesFisher(ot.Point([1.0, 0.0, 0.0]), 0.0)
assert distUniform.getDimension() == 3
ott.assert_almost_equal(distUniform.getMean(), [0.0, 0.0, 0.0])
assert math.isnan(distUniform.computeEntropy())
uniPoint = ot.Point([1.0, 0.0, 0.0])
ott.assert_almost_equal(distUniform.computePDF(uniPoint), 0.079577, 1e-5, 0.0)

# Distribution validation
validation = ott.DistributionValidation(distribution)
validation.skipCDF()
validation.skipCorrelation()
validation.skipDependenceMeasures()
validation.skipGradient()
validation.skipMoments()
validation.skipDDF()
validation.skipQuantile()
validation.skipProbability()
validation.skipMinimumVolumeLevelSet()
validation.skipMinimumVolumeInterval()
validation.skipConfidenceInterval()
validation.skipInverseSurvival()
validation.skipComplementaryCDF()
validation.skipConditional()
validation.skipTransformation()
validation.skipParameters()
validation.run()
