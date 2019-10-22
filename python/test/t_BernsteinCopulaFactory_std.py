#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

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
            max_error = max(max_error, abs(ref_copula.computeCDF(
                point) - est_copula.computeCDF(point)))
    return max_error


def check_bernstein_copula(est_copula):
    """
    Check if an estimated distribution of kind EmpiricalBernstein
    is of type copula and all marginals are also.
    """
    print("Is estimation a copula ? --> ", est_copula.isCopula())
    print("Maginal checking")
    dimension = est_copula.getDimension()
    for d in range(dimension):
        print("Is marginal %d a copula ? --> %s" % (d, est_copula.isCopula()))


try:
    coll = [ot.GumbelCopula(3.0), ot.ClaytonCopula(3.0), ot.FrankCopula(3.0)]
    size = 100
    for i, ref_copula in enumerate(coll):
        ref_copula = coll[i]
        print("Reference copula", str(ref_copula))
        sample = ref_copula.getSample(size)
        # Default method: log-likelihood
        m = ot.BernsteinCopulaFactory.ComputeLogLikelihoodBinNumber(sample)
        print("Log-likelihood m=", m)
        est_copula = ot.BernsteinCopulaFactory().build(sample, m)
        max_error = compute_max_error(ref_copula, est_copula)
        print("Max. error=%.5f" % max_error)
        check_bernstein_copula(est_copula)
        # AMISE method
        m = ot.BernsteinCopulaFactory.ComputeAMISEBinNumber(sample)
        print("AMISE m=", m)
        est_copula = ot.BernsteinCopulaFactory().build(sample, m)
        max_error = compute_max_error(ref_copula, est_copula)
        print("Max. error=%.5f" % max_error)
        check_bernstein_copula(est_copula)
        # Penalized Csiszar divergence method
        f = ot.SymbolicFunction("t", "-log(t)")
        m = ot.BernsteinCopulaFactory.ComputePenalizedCsiszarDivergenceBinNumber(
            sample, f)
        print("Penalized Csiszar divergence m=", m)
        est_copula = ot.BernsteinCopulaFactory().build(sample, m)
        max_error = compute_max_error(ref_copula, est_copula)
        print("Max. error=%.5f" % max_error)
        check_bernstein_copula(est_copula)
        print("")

except:
    import sys
    print("t_BernsteinCopulaFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
