#! /usr/bin/env python

import math
import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object on the circle
mu = [0.0]
sigma = ot.CovarianceMatrix([[0.25]])
distribution = otexp.WrappedNormal(mu, sigma)
print("Distribution ", distribution)

# Is this distribution continuous ?
assert distribution.isContinuous()

# Dimension
assert distribution.getDimension() == 1

# The period is a real number, not truncated to an integer
period = distribution.getPeriod()
ott.assert_almost_equal(period, 2.0 * math.pi, 1e-12, 0.0)

# Mean
ott.assert_almost_equal(distribution.getMean(), [0.0], 1e-12, 0.0)

# Periodicity of the density: f(-pi) == f(+pi)
pdf_lo = distribution.computePDF([-math.pi])
pdf_hi = distribution.computePDF([math.pi])
ott.assert_almost_equal(pdf_lo, pdf_hi, 1e-12, 0.0)

# CDF on the fundamental domain [-pi, pi]
cdf_lo = distribution.computeCDF([-math.pi])
cdf_mid = distribution.computeCDF([0.0])
cdf_hi = distribution.computeCDF([math.pi])
ott.assert_almost_equal(cdf_lo, 0.0, 1e-6, 0.0)
ott.assert_almost_equal(cdf_mid, 0.5, 1e-6, 0.0)
ott.assert_almost_equal(cdf_hi, 1.0, 1e-6, 0.0)

# Realizations stay in the fundamental domain [-pi, pi]
ot.RandomGenerator.SetSeed(0)
realization = distribution.getRealization()
assert -math.pi <= realization[0] <= math.pi

# The wrapped mean is the projection of mu on the fundamental domain
dist_wrap = otexp.WrappedNormal([1.5 * math.pi], sigma)
ott.assert_almost_equal(dist_wrap.getMu(), [-0.5 * math.pi], 1e-12, 0.0)
ott.assert_almost_equal(dist_wrap.getMean(), [-0.5 * math.pi], 1e-12, 0.0)

# Sampling quality: the sample mean is close to the wrapped mean
ot.RandomGenerator.SetSeed(0)
sample = distribution.getSample(5000)
sample_mean = sample.computeMean()
ott.assert_almost_equal(sample_mean, [0.0], 0.0, 1e-2)
for i in range(10):
    assert -math.pi <= sample[i, 0] <= math.pi

# setMu / getMu round-trip
dist_setter = otexp.WrappedNormal(mu, sigma)
dist_setter.setMu([0.3])
ott.assert_almost_equal(dist_setter.getMu(), [0.3], 1e-12, 0.0)
dist_setter.setMu([2.0 * math.pi + 0.3])
ott.assert_almost_equal(dist_setter.getMu(), [0.3], 1e-12, 0.0)

# setSigma / getSigma round-trip
sigma2 = ot.CovarianceMatrix([[1.0]])
dist_setter.setSigma(sigma2)
ott.assert_almost_equal(dist_setter.getSigma()[0, 0], 1.0, 1e-12, 0.0)

# setPeriod / getPeriod round-trip
dist_setter.setPeriod(1.0)
ott.assert_almost_equal(dist_setter.getPeriod(), 1.0, 1e-12, 0.0)
with ott.assert_raises(TypeError):
    dist_setter.setPeriod(-1.0)

# Invalid period in the constructor
with ott.assert_raises(TypeError):
    otexp.WrappedNormal(mu, sigma, 0.0)

# Parameter round-trip: mu (1) + sigma row-major (1) + period (1)
param = distribution.getParameter()
ott.assert_almost_equal(param, [0.0, 0.25, 2.0 * math.pi], 1e-12, 0.0)
new_dist = otexp.WrappedNormal()
new_dist.setParameter(param)
ott.assert_almost_equal(new_dist.getParameter(), param, 1e-12, 0.0)
with ott.assert_raises(TypeError):
    distribution.setParameter([1.0, 2.0])

# getParameterDescription
desc = distribution.getParameterDescription()
assert desc.getSize() == 3

# computePDF with wrong dimension
with ott.assert_raises(TypeError):
    distribution.computePDF([1.0, 2.0])

# CDF of the torus distribution: generic integration gives a value in [0, 1]
torus = otexp.WrappedNormal([0.0, 0.0], ot.CovarianceMatrix(2))
torus_cdf = torus.computeCDF([0.0, 0.0])
assert 0.0 <= torus_cdf <= 1.0

# Large-sigma cases approach the uniform density 1/period^d on the torus
lattice_terms = ot.ResourceMap.GetAsUnsignedInteger("WrappedNormal-MaxLatticeTerms")
ot.ResourceMap.SetAsUnsignedInteger("WrappedNormal-MaxLatticeTerms", 100)
for d in (1, 2, 4):
    cov = ot.CovarianceMatrix(d)
    for i in range(d):
        cov[i, i] = 625.0
    wide = otexp.WrappedNormal([0.0] * d, cov, 2.0 * math.pi)
    uniform = 1.0 / (2.0 * math.pi) ** d
    ott.assert_almost_equal(wide.computePDF([0.0] * d), uniform, 0.0, 1e-9)
    ott.assert_almost_equal(wide.computeLogPDF([0.0] * d), math.log(uniform), 0.0, 1e-9)
ot.ResourceMap.SetAsUnsignedInteger("WrappedNormal-MaxLatticeTerms", lattice_terms)

# The direct lattice sum and the dual Fourier series must agree: this sigma
# and period select the Fourier branch for the CDF.
fourier_cdf = otexp.WrappedNormal([0.2], ot.CovarianceMatrix([[1.0]]), 1.0)
for x in [-0.5, -0.1, 0.25, 0.5]:
    cdf_ref = sum(
        ot.DistFunc.pNormal(x - 0.2 + k) - ot.DistFunc.pNormal(-0.5 - 0.2 + k)
        for k in range(-100, 101)
    )
    ott.assert_almost_equal(fourier_cdf.computeCDF([x]), cdf_ref, 1e-12, 0.0)

# A huge sigma / period ratio used to require billions of lattice terms:
# the Fourier CDF is bounded and converges to the uniform CDF.
wide_circle = otexp.WrappedNormal([0.0], ot.CovarianceMatrix([[1.0e18]]), 1.0)
ott.assert_almost_equal(wide_circle.computeCDF([0.25]), 0.75, 1e-12, 0.0)

# A needle-shaped covariance has many partial lattice vectors but few
# complete points: the node budget must stop the recursion instead of looping
# over the long direction. The diagonal covariance factorizes, so the
# concentrated direction keeps its exact density instead of being flattened
# to the uniform limit.
needle = otexp.WrappedNormal(
    [0.0, 0.0],
    ot.CovarianceMatrix([[1.0e-12, 0.0], [0.0, 1.0e12]]),
    1.0,
)
ott.assert_almost_equal(
    needle.computePDF([0.0, 0.4]), 1.0 / (1.0e-6 * math.sqrt(2.0 * math.pi)), 1e-12, 0.0
)
assert needle.computePDF([0.3, 0.4]) < 1e-300
# A huge argument overflows the Fourier phase: the density is period-periodic,
# so the argument must be reduced before summing instead of falling into the
# direct lattice loop with a non-finite sum.
ott.assert_almost_equal(
    needle.computeLogPDF([1.0e308, 0.4]), needle.computeLogPDF([0.0, 0.4]), 1e-12, 0.0
)

# Without the diagonal factorization the density cannot be recovered when
# both enumerations exceed the budget: report it instead of returning the
# uniform limit.
small, large = 1.0e-6, 1.0e6
rotated_cov = ot.CovarianceMatrix(2)
rotated_cov[0, 0] = 0.5 * (small + large)
rotated_cov[1, 1] = 0.5 * (small + large)
rotated_cov[0, 1] = 0.5 * (small - large)
ot.ResourceMap.SetAsUnsignedInteger("WrappedNormal-MaxLatticeTerms", 100)
try:
    rotated_needle = otexp.WrappedNormal([0.0, 0.0], rotated_cov, 1.0)
    with ott.assert_raises(RuntimeError):
        try:
            rotated_needle.computePDF([0.3, 0.4])
        except RuntimeError as exc:
            # A Cholesky failure would also raise: check that the intended
            # exception is reached.
            assert "not diagonal" in str(exc)
            raise
finally:
    ot.ResourceMap.SetAsUnsignedInteger("WrappedNormal-MaxLatticeTerms", lattice_terms)

# Unaffordable Gauss-Hermite grids throw instead of hanging (2^13 > budget)
big = otexp.WrappedNormal([0.0] * 13, ot.CovarianceMatrix(13))
with ott.assert_raises(RuntimeError):
    big.getCovariance()
with ott.assert_raises(RuntimeError):
    big.computeEntropy()

# Default constructor: dimension 2 on the torus
default = otexp.WrappedNormal()
assert default.getDimension() == 2
ott.assert_almost_equal(default.getPeriod(), 2.0 * math.pi, 1e-12, 0.0)

# Entropy: flat case sigma^2 = 9 is close to the uniform entropy log(2 pi)
flat = otexp.WrappedNormal([0.0], ot.CovarianceMatrix([[9.0]]))
ott.assert_almost_equal(flat.computeEntropy(), 1.8377423, 1e-3, 0.0)
# Peaked case sigma^2 = 0.25 matches the unwrapped Gaussian entropy
peaked = otexp.WrappedNormal([0.0], ot.CovarianceMatrix([[0.25]]))
ott.assert_almost_equal(
    peaked.computeEntropy(),
    0.5 * (1.0 + math.log(2.0 * math.pi)) + 0.5 * math.log(0.25),
    1e-6,
    0.0,
)

# Covariance of the wrapped variable: tangent sigma when peaked,
# uniform variance pi^2/3 in the flat limit (sigma^2 = 9 is close to uniform
# up to the finite-concentration bias)
ott.assert_almost_equal(peaked.getCovariance()[0, 0], 0.25, 1e-6, 0.0)
ott.assert_almost_equal(flat.getCovariance()[0, 0], math.pi**2 / 3.0, 5e-2, 0.0)

# High dimension: ellipsoidal lattice sums avoid the premature uniform fallback
d = 8
cov8 = ot.CovarianceMatrix(d)
for i in range(d):
    cov8[i, i] = 9.0
wide8 = otexp.WrappedNormal([0.0] * d, cov8, 2.0 * math.pi)
uniform8 = 1.0 / (2.0 * math.pi) ** d
ott.assert_almost_equal(wide8.computePDF([0.0] * d) / uniform8, 1.19219768, 1e-6, 0.0)

# Persistence through the Study mechanism regenerates the cache
if ot.PlatformInfo.HasFeature("libxml2"):
    file = "study_wrapped_normal.xml"
    study = ot.Study(file)
    study.add("distribution", distribution)
    study.save()
    study = ot.Study(file)
    study.load()
    loaded = otexp.WrappedNormal()
    study.fillObject("distribution", loaded)
    assert loaded == distribution
    ott.assert_almost_equal(
        loaded.computePDF([0.0]), distribution.computePDF([0.0]), 1e-12, 0.0
    )
    assert -math.pi <= loaded.getRealization()[0] <= math.pi
    os.remove(file)

# Clone via Distribution wrapper
dist = ot.Distribution(distribution)
assert dist.getImplementation().getClassName() == "WrappedNormal"

# Distribution validation
ot.RandomGenerator.SetSeed(0)
validation = ott.DistributionValidation(distribution)
validation.skipPDFAtLowerBound()
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

# __repr__ and __str__
assert "WrappedNormal" in repr(distribution)
assert "WrappedNormal" in str(distribution)
