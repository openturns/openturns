#! /usr/bin/env python

# %%
import openturns as ot
import openturns.testing as ott
import os

# %%
ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.Binomial(15, 0.7)
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", repr(oneRealization))

# Define a point
point = ot.Point(distribution.getDimension(), 5.0)
print("Point= ", repr(point))

# PDF value
PDF = distribution.computePDF(point)
print("pdf     =%.6f" % PDF)

# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
print("cdf=%.6f" % CDF)
# quantile
quantile = distribution.computeQuantile(0.95)
print("quantile=", repr(quantile))
print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
print("entropy=%.6f" % distribution.computeEntropy())
mean = distribution.getMean()
print("mean=", repr(mean))
standardDeviation = distribution.getStandardDeviation()
print("standard deviation=", repr(standardDeviation))
skewness = distribution.getSkewness()
print("skewness=", repr(skewness))
kurtosis = distribution.getKurtosis()
print("kurtosis=", repr(kurtosis))
covariance = distribution.getCovariance()
print("covariance=", repr(covariance))
parameters = distribution.getParametersCollection()
print("parameters=", repr(parameters))
print("Standard representative=", distribution.getStandardRepresentative())
# Confidence interval
alpha = 0.05
bounds = distribution.computeBilateralConfidenceInterval(1 - alpha)
print("%.2f%% bilateral confidence interval" % ((1 - alpha) * 100), " =", bounds)

# %%
# check survival at upper bound
distribution = ot.Binomial(10, 1.0)
assert distribution.computeCDF(10.0) == 1.0
assert distribution.computeComplementaryCDF(10.0) == 0.0
assert distribution.computeSurvivalFunction(10.0) == 0.0

# negative quantile bug
distribution = ot.Binomial(3, 0.5)
assert distribution.computeScalarQuantile(0.9, True) == 0

# %%
print("Check on dataset")
separator = ","
skipped_lines = 13
path = os.path.join(os.path.dirname(__file__), "t_Binomial_std.csv")
sample = ot.Sample.ImportFromTextFile(path, separator, skipped_lines)
rtol = 1.e-12
atol = ot.SpecFunc.MinScalar
sample_size = sample.getSize()
for i in range(sample_size):
    x, n, pr, expected_pdf, expected_cdfp, expected_cdfq = sample[i]
    x = int(x)
    n = int(n)
    distribution = ot.Binomial(n, pr)
    computed_p = distribution.computePDF(x)
    computed_cdf = distribution.computeCDF(x)
    computed_ccdf = distribution.computeComplementaryCDF(x)
    print(f"row = {i}/{sample_size}, x = {x}, n = {n}, pr = {pr}, "
          f"P(X = x) = {expected_pdf}, CDF = {expected_cdfp}, Compl. CDF = {expected_cdfq}")
    # Check PDF
    print(f"    computePDF. Computed = {computed_p:.11e}, expected = {expected_pdf:.11e}")
    ott.assert_almost_equal(computed_p, expected_pdf, rtol, atol)
    # Check CDF
    print(f"    computeCDF. Computed = {computed_cdf:.11e}, expected = {expected_cdfp:.11e}")
    ott.assert_almost_equal(computed_cdf, expected_cdfp, rtol, atol)
    # Check complementary CDF
    print(f"    computeComplementaryCDF. Computed = {computed_ccdf:.11e}, expected = {expected_cdfq:.11e}")
    ott.assert_almost_equal(computed_ccdf, expected_cdfq, rtol, atol)
    # Check quantile
    if pr == 0.0 and x < n:
        # The function is not invertible
        # for this particular input.
        continue
    elif expected_cdfp < expected_cdfq:
        computed_x = int(distribution.computeQuantile(computed_cdf)[0])
        cdfXM1 = distribution.computeCDF(computed_x - 1)
        cdfX = distribution.computeCDF(computed_x)
        print(f"    computeQuantile (A). Computed X = {computed_x}, F(X - 1) = {cdfXM1:.7e}, F(X) = {cdfX:.7e}")
        if computed_cdf == 0.0:
            assert computed_x == 0.0
        else:
            assert cdfXM1 < computed_cdf and computed_cdf <= cdfX
    else:
        computed_x = int(distribution.computeQuantile(computed_ccdf, True)[0])
        print(f"    computeQuantile (B). Computed X = {computed_x}, expected = {x}")
        assert x == computed_x


# %%
# quantile bug
alpha = 0.05
beta = 0.05
for n in range(59, 100):
    d = ot.Binomial(n, alpha)
    k = d.computeQuantile(beta)[0]
    p1 = d.computeCDF(k - 1)
    p2 = d.computeCDF(k)
    ok = p1 < beta <= p2
    print(f"n={n} k={k:.0f} p(k-1)={p1:.4f} p(k)={p2:.4f} ok={ok}")
    assert ok

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()
