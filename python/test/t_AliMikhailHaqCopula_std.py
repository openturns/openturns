#! /usr/bin/env python

import math as m
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Instantiate one distribution object
dim = 2
copula = ot.AliMikhailHaqCopula(0.5)
print("Copula ", repr(copula))
print("Copula ", copula)
print("Mean ", repr(copula.getMean()))
print("Covariance ", repr(copula.getCovariance()))

# Is this copula an elliptical distribution?
print("Elliptical distribution= ", copula.isElliptical())

# Is this copula elliptical ?
print("Elliptical copula= ", copula.hasEllipticalCopula())

# Is this copula independent ?
print("Independent copula= ", copula.hasIndependentCopula())

# Test for realization of distribution
oneRealization = copula.getRealization()
print("oneRealization=", repr(oneRealization))

# Test for sampling
size = 10
oneSample = copula.getSample(size)
print("oneSample=", repr(oneSample))

# Define a point
point = ot.Point(dim, 0.2)

# Show PDF and CDF of point
pointPDF = copula.computePDF(point)
pointCDF = copula.computeCDF(point)
print("Point = ", repr(point), " pdf=%.6f" % pointPDF, " cdf=%.6f" % pointCDF)

# Get 50% quantile
quantile = copula.computeQuantile(0.5)
print("Quantile=", repr(quantile))
print("CDF(quantile)=%.6f" % copula.computeCDF(quantile))

# Get 5% quantile
quantile = copula.computeQuantile(0.95, True)
print("Quantile=", repr(quantile))

# Get 95% survival function
inverseSurvival = ot.Point(copula.computeInverseSurvivalFunction(0.95))
print("InverseSurvival=", repr(inverseSurvival))
print(
    "Survival(inverseSurvival)=%.6f" % copula.computeSurvivalFunction(inverseSurvival)
)
print("entropy=%.6f" % copula.computeEntropy())
# Confidence regions
interval, threshold = copula.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
print("Minimum volume interval=", interval)
print("threshold=", ot.Point(1, threshold))
levelSet, beta = copula.computeMinimumVolumeLevelSetWithThreshold(0.95)
print("Minimum volume level set=", levelSet)
print("beta=", ot.Point(1, beta))
interval, beta = copula.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
print("Bilateral confidence interval=", interval)
print("beta=", ot.Point(1, beta))
interval, beta = copula.computeUnilateralConfidenceIntervalWithMarginalProbability(
    0.95, False
)
print("Unilateral confidence interval (lower tail)=", interval)
print("beta=", ot.Point(1, beta))
interval, beta = copula.computeUnilateralConfidenceIntervalWithMarginalProbability(
    0.95, True
)
print("Unilateral confidence interval (upper tail)=", interval)
print("beta=", ot.Point(1, beta))

# Extract the marginals
for i in range(dim):
    margin = copula.getMarginal(i)
    print("margin=", repr(margin))
    print("margin PDF=%.6f" % margin.computePDF([0.25]))
    print("margin CDF=%.6f" % margin.computeCDF([0.25]))
    print("margin quantile=", repr(margin.computeQuantile(0.95)))
    print("margin realization=", repr(margin.getRealization()))

# Extract a 2-D marginal
indices = [1, 0]
print("indices=", repr(indices))
margins = copula.getMarginal(indices)
print("margins=", repr(margins))
print("margins PDF=%.6f" % margins.computePDF([0.25] * 2))
print("margins CDF=%.6f" % margins.computeCDF([0.25] * 2))
quantile = ot.Point(margins.computeQuantile(0.95))
print("margins quantile=", repr(quantile))
print("margins CDF(qantile)=%.6f" % margins.computeCDF(quantile))
print("margins realization=", repr(margins.getRealization()))

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(copula)
validation.run()

# Theta setter rejects NaN and out-of-range values, accepts endpoints
probe = ot.AliMikhailHaqCopula(0.5)
with ott.assert_raises(TypeError):
    probe.setTheta(float("nan"))
with ott.assert_raises(TypeError):
    probe.setTheta(-1.5)
with ott.assert_raises(TypeError):
    probe.setTheta(1.5)
probe.setTheta(1.0)
ott.assert_almost_equal(probe.getTheta(), 1.0, 0.0, 0.0)

# Theta=1 is the uv/(u+v-uv) copula, of generator 1/t-1
unitCopula = ot.AliMikhailHaqCopula(1.0)
ott.assert_almost_equal(unitCopula.computeCDF([0.2, 0.2]), 1.0 / 9.0, 1e-12, 0.0)
ott.assert_almost_equal(unitCopula.computePDF([0.2, 0.2]), 1.71467764, 1e-6, 0.0)
ott.assert_almost_equal(unitCopula.computeArchimedeanGenerator(0.5), 1.0, 0.0, 0.0)
ott.assert_almost_equal(unitCopula.computeInverseArchimedeanGenerator(1.0), 0.5, 0.0, 0.0)
ott.assert_almost_equal(unitCopula.computeArchimedeanGeneratorDerivative(0.5), -4.0, 0.0, 0.0)
ott.assert_almost_equal(unitCopula.computeArchimedeanGeneratorSecondDerivative(0.5), 16.0, 0.0, 0.0)

# Theta=0 is the independent copula, of generator -log(t)
independentCopula = ot.AliMikhailHaqCopula(0.0)
ott.assert_almost_equal(independentCopula.computeArchimedeanGeneratorDerivative(0.5), -2.0, 0.0, 0.0)
ott.assert_almost_equal(
    independentCopula.computeArchimedeanGeneratorSecondDerivative(0.5), 4.0, 0.0, 0.0
)

# --- AliMikhailHaqCopula: endpoints allowed, independence generator ---
assert ot.AliMikhailHaqCopula(1.0).getTheta() == 1.0
assert ot.AliMikhailHaqCopula(-1.0).getTheta() == -1.0
with ott.assert_raises(Exception):
    ot.AliMikhailHaqCopula(1.5)

# --- AliMikhailHaqCopula: entropy is finite at endpoints, see #3382 ---
ott.assert_almost_equal(
    ot.AliMikhailHaqCopula(-1.0).computeEntropy(),
    0.5 + m.log(2.0) - m.pi**2 / 8.0,
    1e-12,
    0.0,
)
ott.assert_almost_equal(
    ot.AliMikhailHaqCopula(1.0).computeEntropy(), 0.5 - m.log(2.0), 1e-12, 0.0
)
