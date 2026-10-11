#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)


def compute_max_error(ref_copula, est_copula):
    """
    Compute max error between ref_copula & estimated one
    Error is evaluated in the set (u, v) where both belong to
    {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0}
    """
    max_error = 0.0
    for n in range(11):
        for p in range(11):
            point = [0.1 * n, 0.1 * p]
            max_error = max(
                max_error,
                abs(ref_copula.computeCDF(point) - est_copula.computeCDF(point)),
            )
    return max_error


def check_bernstein_copula(est_copula):
    """
    Check if an estimated distribution of kind EmpiricalBernstein
    is of type copula and all marginals are also.
    """
    assert est_copula.isCopula()
    dimension = est_copula.getDimension()
    for d in range(dimension):
        assert est_copula.getMarginal(d).isCopula()


# reference selections and errors, seeded run
reference = [
    # GumbelCopula(3.0)
    [(25, 0.03007955557515074), (5, 0.070768723594356042), (92, 0.02671845339711898)],
    # ClaytonCopula(3.0)
    [(33, 0.026791658239094601), (5, 0.072667050775667263), (85, 0.031137020998843884)],
    # FrankCopula(3.0)
    [(3, 0.059100062777299456), (5, 0.043119949140935809), (20, 0.024632966264832412)],
]

coll = [ot.GumbelCopula(3.0), ot.ClaytonCopula(3.0), ot.FrankCopula(3.0)]
size = 100
for i, ref_copula in enumerate(coll):
    ref_copula = coll[i]
    sample = ref_copula.getSample(size)
    # Default method: log-likelihood
    m = ot.BernsteinCopulaFactory.ComputeLogLikelihoodBinNumber(sample)
    assert m == reference[i][0][0]
    est_copula = ot.BernsteinCopulaFactory().build(sample, m)
    max_error = compute_max_error(ref_copula, est_copula)
    ott.assert_almost_equal(max_error, reference[i][0][1], 1e-9, 0.0)
    check_bernstein_copula(est_copula)
    # AMISE method
    m = ot.BernsteinCopulaFactory.ComputeAMISEBinNumber(sample)
    assert m == reference[i][1][0]
    est_copula = ot.BernsteinCopulaFactory().build(sample, m)
    max_error = compute_max_error(ref_copula, est_copula)
    ott.assert_almost_equal(max_error, reference[i][1][1], 1e-9, 0.0)
    check_bernstein_copula(est_copula)
    # Penalized Csiszar divergence method
    f = ot.SymbolicFunction("t", "-log(t)")
    m = ot.BernsteinCopulaFactory.ComputePenalizedCsiszarDivergenceBinNumber(sample, f)
    assert m == reference[i][2][0]
    est_copula = ot.BernsteinCopulaFactory().build(sample, m)
    max_error = compute_max_error(ref_copula, est_copula)
    ott.assert_almost_equal(max_error, reference[i][2][1], 1e-9, 0.0)
    check_bernstein_copula(est_copula)
# Test the build method dispatch for all criteria
factory = ot.BernsteinCopulaFactory()
small_sample = ot.GumbelCopula(3.0).getSample(30)
assert factory.build(small_sample).getClassName() == "Distribution"
assert factory.build(small_sample, "AMISE").getClassName() == "Distribution"
assert (
    factory.build(small_sample, "LogLikelihood").getClassName() == "Distribution"
)
f = ot.SymbolicFunction("t", "-log(t)")
assert (
    factory.build(small_sample, "PenalizedCsiszarDivergence", f).getClassName()
    == "Distribution"
)
with ott.assert_raises(TypeError):
    factory.build(small_sample, "UnknownMethod")
with ott.assert_raises(ValueError):
    factory.build(small_sample, 0)

# Single-candidate search: MinM == MaxM evaluates once and stops
ot.ResourceMap.SetAsUnsignedInteger("BernsteinCopulaFactory-MinM", 3)
ot.ResourceMap.SetAsUnsignedInteger("BernsteinCopulaFactory-MaxM", 3)
try:
    ot.RandomGenerator.SetSeed(0)
    singleSample = ot.GumbelCopula(3.0).getSample(size)
    assert ot.BernsteinCopulaFactory.ComputeLogLikelihoodBinNumber(singleSample) == 3
finally:
    ot.ResourceMap.SetAsUnsignedInteger("BernsteinCopulaFactory-MinM", 2)
    ot.ResourceMap.SetAsUnsignedInteger("BernsteinCopulaFactory-MaxM", 1)
