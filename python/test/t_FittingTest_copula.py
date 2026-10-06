#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Reduce bootstrap cost for the test suite
ot.ResourceMap.SetAsUnsignedInteger("FittingTest-CopulaMinimumSamplingSize", 10)
ot.ResourceMap.SetAsUnsignedInteger("FittingTest-CopulaMaximumSamplingSize", 100)
ot.ResourceMap.SetAsScalar("FittingTest-CopulaPrecision", 0.05)

ot.RandomGenerator.SetSeed(0)
copula = ot.GumbelCopula(3.0)
size = 100
sample = copula.getSample(size)

# Fully specified, true copula: accept
result_cvm = ot.FittingTest.CopulaCramerVonMises(sample, copula)
print("CvM true p=", result_cvm.getPValue())
assert result_cvm.getBinaryQualityMeasure()
assert result_cvm.getStatistic() >= 0.0
assert 0.0 <= result_cvm.getPValue() <= 1.0

result_ks = ot.FittingTest.CopulaKolmogorov(sample, copula)
print("KS true p=", result_ks.getPValue())
assert result_ks.getBinaryQualityMeasure()

# Fully specified, wrong copula: reject
wrong = ot.ClaytonCopula(3.0)
result_cvm_wrong = ot.FittingTest.CopulaCramerVonMises(sample, wrong)
print("CvM wrong p=", result_cvm_wrong.getPValue())
assert not result_cvm_wrong.getBinaryQualityMeasure()

result_ks_wrong = ot.FittingTest.CopulaKolmogorov(sample, wrong)
print("KS wrong p=", result_ks_wrong.getPValue())
assert not result_ks_wrong.getBinaryQualityMeasure()

# Factory version, true family: accept
estimated, result_factory = ot.FittingTest.CopulaCramerVonMises(
    sample, ot.GumbelCopulaFactory()
)
print("CvM factory true p=", result_factory.getPValue())
assert result_factory.getBinaryQualityMeasure()

estimated_ks, result_factory_ks = ot.FittingTest.CopulaKolmogorov(
    sample, ot.GumbelCopulaFactory()
)
print("KS factory true p=", result_factory_ks.getPValue())
assert result_factory_ks.getBinaryQualityMeasure()

# Factory version, wrong family: reject
_, result_factory_wrong = ot.FittingTest.CopulaCramerVonMises(
    sample, ot.ClaytonCopulaFactory()
)
print("CvM factory wrong p=", result_factory_wrong.getPValue())
assert not result_factory_wrong.getBinaryQualityMeasure()

# Rank-based: raw joint sample with non-uniform marginals behaves the same
marginals = [ot.Normal(), ot.Exponential()]
joint = ot.JointDistribution(marginals, copula)
raw_sample = joint.getSample(size)
result_raw = ot.FittingTest.CopulaCramerVonMises(raw_sample, copula)
print("CvM raw p=", result_raw.getPValue())
assert result_raw.getBinaryQualityMeasure()

# Error cases
with ott.assert_raises(TypeError):
    ot.FittingTest.CopulaCramerVonMises(sample.getMarginal(0), copula)
with ott.assert_raises(TypeError):
    ot.FittingTest.CopulaCramerVonMises(sample, ot.Normal(2))
with ott.assert_raises(TypeError):
    ot.FittingTest.CopulaKolmogorov(sample, ot.Normal(2))
