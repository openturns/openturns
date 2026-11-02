#! /usr/bin/env python

import math

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Print-free checks (no expout): same coverage as the former diagnostic
# output, migrated to asserts with the blessed reference values
cases = [
    {
        "distribution": ot.TruncatedNormal(1.5, 3.0, -2.0, 5.0),
        "elliptical": True,
        "realization": [3.3246],
        "ddf": [-0.0312729],
        "logpdf": -1.960925,
        "pdf": 0.140728,
        "cdf": 0.827107,
        "ccdf": 0.172893,
        "pdfgr": [0.0312729, 0.0031597, 0.0125231, -0.0125231],
        "logpdfgr": [0.222222, 0.0224525, 0.0889876, -0.0889876],
        "cdfgr": [-0.0517406, -0.0258989, -0.0153853, -0.0736023],
        "quantile": [4.48948],
        "inv_survival": [-1.48948],
        "mvi": [-1.73303, 4.73303],
        "bilateral": [-1.73303, 4.73303],
        "mean": [1.5],
        "std": [1.84222],
        "cov": ot.CovarianceMatrix(1, [3.39378]),
        "parameters": [1.5, 3.0, -2.0, 5.0],
        "std_rep": [0.0, 0.857143, -1.0, 1.0],
        "cf": [-0.010863830248557443, 0.018222189352990206],
    },
    {
        "distribution": ot.TruncatedNormal(50.0, 1.0, 3.0, 4.0),
        "elliptical": False,
        "realization": [3.99466],
        "ddf": [1.93801e-07],
        "logpdf": -19.295887,
        "pdf": None,  # underflows print precision, checked via logPDF below
        "cdf": 0.0,  # checked with absolute tolerance below
        "ccdf": 1.0,
        "pdfgr": [-1.99336e-09, 1.84427e-07, 0.0, -1.91808e-07],
        "logpdfgr": [-0.478281, 44.2509, 0.0, -46.0217],
        "cdfgr": [-4.47731e-11, 4.14344e-09, -2.95456e-19, -4.12299e-09],
        "quantile": [3.99889],
        "inv_survival": [3.93495],
        "mvi": [3.93495, 4.0],
        "bilateral": [3.91991, 3.99945],
        "mean": [3.97828],
        "std": [0.0217084],
        "cov": ot.CovarianceMatrix(1, [0.000471255]),
        "parameters": [50.0, 1.0, 3.0, 4.0],
        "std_rep": [93.0, 2.0, -1.0, 1.0],
        "cf": [0.21082122033214787, 0.9745843281298094],
    },
]
for case in cases:
    distribution = case["distribution"]
    a, b = case["parameters"][2], case["parameters"][3]

    # Is this distribution elliptical / continuous?
    assert distribution.isElliptical() == case["elliptical"]
    assert distribution.isContinuous()

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    assert a <= oneRealization[0] <= b
    ott.assert_almost_equal(oneRealization, case["realization"], 1e-4, 0.0)

    # Define a point
    point = ot.Point(distribution.getDimension(), 3.5)

    # derivative of PDF with regards its arguments
    ott.assert_almost_equal(
        distribution.computeDDF(point), case["ddf"], 1e-5, 1e-12
    )

    # PDF value
    LPDF = distribution.computeLogPDF(point)
    ott.assert_almost_equal(LPDF, case["logpdf"], 1e-5, 1e-12)
    PDF = distribution.computePDF(point)
    if case["pdf"] is None:
        ott.assert_almost_equal(PDF, math.exp(LPDF), 1e-12, 0.0)
    else:
        ott.assert_almost_equal(PDF, case["pdf"], 1e-5, 1e-12)
        ott.assert_almost_equal(PDF, math.exp(LPDF), 1e-12, 0.0)

    # CDF values
    CDF = distribution.computeCDF(point)
    CCDF = distribution.computeComplementaryCDF(point)
    ott.assert_almost_equal(CDF, case["cdf"], 1e-5, 1e-6)
    ott.assert_almost_equal(CCDF, case["ccdf"], 1e-5, 1e-12)
    ott.assert_almost_equal(CDF + CCDF, 1.0, 1e-12, 0.0)

    # derivatives with regards the parameters of the distribution
    ott.assert_almost_equal(
        distribution.computePDFGradient(point), case["pdfgr"], 1e-5, 1e-12
    )
    ott.assert_almost_equal(
        distribution.computeLogPDFGradient(point),
        case["logpdfgr"],
        1e-5,
        1e-12,
    )
    ott.assert_almost_equal(
        distribution.computeCDFGradient(point), case["cdfgr"], 1e-5, 1e-12
    )

    # quantile and survival round-trips
    quantile = distribution.computeQuantile(0.95)
    ott.assert_almost_equal(quantile, case["quantile"], 1e-5, 1e-12)
    ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-9, 0.0)
    inverseSurvival = ot.Point(
        distribution.computeInverseSurvivalFunction(0.95)
    )
    ott.assert_almost_equal(inverseSurvival, case["inv_survival"], 1e-5, 1e-12)
    ott.assert_almost_equal(
        distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-9, 0.0
    )
    # NOTE: entropy is covered by DistributionValidation below, no manual check needed

    # Confidence regions
    interval, _ = (
        distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
    )
    ott.assert_almost_equal(
        interval.getLowerBound(), [case["mvi"][0]], 1e-5, 1e-12
    )
    ott.assert_almost_equal(
        interval.getUpperBound(), [case["mvi"][1]], 1e-5, 1e-12
    )
    ott.assert_almost_equal(
        distribution.computeProbability(interval), 0.95, 1e-9, 0.0
    )
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(
        0.95
    )
    lower = interval.getLowerBound()[0]
    ott.assert_almost_equal(
        beta, distribution.computePDF([lower]), 1e-9, 1e-12
    )
    interval, _ = (
        distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
            0.95
        )
    )
    ott.assert_almost_equal(
        interval.getLowerBound(), [case["bilateral"][0]], 1e-5, 1e-12
    )
    ott.assert_almost_equal(
        interval.getUpperBound(), [case["bilateral"][1]], 1e-5, 1e-12
    )
    ott.assert_almost_equal(
        distribution.computeProbability(interval), 0.95, 1e-9, 0.0
    )
    interval, _ = (
        distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
            0.95, False
        )
    )
    ott.assert_almost_equal(interval.getLowerBound()[0], a, 0.0, 0.0)
    ott.assert_almost_equal(
        distribution.computeProbability(interval), 0.95, 1e-9, 0.0
    )
    interval, _ = (
        distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
            0.95, True
        )
    )
    ott.assert_almost_equal(interval.getUpperBound()[0], b, 0.0, 0.0)
    ott.assert_almost_equal(
        distribution.computeProbability(interval), 0.95, 1e-9, 0.0
    )

    # characteristic function
    CF = distribution.computeCharacteristicFunction(point[0])
    ott.assert_almost_equal([CF.real, CF.imag], case["cf"], 1e-9, 0.0)

    mean = distribution.getMean()
    ott.assert_almost_equal(mean, case["mean"], 1e-5, 1e-12)
    standardDeviation = distribution.getStandardDeviation()
    ott.assert_almost_equal(standardDeviation, case["std"], 1e-5, 1e-12)
    covariance = distribution.getCovariance()
    ott.assert_almost_equal(covariance, case["cov"], 1e-5, 1e-12)
    ott.assert_almost_equal(
        covariance[0, 0], standardDeviation[0] ** 2, 1e-12, 0.0
    )
    ott.assert_almost_equal(
        distribution.getParameter(), case["parameters"], 0.0, 0.0
    )
    assert (
        list(distribution.getParameterDescription())
        == ["mu", "sigma", "a", "b"]
    )
    ott.assert_almost_equal(
        distribution.getStandardRepresentative().getParameter(),
        case["std_rep"],
        1e-6,
        1e-12,
    )

    ot.Log.Show(ot.Log.TRACE)
    validation = ott.DistributionValidation(distribution)
    validation.skipPDFAtLowerBound()  # the density is the renormalized normal density at the finite lower bound
    validation.skipMinimumVolumeLevelSet()  # slow
    validation.skipMinimumVolumeInterval()  # slow 1M-sample MC proba
    validation.skipConfidenceInterval()  # slow 3x 1M-sample MC proba
    # Monte Carlo sizes: standard errors stay well below the assert tolerances
    validation.setMomentsSamplingSize(100000)
    validation.setEntropySamplingSize(100000)
    if (
        ot.Normal(distribution.getMu(), distribution.getSigma()).computeProbability(
            distribution.getRange()
        )
        < 0.25
    ):
        # when rejection becomes too costly
        validation.skipMoments()
        validation.skipCorrelation()
        validation.skipConfidenceInterval()
    validation.run()

# Issue #3024: far right-tail truncation with normal scale
t = ot.TruncatedNormal(0.0, 1.0, 10.0, 39.0)
ott.assert_almost_equal(t.computePDF(10.1), 3.6963528500427416, 1e-12)
ott.assert_almost_equal(t.computeCDF(10.1), 0.6375114502856419, 1e-12)
ott.assert_almost_equal(t.computeComplementaryCDF(10.1), 0.36248854971435807, 1e-12)
# Symmetrical left-tail case
t2 = ot.TruncatedNormal(0.0, 1.0, -39.0, -10.0)
ott.assert_almost_equal(t2.computePDF(-10.1), 3.6963528500427416, 1e-12)
ott.assert_almost_equal(t2.computeCDF(-10.1), 0.36248854971435807, 1e-12)
# quantile/cdf failure with low-probability truncation interval
t3 = ot.TruncatedNormal(-52.0658, 1.56137, -2.22475e-05, 0.1408)
ott.assert_almost_equal(t3.computeScalarQuantile(0.5), 0.030153711943197572, 1e-12)
ott.assert_almost_equal(t3.computeCDF(0.0), 0.00049999351035801, 1e-12)
# Extreme far-tail truncation: natural-scale mass underflows,
# everything goes through the log-scale normalization factor
t7 = ot.TruncatedNormal(0.0, 1.0, 20.0, 22.0)
ott.assert_almost_equal(t7.computePDF(21.0), 2.5065256268967036e-08, 1e-12)
ott.assert_almost_equal(t7.computeLogPDF(21.0), -17.501783162107387, 1e-12)
ott.assert_almost_equal(t7.computeCDF(21.0), 0.9999999988091047, 1e-12)
ott.assert_almost_equal(t7.computeCDF(t7.computeQuantile(0.5)[0]), 0.5, 1e-12)
ott.assert_almost_equal(t7.getMean()[0], 20.049753068528293, 1e-12)
ott.assert_almost_equal(t7.getCovariance()[0, 0], 0.0024632616150521815, 1e-12)
cf7 = t7.computeCharacteristicFunction(0.5)
ott.assert_almost_equal(cf7.real, -0.825028811645019, 1e-9)
ott.assert_almost_equal(cf7.imag, -0.5645458527839131, 1e-9)
# Symmetrical left-tail case
t8 = ot.TruncatedNormal(0.0, 1.0, -22.0, -20.0)
ott.assert_almost_equal(t8.computePDF(-21.0), 2.5065256268967036e-08, 1e-12)
ott.assert_almost_equal(t8.getMean()[0], -20.049753068528293, 1e-12)
ott.assert_almost_equal(t8.computeCDF(t8.computeQuantile(0.5)[0]), 0.5, 1e-12)

# Half-normal: TruncatedNormal(0, 1, 0, +inf)
t4 = ot.TruncatedNormal(0.0, 1.0, 0.0, ot.SpecFunc.Infinity)
ott.assert_almost_equal(t4.getMean()[0], 0.7978845608028654, 1e-12)
ott.assert_almost_equal(t4.getStandardDeviation()[0], 0.6028102749890869, 1e-12)
ott.assert_almost_equal(t4.getCovariance()[0, 0], 0.36338022763241865, 1e-12)
# Symmetric left-tail case: skewness should be negated
t5 = ot.TruncatedNormal(0.0, 1.0, -ot.SpecFunc.Infinity, 0.0)
ott.assert_almost_equal(t5.getStandardDeviation()[0], 0.6028102749890869, 1e-12)
ott.assert_almost_equal(t5.getSkewness()[0], -t4.getSkewness()[0], 1e-12)
# Both bounds infinite: should be standard normal
t6 = ot.TruncatedNormal(0.0, 1.0, -ot.SpecFunc.Infinity, ot.SpecFunc.Infinity)
ott.assert_almost_equal(t6.getStandardDeviation()[0], 1.0, 1e-12)
ott.assert_almost_equal(t6.getSkewness()[0], 0.0, 1e-12)
ott.assert_almost_equal(t6.getKurtosis()[0], 3.0, 1e-12)

# --- TruncatedNormal: setters recompute everything ---
tn = ot.TruncatedNormal(0.0, 1.0, -1.0, 1.0)
tn.setMu(0.5)
ott.assert_almost_equal(
    tn.computePDF([0.0]),
    ot.TruncatedNormal(0.5, 1.0, -1.0, 1.0).computePDF([0.0]),
    1e-12,
    0.0,
)
tn.setSigma(2.0)
ott.assert_almost_equal(
    tn.computePDF([0.0]),
    ot.TruncatedNormal(0.5, 2.0, -1.0, 1.0).computePDF([0.0]),
    1e-12,
    0.0,
)

# Density at the finite bounds: renormalized normal density
edge = ot.TruncatedNormal(1.5, 3.0, -2.0, 5.0)
ref = ot.Normal(1.5, 3.0)
ott.assert_almost_equal(
    edge.computePDF(-2.0), ref.computePDF(-2.0) / (ref.computeCDF(5.0) - ref.computeCDF(-2.0)), 1e-12, 0.0
)
ott.assert_almost_equal(edge.computePDF(5.0), edge.computePDF(-2.0), 1e-12, 0.0)
assert edge.computePDF(-2.0) > 0.0
ott.assert_almost_equal(edge.computeLogPDF(-2.0), math.log(edge.computePDF(-2.0)), 1e-12, 0.0)
# Right-sided DDF and inside PDF gradient at the finite lower bound
ott.assert_almost_equal(edge.computeDDF([-2.0]), [-(-2.0 - 1.5) / 9.0 * edge.computePDF(-2.0)], 1e-12, 0.0)
grad = edge.computePDFGradient([-2.0])
assert grad.norm() > 0.0
ott.assert_almost_equal(grad[2], -edge.computePDFGradient([5.0])[3], 1e-12, 0.0)
