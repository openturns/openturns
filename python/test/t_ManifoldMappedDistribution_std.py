#! /usr/bin/env python

import math
import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Manifold mapped distribution on the 2-sphere charted at the north pole
manifold = otexp.Manifold(otexp.SphereManifold(3))
background = ot.Normal(2)
chartCenter = [0.0, 0.0, 1.0]
distribution = otexp.ManifoldMappedDistribution(background, manifold, chartCenter)
print("Distribution ", repr(distribution))
print("Distribution ", distribution)
assert distribution.getDimension() == 3
assert distribution.isContinuous()
assert not distribution.isDiscrete()

# Accessors
assert distribution.getBackground() == background
assert distribution.getManifold() == manifold
ott.assert_almost_equal(distribution.getChartCenter(), chartCenter, 1e-12, 1e-12)
assert distribution.getTangentBasis().getSize() == 2

# PDF at the chart center: standard normal density at 0 divided by density 1
ott.assert_almost_equal(distribution.computePDF(chartCenter), 1.0 / (2.0 * math.pi), 1e-12, 1e-12)

# PDF at angle theta from the pole: normal density corrected by sin(theta)/theta
theta = 0.5
point = [math.sin(theta), 0.0, math.cos(theta)]
expected = math.exp(-0.5 * theta * theta) / (2.0 * math.pi) * theta / math.sin(theta)
ott.assert_almost_equal(distribution.computePDF(point), expected, 1e-10, 1e-10)

# Log PDF consistency with PDF
ott.assert_almost_equal(distribution.computeLogPDF(point), math.log(distribution.computePDF(point)), 1e-10, 1e-10)

# At the cut locus (antipode) the density is singular, PDF is zero
ott.assert_almost_equal(distribution.computePDF([0.0, 0.0, -1.0]), 0.0, 1e-12, 1e-12)

# Realizations lie on the sphere
sample = distribution.getSample(100)
for i in range(100):
    ott.assert_almost_equal([sample[i].norm()], [1.0], 1e-10, 1e-10)

# The mean points along the chart center by symmetry, with
# E[cos(R)] = 0.2752... for a Rayleigh(1) radius R
mean = distribution.getMean()
print("mean=", mean)
ott.assert_almost_equal(mean[0], 0.0, 1e-1, 1e-1)
ott.assert_almost_equal(mean[1], 0.0, 1e-1, 1e-1)
ott.assert_almost_equal(mean[2], 0.2752, 1e-1, 2e-2)

# Parameters are delegated to the background distribution
assert distribution.getParameterDescription() == background.getParameterDescription()
ott.assert_almost_equal(distribution.getParameter(), background.getParameter(), 1e-12, 1e-12)

# Background dimension must match the intrinsic dimension
with ott.assert_raises(TypeError):
    otexp.ManifoldMappedDistribution(ot.Normal(3), manifold, chartCenter)

# Moving the chart center updates the tangent basis
distribution.setChartCenter([0.0, 1.0, 0.0])
ott.assert_almost_equal(distribution.getChartCenter(), [0.0, 1.0, 0.0], 1e-12, 1e-12)
ott.assert_almost_equal(distribution.computePDF([0.0, 1.0, 0.0]), 1.0 / (2.0 * math.pi), 1e-12, 1e-12)

# SO(3) case: background Normal(3) charted at the identity
rotations = otexp.Manifold(otexp.SO3Manifold())
identity = [1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0]
rotationDistribution = otexp.ManifoldMappedDistribution(ot.Normal(3), rotations, identity)
assert rotationDistribution.getDimension() == 9
ott.assert_almost_equal(rotationDistribution.computePDF(identity), (2.0 * math.pi) ** (-1.5), 1e-12, 1e-12)
oneRealization = rotationDistribution.getRealization()
assert rotations.isOnManifold(oneRealization)

# Persistence through the Study mechanism
if ot.PlatformInfo.HasFeature("libxml2"):
    file = "study_manifold_mapped.xml"
    study = ot.Study()
    study.setStorageManager(ot.XMLStorageManager(file))
    study.add("distribution", distribution)
    study.save()

    study = ot.Study()
    study.setStorageManager(ot.XMLStorageManager(file))
    study.load()
    loaded = otexp.ManifoldMappedDistribution()
    study.fillObject("distribution", loaded)
    assert loaded == distribution
    os.remove(file)
