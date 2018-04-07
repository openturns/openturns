#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    coll = [GumbelCopula(3.0), ClaytonCopula(3.0), FrankCopula(3.0)]
    size = 100
    for i in range(len(coll)):
        ref_copula = coll[i]
        print("Reference copula", str(ref_copula))
        sample = ref_copula.getSample(size)
        # Default method: log-likelihood
        m = BernsteinCopulaFactory.ComputeLogLikelihoodBinNumber(sample)
        print("Log-likelihood m=", m)
        est_copula = BernsteinCopulaFactory().build(sample, m)
        max_error = 0.0
        for n in range(11):
            for p in range(11):
                point = [0.1 * n, 0.1 * p]
                max_error = max(max_error, abs(ref_copula.computeCDF(
                    point) - est_copula.computeCDF(point)))
        print("Max. error=%.5f" % max_error)
        # AMISE method
        m = BernsteinCopulaFactory.ComputeAMISEBinNumber(sample)
        print("AMISE m=", m)
        est_copula = BernsteinCopulaFactory().build(sample, m)
        max_error = 0.0
        for n in range(11):
            for p in range(11):
                point = [0.1 * n, 0.1 * p]
                max_error = max(max_error, abs(ref_copula.computeCDF(
                    point) - est_copula.computeCDF(point)))
        print("Max. error=%.5f" % max_error)
        # Penalized Csiszar divergence method
        f = SymbolicFunction("t", "-log(t)")
        m = BernsteinCopulaFactory.ComputePenalizedCsiszarDivergenceBinNumber(sample, f)
        print("Penalized Csiszar divergence m=", m)
        est_copula = BernsteinCopulaFactory().build(sample, m)
        max_error = 0.0
        for n in range(11):
            for p in range(11):
                point = [0.1 * n, 0.1 * p]
                max_error = max(max_error, abs(ref_copula.computeCDF(
                    point) - est_copula.computeCDF(point)))
        print("Max. error=%.5f" % max_error)

except:
    import sys
    print("t_BernsteinCopulaFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
