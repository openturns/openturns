#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()

# Instantiate one distribution object
for dim in range(1, 2):
    theta = ot.Point(dim + 1)
    for i in range(dim + 1):
        theta[i] = (i + 1.0) / 4.0
    distribution = ot.Dirichlet(theta)
    description = [""] * dim
    for j in range(1, dim + 1):
        oss = "Marginal " + str(j)
        description[j - 1] = oss
    distribution.setDescription(description)
    assert distribution.getClassName() == "Dirichlet"

    # Is this distribution elliptical ?
    assert not distribution.isElliptical()

    # Is this distribution continuous ?
    assert distribution.isContinuous()

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    ott.assert_almost_equal(oneRealization, [0.168034], 1e-6, 0.0)

    # Define a point
    point = ot.Point(distribution.getDimension(), 0.5 / distribution.getDimension())
    ott.assert_almost_equal(point, [0.5], 0.0, 0.0)

    # Show PDF and CDF of point
    LPDF = distribution.computeLogPDF(point)
    ott.assert_almost_equal(LPDF, -0.79067254, 1e-7, 0.0)
    PDF = distribution.computePDF(point)
    ott.assert_almost_equal(PDF, 0.45353967, 1e-7, 0.0)
    CDF = distribution.computeCDF(point)
    ott.assert_almost_equal(CDF, 0.6831129, 1e-7, 0.0)
    quantile = distribution.computeQuantile(0.95)
    ott.assert_almost_equal(quantile, [0.982959], 1e-6, 0.0)
    ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-6, 0.0)
    # Get 95% survival function
    inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
    ott.assert_almost_equal(inverseSurvival, [1.8464e-05], 1e-6, 1e-12)
    ott.assert_almost_equal(
        distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 0.0
    )
    ott.assert_almost_equal(distribution.computeEntropy(), -1.137913, 1e-6, 0.0)

    # Confidence regions
    (
        interval,
        threshold,
    ) = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
    ott.assert_almost_equal(interval.getLowerBound(), [0.0], 0.0, 0.0)
    ott.assert_almost_equal(interval.getUpperBound(), [0.982959], 1e-6, 0.0)
    ott.assert_almost_equal(threshold, 0.95, 1e-12, 0.0)
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
    ott.assert_almost_equal(beta, 0.445853, 1e-6, 0.0)
    (
        interval,
        beta,
    ) = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
    ott.assert_almost_equal(interval.getLowerBound(), [1.1540077688761149e-06], 1e-12, 0.0)
    ott.assert_almost_equal(interval.getUpperBound(), [0.995712], 1e-6, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-12, 0.0)
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False
    )
    ott.assert_almost_equal(interval.getLowerBound(), [0.0], 0.0, 0.0)
    ott.assert_almost_equal(interval.getUpperBound(), [0.982959], 1e-6, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-12, 0.0)
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True
    )
    ott.assert_almost_equal(interval.getLowerBound(), [1.8464e-05], 1e-6, 1e-12)
    ott.assert_almost_equal(interval.getUpperBound(), [1.0], 0.0, 0.0)
    ott.assert_almost_equal(beta, 0.95, 1e-12, 0.0)

    mean = distribution.getMean()
    ott.assert_almost_equal(mean, [1.0 / 3.0], 1e-12, 0.0)
    standardDeviation = distribution.getStandardDeviation()
    ott.assert_almost_equal(standardDeviation, [0.356348], 1e-6, 0.0)
    skewness = distribution.getSkewness()
    ott.assert_almost_equal(skewness, [0.680301], 1e-6, 0.0)
    kurtosis = distribution.getKurtosis()
    ott.assert_almost_equal(kurtosis, [1.90909], 1e-5, 0.0)
    covariance = distribution.getCovariance()
    ott.assert_almost_equal(
        covariance, ot.CovarianceMatrix(1, [0.12698412698412698]), 1e-12, 0.0
    )

    x = 0.6
    y = [0.2] * (dim - 1)
    ott.assert_almost_equal(distribution.computeConditionalPDF(x, y), 0.442267, 1e-6, 0.0)
    ott.assert_almost_equal(distribution.computeConditionalCDF(x, y), 0.727715, 1e-6, 0.0)
    ott.assert_almost_equal(
        distribution.computeConditionalQuantile(x, y), 0.329133, 1e-6, 0.0
    )
    pt = ot.Point([0.1 * i + 0.15 for i in range(dim)])
    ott.assert_almost_equal(
        distribution.computeSequentialConditionalPDF(point), [0.45354], 1e-5, 0.0
    )
    resCDF = distribution.computeSequentialConditionalCDF(pt)
    ott.assert_almost_equal(resCDF, [0.482299], 1e-6, 0.0)
    ott.assert_almost_equal(
        distribution.computeSequentialConditionalQuantile(resCDF), [0.15], 1e-12, 0.0
    )

    # Ticket #2306
    if dim == 2:
        condPDF = distribution.computeConditionalPDF(-0.1, [0.5])
        assert m.isfinite(condPDF), "condPDF is not normal"
        condPDF = distribution.computeConditionalPDF(0.5, [0.5])
        assert m.isfinite(condPDF), "condPDF is not normal"
        condPDF = distribution.computeConditionalPDF(0.6, [0.5])
        assert m.isfinite(condPDF), "condPDF is not normal"
        condPDF = distribution.computeSequentialConditionalPDF([0.5, -0.1])
        assert m.isfinite(condPDF), "condPDF is not normal"
        condPDF = distribution.computeSequentialConditionalPDF([0.5, 0.5])
        assert m.isfinite(condPDF), "condPDF is not normal"
        condPDF = distribution.computeSequentialConditionalPDF([0.5, 0.6])
        assert m.isfinite(condPDF), "condPDF is not normal"

    # Extract the marginals
    for i in range(dim):
        margin = distribution.getMarginal(i)
        ott.assert_almost_equal(margin.computePDF(ot.Point(1, 0.5)), 0.45353967, 1e-7, 0.0)
        ott.assert_almost_equal(margin.computeCDF(ot.Point(1, 0.5)), 0.6831129, 1e-7, 0.0)
        ott.assert_almost_equal(margin.computeQuantile(0.95), [0.982959], 1e-6, 0.0)
        ott.assert_almost_equal(margin.getRealization(), [0.2406935108379952], 1e-12, 0.0)
    if dim >= 2:
        # Extract a 2-D marginal
        indices = [1, 0]
        margins = distribution.getMarginal(indices)
        ott.assert_almost_equal(margins.computePDF(ot.Point(2, 0.5)), 0.45353967, 1e-7, 0.0)
        ott.assert_almost_equal(margins.computeCDF(ot.Point(2, 0.5)), 0.6831129, 1e-7, 0.0)
        quantile = margins.computeQuantile(0.95)
        ott.assert_almost_equal(quantile, [0.982959], 1e-6, 0.0)
        ott.assert_almost_equal(margins.computeCDF(quantile), 0.95, 1e-6, 0.0)

    ot.Log.Show(ot.Log.TRACE)
    validation = ott.DistributionValidation(distribution)
    validation.skipCDF()  # 1D Dirichlet is degenerate (constant 1)
    validation.skipGradient()  # 1D Dirichlet is degenerate (constant 1)
    validation.skipMoments()  # 1D Dirichlet is degenerate (constant 1)
    validation.setMomentsSamplingSize(100000)
    validation.setDomainSamplingSize(100000)
    validation.setEntropySamplingSize(100000)
    validation.run()

# Use a small integration size to keep the test fast
defaultIntegrationSize = ot.ResourceMap.GetAsUnsignedInteger(
    "Dirichlet-DefaultIntegrationSize"
)
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-DefaultIntegrationSize", 6)
# check Dirichlet computeCDF has no global sample cache
theta1 = [2.0, 6.0, 3.0, 4.0, 5.0]
d1 = ot.Dirichlet(theta1)
theta2 = [10.0, 30.0, 15.0, 20.0, 25.0]
d2 = ot.Dirichlet(theta2)
point = [0.15] * 4
cdf1 = d1.computeCDF(point)
cdf2 = d2.computeCDF(point)
assert abs(cdf1 - cdf2) > 1e-8, "Dirichlet CDF should differ for different theta"

# --- Extended coverage: multi-dimensional paths, edge cases and setters ---
d1d = ot.Dirichlet([0.25, 0.5])
assert d1d.computeCDF([0.0]) == 0.0
assert d1d.computeCDF([1.0]) == 1.0
with ott.assert_raises(Exception):
    d1d.computePDF([0.5, 0.5])
with ott.assert_raises(Exception):
    d1d.computeLogPDF([0.5, 0.5])
with ott.assert_raises(Exception):
    d1d.computeCDF([0.5, 0.5])
with ott.assert_raises(Exception):
    d1d.computeQuantile(-0.1)

# ctor / setTheta errors
with ott.assert_raises(Exception):
    ot.Dirichlet([1.0])
with ott.assert_raises(Exception):
    ot.Dirichlet([1.0, -0.5])
with ott.assert_raises(Exception):
    ot.Dirichlet([1.0, 0.0])
d = ot.Dirichlet([1.0, 2.0, 3.0])
with ott.assert_raises(Exception):
    d.setTheta([1.0])
with ott.assert_raises(Exception):
    d.setTheta([1.0, -2.0])
with ott.assert_raises(Exception):
    d.setParametersCollection([])
with ott.assert_raises(Exception):
    d.setParametersCollection([ot.Point(1, 1.0)])
# setParameter round-trip, preserves description
d.setDescription(["a", "b"])
d.setParameter([2.0, 3.0, 4.0])
ott.assert_almost_equal(d.getParameter(), [2.0, 3.0, 4.0])
assert d.getDescription() == ["a", "b"]
with ott.assert_raises(Exception):
    d.setParameter([1.0])
with ott.assert_raises(Exception):
    d.setParameter([1.0, -1.0, 2.0])
assert list(d.getParameter()) == [2.0, 3.0, 4.0]
assert d.getParameterDescription() == ["theta_0", "theta_1", "theta_2"]
assert len(d.getParametersCollection()) == 2

for theta in ([1.0, 2.0, 3.0], [2.0, 6.0, 3.0, 4.0], [2.0, 6.0, 3.0, 4.0, 5.0]):
    dd = ot.Dirichlet(theta)
    dim = dd.getDimension()
    dd.getMean()
    dd.getCovariance()
    dd.getStandardDeviation()
    dd.getSkewness()
    dd.getKurtosis()
    dd.computeEntropy()
    dd.getRealization()
    assert dd.hasIndependentCopula() == (dim == 1)
    assert dd.hasEllipticalCopula() == (dim == 1)
    if dim == 2:
        dd.getSpearmanCorrelation()
        dd.getKendallTau()
    dd.getParametersCollection()
    # PDF edges: wrong dimension, non-positive component, sum >= 1
    with ott.assert_raises(Exception):
        dd.computePDF(ot.Point(dim + 1, 0.1))
    assert dd.computePDF(ot.Point(dim, -0.1)) == 0.0
    assert dd.computePDF(ot.Point(dim, 0.9)) == 0.0
    # CDF branches: negative component -> 0, all >= 1 -> 1
    assert dd.computeCDF(ot.Point(dim, -0.1)) == 0.0
    assert dd.computeCDF(ot.Point(dim, 2.0)) == 1.0
    # inside simplex -> Gauss integration
    dd.computeCDF(ot.Point(dim, 0.1))
    # first sorted-sum branch (dim > 1): e.g. [0.6, 0.6, ...]
    dd.computeCDF(ot.Point(dim, 0.6))
    # marginals: 1D Beta and multi-D Dirichlet, invalid indices
    dd.getMarginal(0)
    with ott.assert_raises(Exception):
        dd.getMarginal([dim])
    if dim >= 2:
        dd.getMarginal([1, 0])
        # second sorted-sum branch (dim > 2): [0.4, 0.4, ...]
        dd.computeCDF(ot.Point(dim, 0.4))
    # conditionals: error paths and out-of-simplex early returns
    with ott.assert_raises(Exception):
        dd.computeConditionalPDF(0.5, ot.Point(dim, 0.1))
    with ott.assert_raises(Exception):
        dd.computeConditionalCDF(0.5, ot.Point(dim, 0.1))
    with ott.assert_raises(Exception):
        dd.computeConditionalQuantile(0.5, ot.Point(dim, 0.1))
    with ott.assert_raises(Exception):
        dd.computeConditionalQuantile(0.5, ot.Point(0, 0.0))
    with ott.assert_raises(Exception):
        dd.computeConditionalQuantile(-0.1, ot.Point(0, 0.0))
    with ott.assert_raises(Exception):
        dd.computeConditionalQuantile(0.5, [-0.1])
    with ott.assert_raises(Exception):
        dd.computeSequentialConditionalPDF(ot.Point(dim + 1, 0.1))
    with ott.assert_raises(Exception):
        dd.computeSequentialConditionalCDF(ot.Point(dim + 1, 0.1))
    with ott.assert_raises(Exception):
        dd.computeSequentialConditionalQuantile(ot.Point(dim + 1, 0.1))
    assert dd.computeConditionalPDF(0.5, [2.0]) == 0.0
    assert dd.computeConditionalPDF(0.5, [-0.5]) == 0.0
    assert dd.computeConditionalCDF(0.5, [-0.5]) == 0.0
    assert dd.computeConditionalCDF(0.5, [2.0]) == 1.0
    dd.computeConditionalPDF(0.2, [0.1] * (dim - 1) if dim > 1 else [])
    dd.computeConditionalCDF(0.2, [0.1] * (dim - 1) if dim > 1 else [])
    dd.computeConditionalQuantile(0.5, [0.1] * (dim - 1) if dim > 1 else [])
    pt = ot.Point(dim, 0.15)
    dd.computeSequentialConditionalPDF(pt)
    dd.computeSequentialConditionalCDF(pt)
    dd.computeSequentialConditionalQuantile(ot.Point(dim, 0.5))

# Sequential conditional CDF: a conditioning vector that exhausts the
# simplex sets the remaining coordinates to one, while a negative
# coordinate leaves them at zero.
sequentialCDF = ot.Dirichlet([2.0, 3.0, 4.0, 5.0]).computeSequentialConditionalCDF(
    [0.7, 0.7, 0.5]
)
assert sequentialCDF[2] == 1.0
assert sequentialCDF[0] < 1.0
assert list(
    ot.Dirichlet([2.0, 3.0, 4.0, 5.0]).computeSequentialConditionalCDF(
        [-0.1, 0.5, 0.5]
    )
) == [0.0, 0.0, 0.0]

# Density on the closed simplex: the boundary value follows the exponents
uniform1 = ot.Dirichlet([1.0, 1.0])
ott.assert_almost_equal(uniform1.computePDF([0.0]), 1.0, 1e-12, 0.0)
ott.assert_almost_equal(uniform1.computePDF([1.0]), 1.0, 1e-12, 0.0)
ott.assert_almost_equal(uniform1.computeLogPDF([0.0]), 0.0, 1e-12, 0.0)
ott.assert_almost_equal(uniform1.computeLogPDF([1.0]), 0.0, 1e-12, 0.0)
uniform2 = ot.Dirichlet([1.0, 1.0, 1.0])
ott.assert_almost_equal(uniform2.computePDF([0.0, 0.5]), 2.0, 1e-12, 0.0)
ott.assert_almost_equal(uniform2.computePDF([0.2, 0.8]), 2.0, 1e-12, 0.0)
ott.assert_almost_equal(uniform2.computeLogPDF([0.2, 0.8]), m.log(2.0), 1e-12, 0.0)

vanishing = ot.Dirichlet([2.0, 1.0, 1.0])
assert vanishing.computePDF([0.0, 0.5]) == 0.0
assert vanishing.computeLogPDF([0.0, 0.5]) == -m.inf
vanishingSlack = ot.Dirichlet([1.0, 1.0, 2.0])
assert vanishingSlack.computePDF([0.2, 0.8]) == 0.0
assert vanishingSlack.computeLogPDF([0.2, 0.8]) == -m.inf

finiteSlack = ot.Dirichlet([2.0, 1.0])
ott.assert_almost_equal(finiteSlack.computePDF([1.0]), 2.0, 1e-12, 0.0)
ott.assert_almost_equal(finiteSlack.computeLogPDF([1.0]), m.log(2.0), 1e-12, 0.0)

# Singular exponents keep the zero convention of the Beta distribution
singular = ot.Dirichlet([0.5, 1.0, 1.0])
assert singular.computePDF([0.0, 0.5]) == 0.0
assert singular.computeLogPDF([0.0, 0.5]) == -m.inf
singularSlack = ot.Dirichlet([1.0, 1.0, 0.5])
assert singularSlack.computePDF([0.2, 0.8]) == 0.0
assert singularSlack.computeLogPDF([0.2, 0.8]) == -m.inf

# CDF outside the simplex (dim >= 4)
dmc = ot.Dirichlet([2.0, 6.0, 3.0, 4.0, 5.0])
mcCDF = dmc.computeCDF([0.3, 0.3, 0.3, 0.3])
assert 0.0 <= mcCDF <= 1.0

# --- Dirichlet: standard deviation, sequential conditionals ---
ott.assert_almost_equal(
    ot.Dirichlet([1.0, 2.0, 3.0]).getStandardDeviation(),
    [0.14085904245475275, 0.17817416127494959],
    1e-12,
    0.0,
)
seq = ot.Dirichlet([1.0, 1.0, 1.0]).computeSequentialConditionalPDF([0.5, 0.3])
assert all(m.isfinite(v) for v in seq)

# A zero integration size is clamped to a valid quadrature order
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-DefaultIntegrationSize", 0)
assert ot.Dirichlet([2.0, 3.0, 4.0]).computeCDF([0.2, 0.3]) > 0.0

ot.ResourceMap.SetAsUnsignedInteger(
    "Dirichlet-DefaultIntegrationSize", defaultIntegrationSize
)

# --- CDF dispatch: reduced coordinates, exact series and numerical guards ---
# Coordinates above one are integrated out and the last active coordinate
# reduces to a beta CDF.
mixedReduction = ot.Dirichlet([2.0, 3.0, 4.0])
ott.assert_almost_equal(
    mixedReduction.computeCDF([0.3, 1.0]),
    ot.DistFunc.pBeta(2.0, 7.0, 0.3),
    1e-12,
    0.0,
)
# The uniform Dirichlet has the closed-form CDF 2 x1 x2.
ott.assert_almost_equal(
    ot.Dirichlet([1.0, 1.0, 1.0]).computeCDF([0.2, 0.3]), 0.12, 1e-12, 0.0
)
# A marginal CDF that underflows to zero makes the whole CDF vanish.
assert ot.Dirichlet([1000.0, 1000.0, 1000.0]).computeCDF([1e-300, 1e-300]) == 0.0

# Large shape parameters exercise the log-scaled box quadrature.
logBoxCDF = ot.Dirichlet([1000.0, 1000.0, 1000.0]).computeCDF([0.3, 0.3])
assert 0.0 < logBoxCDF < 1.0, "log-scaled box quadrature returned an invalid CDF"

# In the upper corner the complement is evaluated by inclusion-exclusion.
upperCDF = ot.Dirichlet([1000.0, 1000.0, 1000.0]).computeCDF([0.37, 0.37])
assert 0.999 < upperCDF <= 1.0, "upper-corner complement returned an invalid CDF"

# The adaptive box quadrature reports a failure when the order limit is
# reached, and the dispatcher then falls back to another method.
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-DefaultIntegrationSize", 3)
assert 0.0 < ot.Dirichlet([2.0, 3.0, 4.5]).computeCDF([0.1, 0.1]) < 1.0
# A singular upper corner in dimension seven uses the lowest quadrature order.
assert 0.0 <= ot.Dirichlet([0.5] + [100.0] * 6).computeCDF([0.2] * 6) <= 1.0
# The same corner in dimension eight needs the box dimension raised.
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumBoxDimension", 8)
assert 0.0 <= ot.Dirichlet([0.5] + [100.0] * 8).computeCDF([0.1] * 7 + [0.5]) <= 1.0
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumBoxDimension", 7)
ot.ResourceMap.SetAsUnsignedInteger(
    "Dirichlet-DefaultIntegrationSize", defaultIntegrationSize
)
# A zero tensor-product budget disables the box quadrature.
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumTensorNodes", 0)
assert 0.0 < ot.Dirichlet([2.0, 3.0, 4.0]).computeCDF([0.1, 0.1]) < 1.0
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumTensorNodes", 20000000)

# The finite beta-integer series is skipped above its exponent limit.
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumIntegerBeta", 2)
assert 0.0 < ot.Dirichlet([2.0, 2.0, 5.0]).computeCDF([0.2, 0.2]) < 1.0
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumIntegerBeta", 128)
# A near-boundary point makes the convolution below the representable range.
assert (
    0.0
    <= ot.Dirichlet([2.0, 2.0, 128.0]).computeCDF([1.0 - 1.0e-15, 1.0e-16])
    <= 1.0
)

# The Exton series is rejected for large beta, and the upper-corner
# quadrature rejects shapes above one million.
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumBoxDimension", 0)
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumQuadratureDimension", 0)
assert 0.0 < ot.Dirichlet([2.0, 2.0, 300.0]).computeCDF([0.01, 0.01]) < 1.0
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumBoxDimension", 7)
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumQuadratureDimension", 10)
assert (
    0.0
    <= ot.Dirichlet([2.0, 2.0, 2.0, 1000001.0]).computeCDF([0.6, 0.3, 0.5])
    <= 1.0
)

# Normal approximation: large shapes, with the other methods disabled.
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumBoxDimension", 0)
ot.ResourceMap.SetAsScalar("Dirichlet-CDFExtonMaximumCancellation", 0.0)
ot.ResourceMap.SetAsScalar("Dirichlet-CDFUpperMaximumCancellation", 0.0)
ot.ResourceMap.SetAsScalar("Dirichlet-CDFNormalMinimumTotal", 0.0)
ot.ResourceMap.SetAsScalar("Dirichlet-CDFNormalMinimumShape", 0.0)
normalCDF = ot.Dirichlet([0.5, 100000.0, 100000.0]).computeCDF([1.0e-3, 0.5])
assert 0.0 <= normalCDF <= 1.0
normalCDF = ot.Dirichlet([0.5, 100000.0, 100000.0]).computeCDF([0.9, 0.5])
assert 0.0 <= normalCDF <= 1.0
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumBoxDimension", 7)
ot.ResourceMap.SetAsScalar("Dirichlet-CDFExtonMaximumCancellation", 1.0e4)
ot.ResourceMap.SetAsScalar("Dirichlet-CDFUpperMaximumCancellation", 1.0e13)
ot.ResourceMap.SetAsScalar("Dirichlet-CDFNormalMinimumTotal", 1.0e5)
ot.ResourceMap.SetAsScalar("Dirichlet-CDFNormalMinimumShape", 100.0)

# Inside and outside the simplex, force all deterministic methods to fail
# so that the Frechet midpoint warning path is exercised.
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumBoxDimension", 0)
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumIntegerBeta", 0)
ot.ResourceMap.SetAsScalar("Dirichlet-CDFExtonMaximumCancellation", 0.0)
ot.ResourceMap.SetAsScalar("Dirichlet-CDFUpperMaximumCancellation", 0.0)
assert 0.0 <= ot.Dirichlet([2.0] * 7).computeCDF([0.1] * 6) <= 1.0
assert 0.0 <= ot.Dirichlet([2.0] * 7).computeCDF([0.2] * 6) <= 1.0
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumBoxDimension", 7)
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumIntegerBeta", 128)
ot.ResourceMap.SetAsScalar("Dirichlet-CDFExtonMaximumCancellation", 1.0e4)
ot.ResourceMap.SetAsScalar("Dirichlet-CDFUpperMaximumCancellation", 1.0e13)

# Above the singular box dimension the upper-corner quadrature is disabled.
assert 0.0 <= ot.Dirichlet([0.5] * 9).computeCDF([0.2] * 8) <= 1.0
# Singular and regular shapes explore the corner quadrature orders.
assert 0.0 <= ot.Dirichlet([0.5] * 8).computeCDF([0.15] * 6 + [0.5]) <= 1.0
assert 0.0 <= ot.Dirichlet([2.0] * 8).computeCDF([0.15] * 6 + [0.5]) <= 1.0

# Positive recursive integration, including an integration cut induced by
# a point outside the simplex.
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumBoxDimension", 0)
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumQuadratureDimension", 0)
for theta, point in [
    ([2.0, 3.0, 4.0, 5.0], [0.4, 0.4, 0.4]),
    ([5.0, 2.0, 1.0, 1.0], [0.6, 0.6, 0.1]),
    ([2.0, 3.0, 4.0, 5.0, 6.0], [0.4, 0.4, 0.4, 0.4]),
]:
    recursiveCDF = ot.Dirichlet(theta).computeCDF(point)
    assert 0.0 <= recursiveCDF <= 1.0
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumBoxDimension", 7)
ot.ResourceMap.SetAsUnsignedInteger("Dirichlet-CDFMaximumQuadratureDimension", 10)

# Marginal of a one-dimensional Dirichlet returns a clone
assert ot.Dirichlet([0.25, 0.5]).getMarginal(0).getDimension() == 1
# Sequential quantile beyond the support stops at the first coordinate
seqQuantile = ot.Dirichlet([1.0, 1.0, 1.0]).computeSequentialConditionalQuantile(
    [1.0, 0.5]
)
assert seqQuantile[0] == 1.0

# Roughness: analytic value B(2*theta - 1) / B(theta)^2 with multinomial Beta
theta = [2.0, 3.0, 4.0]
ott.assert_almost_equal(
    ot.Dirichlet(theta).getRoughness(),
    m.exp(
        m.lgamma(3.0)
        + m.lgamma(5.0)
        + m.lgamma(7.0)
        - m.lgamma(15.0)
        - 2.0
        * (
            m.lgamma(2.0)
            + m.lgamma(3.0)
            + m.lgamma(4.0)
            - m.lgamma(9.0)
        )
    ),
)
# The L2 norm of the PDF diverges when a concentration is <= 0.5
with ott.assert_raises(Exception):
    ot.Dirichlet([0.5, 0.5, 0.5]).getRoughness()
