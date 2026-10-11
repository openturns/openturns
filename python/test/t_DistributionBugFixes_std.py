#! /usr/bin/env python
# Regression tests for the fix_distribution branch.
# Each block covers a bug fixed by direct source reading (see doc/distribution_audit.tex).
# Pure asserts, no reference output file needed.

import cmath
import math as m
import os

import numpy as np

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# --- Bernoulli: kurtosis sign, load recomputes range ---
ott.assert_almost_equal(ot.Bernoulli(0.7).getKurtosis(), [1.761904761904762], 1e-12, 0.0)
ott.assert_almost_equal(ot.Bernoulli(0.5).getKurtosis(), [1.0], 1e-12, 0.0)
studyFile = "myStudyBugFixes.xml"
study = ot.Study(studyFile)
study.add("bernoulli", ot.Bernoulli(0.0))
study.save()
study2 = ot.Study(studyFile)
study2.load()
reloaded = ot.Bernoulli()
study2.fillObject("bernoulli", reloaded)
os.remove(studyFile)
ott.assert_almost_equal(reloaded.getRange().getLowerBound(), [0.0], 0.0, 0.0)
ott.assert_almost_equal(reloaded.getRange().getUpperBound(), [0.0], 0.0, 0.0)

# --- Arcsine: setAB invalidates cached mean/covariance, kurtosis is 1.5 ---
arcsine = ot.Arcsine(0.0, 1.0)
assert arcsine.getMean()[0] == 0.5
arcsine.setA(-1.0)
ott.assert_almost_equal(arcsine.getMean(), [0.0], 0.0, 0.0)
ott.assert_almost_equal(arcsine.getKurtosis(), [1.5], 0.0, 0.0)
with ott.assert_raises(Exception):
    ot.ArcsineMuSigma().setValues([0.0, -1.0])

# --- Beta: endpoint density, DDF guard, Arcsine shortcut, roughness guard ---
ott.assert_almost_equal(ot.Beta(3.0, 1.0, 0.0, 2.0).computePDF(2.0), 1.5, 1e-12, 0.0)
ott.assert_almost_equal(
    ot.Beta(3.0, 1.0, 0.0, 2.0).computeLogPDF(2.0), m.log(1.5), 1e-12, 0.0
)
ott.assert_almost_equal(ot.Beta(2.0, 1.0, 0.0, 1.0).computeDDF([1.0]), [0.0], 0.0, 0.0)
assert (
    ot.Beta(0.5, 0.5, -1.0, 1.0).getStandardRepresentative().getImplementation().getClassName()
    == "Arcsine"
)
with ott.assert_raises(Exception):
    ot.Beta(0.3, 0.4).getRoughness()
with ott.assert_raises(Exception):
    ot.BetaMuSigma().setValues([0.5, 10.0, 0.0, 1.0])
with ott.assert_raises(Exception):
    ot.BetaMuSigma()([0.5, 10.0, 0.0, 1.0])

# --- Binomial: negative trial count rejected, degenerate quantiles ---
with ott.assert_raises(Exception):
    ot.Binomial().setParameter([-1.0, 0.5])
ott.assert_almost_equal(ot.Binomial(5, 0.0).computeQuantile(0.3), [0.0], 0.0, 0.0)
ott.assert_almost_equal(ot.Binomial(5, 1.0).computeQuantile(0.3), [5.0], 0.0, 0.0)

# --- BlockIndependentCopula: log-density path, probability offsets ---
bic = ot.BlockIndependentCopula([ot.IndependentCopula(1), ot.IndependentCopula(1)])
ott.assert_almost_equal(bic.computeLogPDF([0.5, 0.5]), 0.0, 1e-12, 0.0)
bic2 = ot.BlockIndependentCopula([ot.FrankCopula(3.0), ot.ClaytonCopula(2.0)])
prob = bic2.computeProbability(ot.Interval([0.0, 0.0, 0.5, 0.5], [1.0, 1.0, 1.0, 1.0]))
ref = ot.ClaytonCopula(2.0).computeProbability(ot.Interval([0.5, 0.5], [1.0, 1.0]))
ott.assert_almost_equal(prob, ref, 1e-12, 0.0)
assert prob < 1.0

# --- BlockIndependentDistribution: multi-block support ---
bid = ot.BlockIndependentDistribution([ot.Binomial(3, 0.5), ot.Binomial(2, 0.5)])
support = bid.getSupport()
assert support.getSize() == 12
assert set(p[1] for p in support) == {0.0, 1.0, 2.0}

# --- Burr: nonexistent moments throw, tail log-density is finite ---
with ott.assert_raises(Exception):
    ot.Burr(2.0, 0.5).getMean()
ott.assert_almost_equal(ot.Burr(2.0, 2.0).getMean()[0], m.pi / 4.0, 1e-12, 0.0)
assert m.isfinite(ot.Burr(2.0, 2.0).computeLogPDF([1.0e150]))

# --- Chi family: gradients valid for small shapes ---
assert m.isfinite(ot.Chi(0.01).computeCDFGradient([0.5])[0])
assert m.isfinite(ot.ChiSquare(0.01).computeCDFGradient([0.5])[0])
assert m.isfinite(ot.NonCentralChiSquare(0.01, 0.0).computePDFGradient([0.5])[0])
with ott.assert_raises(Exception):
    ot.ChiFactory().build(ot.Sample([[1.0], [-2.0], [3.0]]))
with ott.assert_raises(Exception):
    ot.ChiSquareFactory().build(ot.Sample([[1.0], [-2.0], [3.0]]))

# --- ClaytonCopula: gradients at independence, negative-dependence quantile ---
ott.assert_almost_equal(ot.ClaytonCopula(0.0).computePDFGradient([0.3, 0.4]), [0.0], 0.0, 0.0)
ott.assert_almost_equal(ot.ClaytonCopula(0.0).computeCDFGradient([0.3, 0.4]), [0.0], 0.0, 0.0)
q = ot.ClaytonCopula(-0.5).computeConditionalQuantile(0.5, [0.5])
assert 0.0 <= q <= 1.0 and m.isfinite(q)

# --- ClaytonCopula: log-density matches the density ---
for thetaClayton in [0.0, 2.0, 50.0, 200.0]:
    clayton = ot.ClaytonCopula(thetaClayton)
    for pt in [[0.3, 0.4], [0.9, 0.95], [0.05, 0.1]]:
        ott.assert_almost_equal(
            clayton.computeLogPDF(pt), m.log(clayton.computePDF(pt)), 1e-12, 0.0
        )

# --- CombinationsDistribution: CDF, k=1 covariance, multi-index marginal ---
comb = ot.CombinationsDistribution(3, 6)
assert m.isfinite(comb.computeCDF([1.0, 2.0, 4.0]))
ott.assert_almost_equal(
    ot.CombinationsDistribution(1, 5).getCovariance(),
    ot.CovarianceMatrix(1, [2.0]),
    1e-12,
    0.0,
)
marginal = ot.CombinationsDistribution(2, 5).getMarginal([0, 1])
supportC = marginal.getSupport()
total = sum(marginal.computePDF(p) for p in supportC)
ott.assert_almost_equal(total, 1.0, 1e-12, 0.0)

# --- CompoundDistribution keeps its link function ---
link1d = ot.SymbolicFunction(["y"], ["y", "1.0"])
compound = ot.CompoundDistribution(ot.Normal(), ot.Exponential(1.0), link1d)
pdfBefore = compound.computePDF([0.0])
compound.setParameter(compound.getParameter())
ott.assert_almost_equal(compound.computePDF([0.0]), pdfBefore, 1e-12, 0.0)

# --- Dirac integrality follows the atom ---
assert ot.Dirac(2.0).isIntegral()
assert not ot.Dirac(0.5).isIntegral()

# --- Dirichlet: standard deviation, sequential conditionals ---
ott.assert_almost_equal(
    ot.Dirichlet([1.0, 2.0, 3.0]).getStandardDeviation(),
    [0.14085904245475275, 0.17817416127494959],
    1e-12,
    0.0,
)
seq = ot.Dirichlet([1.0, 1.0, 1.0]).computeSequentialConditionalPDF([0.5, 0.3])
assert all(m.isfinite(v) for v in seq)

# --- DiscreteCompoundDistribution: huge transforms are rejected ---
with ott.assert_raises(Exception):
    ot.DiscreteCompoundDistribution(ot.Binomial(2097152, 0.5), ot.Poisson(2.0))

# --- EmpiricalBernsteinCopula: sequential CDF stays in [0, 1] ---
ot.RandomGenerator.SetSeed(1)
ebc = ot.EmpiricalBernsteinCopula(ot.Normal(2).getSample(20), 2)
seqCDF = ebc.computeSequentialConditionalCDF([0.5, 0.5])
assert all(0.0 <= v <= 1.0 for v in seqCDF)

# --- Exponential: gradients agree with density at the atom ---
ott.assert_almost_equal(ot.Exponential().computePDFGradient([0.0]), [0.0, 0.0], 0.0, 0.0)
ott.assert_almost_equal(ot.Exponential().computeCDFGradient([0.0]), [0.0, 0.0], 0.0, 0.0)

# --- ExtremeValueCopula: density is nonnegative ---
evc = ot.ExtremeValueCopula(ot.SymbolicFunction("t", "(t-0.5)^2+0.75"))
assert evc.computePDF([0.3, 0.4]) >= 0.0
assert m.isfinite(evc.computeLogPDF([0.3, 0.4]))

# --- FarlieGumbelMorgensternCopula: gradients agree with density at atoms ---
ott.assert_almost_equal(
    ot.FarlieGumbelMorgensternCopula(0.5).computePDFGradient([0.0, 0.5]), [0.0], 0.0, 0.0
)
ott.assert_almost_equal(
    ot.FarlieGumbelMorgensternCopula(0.5).computeCDFGradient([0.0, 0.5]), [0.0], 0.0, 0.0
)
ot.RandomGenerator.SetSeed(2)
assert all(m.isfinite(v) for v in ot.FarlieGumbelMorgensternCopula(1.0).getRealization())

# --- FiniteDiscreteDistribution: tail quantile walks the right way ---
fdd = ot.FiniteDiscreteDistribution([[0.0], [1.0]], [0.5, 0.5])
ott.assert_almost_equal(fdd.computeQuantile(0.4, True), [1.0], 0.0, 0.0)
ott.assert_almost_equal(fdd.computeQuantile(0.6), [1.0], 0.0, 0.0)

# --- FrankCopula: negative dependence uses the general formulas ---
frankNeg = ot.FrankCopula(-5.0)
assert m.isfinite(frankNeg.computeEntropy())
# entropy is consistent with a Monte Carlo estimate
ot.RandomGenerator.SetSeed(10)
sizeMC = 200000
sampleMC = frankNeg.getSample(sizeMC)
logPDFMC = frankNeg.computeLogPDF(sampleMC).asPoint()
ott.assert_almost_equal(
    frankNeg.computeEntropy(), -sum(logPDFMC) / sizeMC, 1e-2, 1e-3
)
assert frankNeg.getCovariance()[0, 1] < 0.0
# covariance matches its Monte Carlo estimate
meanMC = sampleMC.computeMean()
covMC = ((sampleMC - meanMC).computeCovariance()[0, 1])
ott.assert_almost_equal(frankNeg.getCovariance()[0, 1], covMC, 1e-2, 1e-3)

# --- Frechet: wrong-dimension inputs are rejected ---
with ott.assert_raises(Exception):
    ot.Frechet().computePDF([0.5, 0.5])

# --- GalambosCopula has no independent case ---
assert not ot.GalambosCopula(0.5).hasIndependentCopula()

# --- Geometric: quantile endpoints and realization support ---
ott.assert_almost_equal(ot.Geometric(0.3).computeScalarQuantile(0.0), 1.0, 0.0, 0.0)
assert ot.Geometric(0.3).computeScalarQuantile(1.0) == ot.SpecFunc.Infinity
ot.RandomGenerator.SetSeed(3)
for _ in range(100):
    assert ot.Geometric(0.3).getRealization()[0] >= 1.0

# --- GumbelCopula: conditional edge cases, gradient at the boundary ---
ott.assert_almost_equal(ot.GumbelCopula(2.0).computeConditionalCDF(0.5, [1.0]), 0.0, 0.0, 0.0)
ott.assert_almost_equal(ot.GumbelCopula(2.0).computeConditionalCDF(1.0, [1.0]), 1.0, 0.0, 0.0)
assert m.isfinite(ot.GumbelCopula(1.0).computePDFGradient([0.3, 0.4])[0])

# --- GumbelFactory rejects constant samples, helper prints true names ---
with ott.assert_raises(Exception):
    ot.GumbelFactory().build(ot.Sample([[1.0], [1.0], [1.0]]))
assert "lambda" in str(ot.GumbelLambdaGamma())
with ott.assert_raises(Exception):
    ot.GumbelMuSigma().setValues([0.0, -1.0])
with ott.assert_raises(Exception):
    ot.GumbelLambdaGamma().setValues([-1.0, 0.0])

# --- Hypergeometric: range lower bound and stepwise CDF ---
ott.assert_almost_equal(
    ot.Hypergeometric(10, 4, 7).getRange().getLowerBound(), [1.0], 0.0, 0.0
)
ott.assert_almost_equal(
    ot.Hypergeometric(10, 4, 7).computeCDF(2.6),
    ot.Hypergeometric(10, 4, 7).computeCDF(2.0),
    0.0,
    0.0,
)

# --- IndependentCopula accepts unit conditioning ---
ott.assert_almost_equal(
    ot.IndependentCopula(2).computeConditionalCDF(0.5, [1.0]), 0.5, 0.0, 0.0
)
ott.assert_almost_equal(
    ot.IndependentCopula(2).computeConditionalQuantile(0.5, [1.0]), 0.5, 0.0, 0.0
)

# --- InverseNormal: CDF stays in [0, 1] over a wide parameter grid ---
for mu, lambd in [(0.01, 100.0), (1.0, 1.0), (2.0, 0.5), (0.5, 50.0)]:
    distIN = ot.InverseNormal(mu, lambd)
    for x in [0.01, 0.1, 1.0, 5.0, 50.0]:
        c = distIN.computeCDF([x])
        assert 0.0 <= c <= 1.0 and m.isfinite(c)
ott.assert_almost_equal(ot.InverseNormal(0.01, 100.0).computeCDF([1.0]), 1.0, 0.0, 0.0)

# --- Wishart families: label count matches parameter count ---
for cls in [ot.Wishart, ot.InverseWishart]:
    dist = cls(ot.CovarianceMatrix(2, [1.0, 0.2, 0.2, 1.0]), 5.0)
    assert len(dist.getParameterDescription()) == len(dist.getParameter()) == 4

# --- PointConditionalDistribution: Student conditional scale is linear ---
R = ot.CorrelationMatrix(2)
R[0, 1] = 0.5
pcdStudent = ot.PointConditionalDistribution(
    ot.Student(5.0, [0.0, 0.0], [1.0, 1.0], R), [1], [0.5]
)
ott.assert_almost_equal(pcdStudent.getMean(), [0.25], 1e-12, 0.0)
ott.assert_almost_equal(
    pcdStudent.getCovariance(), ot.CovarianceMatrix(1, [0.984375]), 1e-12, 0.0
)
# covariance constructor honors its documented contract
covStudent = ot.Student(6.0, [0.25], ot.CovarianceMatrix(1, [0.984375]))
ott.assert_almost_equal(
    covStudent.getCovariance(), ot.CovarianceMatrix(1, [0.984375]), 1e-12, 0.0
)
with ott.assert_raises(Exception):
    ot.Student(1.5, [0.0], ot.CovarianceMatrix(1, [1.0]))
# ... including for singular matrices as before
singStudent = ot.Student(
    4.2, [0.0] * 3, ot.CovarianceMatrix([[1.0, 1.0, 0.0], [1.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
)
ott.assert_almost_equal(singStudent.getMean(), [0.0] * 3, 0.0, 0.0)

# --- JointByConditioningDistribution: empty conditioning domain gives 0 ---
link = ot.SymbolicFunction(["y"], ["y", "1.0"])
jbcd = ot.JointByConditioningDistribution(ot.Normal(), ot.Normal(), link)
assert jbcd.computeCDF([-100.0, -100.0]) == 0.0

# --- JointByConditioningDistribution: trailing conditioned state is kept ---
link2d = ot.SymbolicFunction(["y"], ["y", "1.0", "y + 1.0", "2.0"])
conditioned2d = ot.JointDistribution([ot.Normal(), ot.Normal()])
jbcd2d = ot.JointByConditioningDistribution(conditioned2d, ot.Uniform(0.0, 1.0), link2d)
reference2d = ot.JointDistribution(
    [ot.Normal(0.3, 1.0), ot.Normal(1.3, 2.0)], ot.IndependentCopula(2)
)
ott.assert_almost_equal(
    jbcd2d.computeConditionalPDF(1.0, [0.3, 0.5]),
    reference2d.computeConditionalPDF(1.0, [0.5]),
    1e-12,
    0.0,
)

# --- KPermutationsDistribution: setK refreshes the range ---
kp = ot.KPermutationsDistribution(3, 5)
kp.setK(2)
assert kp.getRange().getDimension() == 2

# --- LogNormal helpers validate their inputs ---
with ott.assert_raises(Exception):
    ot.LogNormalMuSigma().setValues([0.0, -1.0, 0.0])
with ott.assert_raises(Exception):
    ot.LogNormalMuSigmaOverMu().setValues([0.0, 0.5, -1.0])
with ott.assert_raises(Exception):
    ot.LogNormalMuErrorFactor().setValues([0.0, 0.5, 0.0])
with ott.assert_raises(Exception):
    ot.GammaMuSigma().setValues([0.0, -1.0, 0.0])
with ott.assert_raises(Exception):
    ot.WeibullMaxMuSigma().setValues([2.0, 1.0, 0.0])
with ott.assert_raises(Exception):
    ot.WeibullMinMuSigma().setValues([0.0, 1.0, 2.0])

# --- LogNormalFactory: invalid shifts are rejected ---
with ott.assert_raises(Exception):
    ot.LogNormalFactory().buildMethodOfLeastSquares(
        ot.Sample([[1.0], [2.0], [3.0]]), 5.0
    )
with ott.assert_raises(Exception):
    ot.ParetoFactory().buildMethodOfLeastSquares(
        ot.Sample([[1.0], [2.0], [3.0]]), 5.0
    )

# --- LogUniform: characteristic function at zero, DDF at the atom ---
ott.assert_almost_equal(
    [ot.LogUniform(1.0, 2.0).computeCharacteristicFunction(0.0).real],
    [1.0],
    0.0,
    0.0,
)
ott.assert_almost_equal(ot.LogUniform(1.0, 2.0).computeDDF([1.0]), [0.0], 0.0, 0.0)

# --- MarshallOlkinCopula: continuity matches independence ---
assert ot.MarshallOlkinCopula(0.0, 0.5).isContinuous()
assert not ot.MarshallOlkinCopula(0.5, 0.5).isContinuous()
with ott.assert_raises(Exception):
    ot.MarshallOlkinCopula(0.5, 0.5).computeCDF([0.5])

# --- MaximumDistribution: equality sees copy counts, operators agree ---
assert ot.MaximumDistribution(ot.Normal(), 3) != ot.MaximumDistribution(ot.Normal(), 4)
assert ot.MaximumDistribution(ot.Normal(), 3) == ot.MaximumDistribution(ot.Normal(), 3)

# --- MaximumEntropyOrderStatisticsDistribution: equality sees its flags ---
coll = [ot.Uniform(0.0, 1.0), ot.Uniform(2.0, 3.0)]
m1 = ot.MaximumEntropyOrderStatisticsDistribution(coll, False)
m2 = ot.MaximumEntropyOrderStatisticsDistribution(coll, False)
m3 = ot.MaximumEntropyOrderStatisticsDistribution(coll, True)
assert m1 == m2
assert m1 != m3

# --- MeixnerDistribution: log-characteristic function uses the asymmetry ---
# parameter order is (beta=scale, alpha=asymmetry, delta, gamma=location)
s, a, d, mu, x = 1.5, 0.5, 2.5, -0.5, 1.0
meix = ot.MeixnerDistribution(s, a, d, mu)
expectedLog = (
    2.0 * d * cmath.log(m.cos(0.5 * a))
    + 1j * mu * x
    - 2.0 * d * cmath.log(cmath.cosh(complex(0.5 * s * x, -0.5 * a)))
)
gotLog = meix.computeLogCharacteristicFunction(x)
ott.assert_almost_equal([gotLog.real], [expectedLog.real], 1e-12, 0.0)
ott.assert_almost_equal([gotLog.imag], [expectedLog.imag], 1e-12, 0.0)

# --- MixedHistogramFiniteDiscreteDistribution: marginal masses, load rebuild ---
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

# --- Mixture: equality sees weights, singularities of atoms are safe ---
assert ot.Mixture([ot.Normal(), ot.Normal()], [0.3, 0.7]) != ot.Mixture(
    [ot.Normal(), ot.Normal()], [0.6, 0.4]
)
assert ot.Mixture([ot.Normal(), ot.Normal()], [0.3, 0.7]) == ot.Mixture(
    [ot.Normal(), ot.Normal()], [0.3, 0.7]
)
assert list(ot.Mixture([ot.Dirac(1.0), ot.Dirac(1.0)], [0.5, 0.5]).getSingularities()) == []
assert list(
    ot.Mixture([ot.Dirac(1.0), ot.Uniform(0.0, 3.0)], [0.5, 0.5]).getSingularities()
) == [1.0]

# --- MultivariateUniform: log-density agrees with density at the atom ---
ott.assert_almost_equal(
    otexp.MultivariateUniform([0.0, 0.0], [1.0, 1.0]).computeLogPDF([0.0, 0.5]),
    ot.SpecFunc.LowestScalar,
    0.0,
    0.0,
)

# --- NonCentralStudent: higher moments are guarded ---
d = ot.NonCentralStudent(5.0, 0.5, 0.0)
d.setNu(2.5)
with ott.assert_raises(Exception):
    d.getSkewness()
d.setNu(3.5)
with ott.assert_raises(Exception):
    d.getKurtosis()

# --- NormalGamma: setters rebuild, kurtosis is guarded ---
ng = ot.NormalGamma(0.0, 1.0, 2.0, 3.0)
ng.setMu(1.0)
ott.assert_almost_equal(
    ng.computePDF([0.5, 0.2]),
    ot.NormalGamma(1.0, 1.0, 2.0, 3.0).computePDF([0.5, 0.2]),
    1e-12,
    0.0,
)
ng.setKappa(2.0)
ott.assert_almost_equal(
    ng.computePDF([0.5, 0.2]),
    ot.NormalGamma(1.0, 2.0, 2.0, 3.0).computePDF([0.5, 0.2]),
    1e-12,
    0.0,
)
with ott.assert_raises(Exception):
    ot.NormalGamma(0.0, 1.0, 1.5, 1.0).getKurtosis()

# --- MaximumDistribution: library convention gives zero density at finite bounds ---
ott.assert_almost_equal(
    ot.MaximumDistribution(ot.Uniform(0.0, 1.0), 2).computePDF([1.0]), 0.0, 0.0, 0.0
)
ott.assert_almost_equal(
    ot.MaximumDistribution(ot.Uniform(0.0, 1.0), 2).computePDF([0.99]), 1.98, 1e-12, 0.0
)

# --- OrdinalSumCopula: single-copula probability ---
osc = ot.OrdinalSumCopula([ot.ClaytonCopula(2.0)], [])
pOsc = osc.computeProbability(ot.Interval([0.2, 0.3], [0.8, 0.9]))
assert 0.0 <= pOsc <= 1.0 and m.isfinite(pOsc)

# --- Pareto: gradients vanish at the atom ---
ott.assert_almost_equal(
    ot.Pareto(1.0, 1.0, 0.0).computePDFGradient([1.0]), [0.0, 0.0, 0.0], 0.0, 0.0
)
ott.assert_almost_equal(
    ot.Pareto(1.0, 1.0, 0.0).computeCDFGradient([1.0]), [0.0, 0.0, 0.0], 0.0, 0.0
)
ott.assert_almost_equal(ot.Pareto(1.0, 1.0, 0.0).computeDDF([1.0]), [0.0], 0.0, 0.0)

# --- PiecewiseLinearDistribution: equality sees its base ---
x = [0.0, 1.0, 2.0]
y = [0.5, 1.0, 0.5]
assert otexp.PiecewiseLinearDistribution(x, y) == otexp.PiecewiseLinearDistribution(x, y)
assert otexp.PiecewiseLinearDistribution(
    x, y
) != otexp.PiecewiseLinearDistribution(x, [0.5, 2.0, 0.5])

# --- ProductDistribution: only centered Normal products are elliptical ---
assert ot.ProductDistribution(ot.Normal(0.0, 1.0), ot.Normal(0.0, 2.0)).isElliptical()
assert not ot.ProductDistribution(ot.Laplace(), ot.Laplace()).isElliptical()
assert not ot.ProductDistribution(ot.Normal(0.0, 1.0), ot.Laplace()).isElliptical()

# --- SkellamFactory checks both rates ---
with ott.assert_raises(Exception):
    ot.SkellamFactory().build(ot.Sample([[-5.0], [-4.0], [-5.0], [-4.0]]))

# --- SmoothedUniformFactory guards a vanishing uniform part ---
with ott.assert_raises(Exception):
    ot.SmoothedUniformFactory().build(ot.Sample([[-10.0]] * 10 + [[10.0]] * 10))

# --- InverseNormalFactory checks positivity on the moments path ---
ot.ResourceMap.SetAsString("InverseNormalFactory-Method", "Moments")
try:
    with ott.assert_raises(Exception):
        ot.InverseNormalFactory().build(ot.Sample([[-1.0], [2.0], [3.0], [4.0]]))
finally:
    ot.ResourceMap.SetAsString("InverseNormalFactory-Method", "MLE")

# --- SquaredNormal: dimension checks and absolute range ---
with ott.assert_raises(Exception):
    ot.SquaredNormal().computePDF([0.5, 0.5])
ott.assert_almost_equal(
    ot.SquaredNormal(-10.0, 1.0).getRange().getUpperBound(), [342.25], 1e-9, 0.0
)

# --- Student: nonpositive degrees of freedom are rejected ---
with ott.assert_raises(Exception):
    ot.Student(5.0, 0.0, 1.0).setNu(-1.0)

# --- TriangularFactory: out-of-range moment modes are reported ---
with ott.assert_raises(Exception):
    ot.TriangularFactory().build(ot.Sample([[0.0], [0.0], [0.0], [0.0], [10.0]]))

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

# --- TruncatedOverMesh rejects disjoint meshes ---
with ott.assert_raises(Exception):
    mesh = ot.Mesh([[10.0], [11.0]], [[0, 1]])
    ot.TruncatedOverMesh(ot.Dirac(0.0), mesh)

# --- Uniform: gradient agrees with density at the atom ---
ott.assert_almost_equal(ot.Uniform(0.0, 1.0).computePDFGradient([0.0]), [0.0, 0.0], 0.0, 0.0)
with ott.assert_raises(Exception):
    ot.UniformFactory().build(ot.Sample([[1.0], [1.0], [1.0]]))

# --- UniformOrderStatistics: CDF is one above the support ---
ott.assert_almost_equal(
    ot.UniformOrderStatistics(3).computeCDF([1.5, 2.0, 2.5]), 1.0, 0.0, 0.0
)

# --- ChiSquare/FisherSnedecor: persistence round-trips ---
study = ot.Study(studyFile)
study.add("chisquare", ot.ChiSquare(3.0))
study.add("fisher", ot.FisherSnedecor(4.0, 5.0))
study.save()
study2 = ot.Study(studyFile)
study2.load()
chiR = ot.ChiSquare(1.0)
study2.fillObject("chisquare", chiR)
ott.assert_almost_equal(chiR.getParameter(), [3.0], 0.0, 0.0)
ott.assert_almost_equal(chiR.computePDF([2.0]), ot.ChiSquare(3.0).computePDF([2.0]), 1e-12, 0.0)
fisherR = ot.FisherSnedecor(1.0, 1.0)
study2.fillObject("fisher", fisherR)
ott.assert_almost_equal(fisherR.getParameter(), [4.0, 5.0], 0.0, 0.0)
ott.assert_almost_equal(
    fisherR.computePDF([1.0]), ot.FisherSnedecor(4.0, 5.0).computePDF([1.0]), 1e-12, 0.0
)
os.remove(studyFile)

# --- WeibullMax/Min: characteristic function matches Monte Carlo ---
ot.RandomGenerator.SetSeed(11)
for weibull in [ot.WeibullMax(2.0, 1.0, 0.0), ot.WeibullMin(2.0, 1.0, 0.0)]:
    mcSample = np.array(weibull.getSample(500000)).ravel()
    u = 0.5
    mcCF = np.mean(np.exp(1j * u * mcSample))
    gotCF = weibull.computeCharacteristicFunction(u)
    ott.assert_almost_equal([gotCF.real], [mcCF.real], 5e-3, 0.0)
    ott.assert_almost_equal([gotCF.imag], [mcCF.imag], 5e-3, 0.0)

# --- UniformOrderStatistics: out-of-range sequential conditionals ---
uos = ot.UniformOrderStatistics(3)
ott.assert_almost_equal(
    uos.computeSequentialConditionalCDF([0.5, 1.5, 2.0]), [0.875, 1.0, 1.0], 1e-12, 0.0
)
ott.assert_almost_equal(
    uos.computeSequentialConditionalPDF([2.0, 2.5, 3.0]), [0.0, 0.0, 0.0], 0.0, 0.0
)

# --- UniformOverMesh: sampling keeps the description ---
mesher = ot.LevelSetMesher([9] * 2)
fn2 = ot.SymbolicFunction(["x0", "x1"], ["x0^2+x1^2-1"])
mesh2 = mesher.build(
    ot.LevelSet(fn2, ot.LessOrEqual(), 0.0), ot.Interval([-1.0, -1.0], [1.0, 1.0]), False
)
uom = ot.UniformOverMesh(mesh2)
uom.setDescription(["a", "b"])
assert list(uom.getSample(5).getDescription()) == ["a", "b"]

# --- VonMisesFisher: uniform samples give a null concentration ---
symmetric = ot.Sample([[1.0, 0.0], [-1.0, 0.0], [0.0, 1.0], [0.0, -1.0]])
vmf = otexp.VonMisesFisherFactory().build(symmetric)
ott.assert_almost_equal(vmf.getParameter()[-1], 0.0, 0.0, 0.0)
fisher = otexp.VonMisesFisher([1.0, 0.0], 1.0)
with ott.assert_raises(Exception):
    fisher.setParameter([1.0, 0.0, -1.0])
assert fisher.getDimension() == 2

# --- ZipfMandelbrot: setters refresh the harmonic table ---
zipf = ot.ZipfMandelbrot(10, 0.5, 1.5)
pdfBefore = zipf.computePDF([3.0])
zipf.setQ(1.5)
assert zipf.computePDF([3.0]) != pdfBefore
ott.assert_almost_equal(
    zipf.computePDF([3.0]), ot.ZipfMandelbrot(10, 1.5, 1.5).computePDF([3.0]), 1e-12, 0.0
)

# --- AliMikhailHaqCopula: endpoints allowed, independence generator ---
assert ot.AliMikhailHaqCopula(1.0).getTheta() == 1.0
assert ot.AliMikhailHaqCopula(-1.0).getTheta() == -1.0
with ott.assert_raises(Exception):
    ot.AliMikhailHaqCopula(1.5)

# --- ArcsineFactory/BetaFactory/GumbelFactory: clean errors on bad samples ---
with ott.assert_raises(Exception):
    ot.ArcsineFactory().build(ot.Sample([[1.0], [1.0], [1.0]]))
with ott.assert_raises(Exception):
    ot.GumbelFactory().build(ot.Sample([[1.0], [1.0], [1.0]]))

# --- PointConditionalDistribution: parameter validation and persistence ---
pcd = ot.PointConditionalDistribution(ot.Normal(2), [1], [0.5])
pcdParam = pcd.getParameter()
pcdParam[-1] = 1.0e6
with ott.assert_raises(Exception):
    pcd.setParameter(pcdParam)
pcd.setIntegrationAlgorithm(ot.SimplicialCubature())
study = ot.Study(studyFile)
study.add("pcd", pcd)
study.save()
study2 = ot.Study(studyFile)
study2.load()
pcdR = ot.PointConditionalDistribution(ot.Normal(2), [1], [0.5])
study2.fillObject("pcd", pcdR)
os.remove(studyFile)
assert (
    pcdR.getIntegrationAlgorithm().getImplementation().getClassName()
    == "SimplicialCubature"
)
ott.assert_almost_equal(
    pcdR.computePDF([0.0]), pcd.computePDF([0.0]), 1e-12, 0.0
)

# --- KernelSmoothing: bandwidth validation and degenerate cache ---
ks = ot.KernelSmoothing()
with ott.assert_raises(Exception):
    ks.build(ot.Normal().getSample(10), [-1.0])
with ott.assert_raises(Exception):
    ks.buildWeighted(
        ot.Sample([[float("nan")], [1.0], [2.0]]), [0.25, 0.25, 0.5], [0.5]
    )
constSample = ot.Sample([[2.0]] * 5)
ks.buildWeighted(constSample, [0.2] * 5, [0.5])
ott.assert_almost_equal(ks.getBandwidth(), [0.5], 0.0, 0.0)

# --- PosteriorDistribution: observations are validated in all ctors ---
with ott.assert_raises(Exception):
    ot.PosteriorDistribution(ot.Normal(), ot.Normal(), ot.Sample(0, 1))
