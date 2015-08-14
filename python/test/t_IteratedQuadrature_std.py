#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# First, compute the volume of the unit ball in R^n
a = -1.0
b = 1.0
formula = "1.0"
lower = list()
upper = list()
algo = ot.IteratedQuadrature(
    ot.GaussKronrod(20, 1.0e-6, ot.GaussKronrodRule(ot.GaussKronrodRule.G3K7)))
for n in range(3):
    inVars = ot.Description.BuildDefault(n + 1, "x")
    inVarsBounds = inVars[0:n]
    if (n > 0):
        formula += "-" + inVars[n - 1] + "^2"
        lower.append(
            ot.NumericalMathFunction(inVarsBounds, ["-sqrt(" + formula + ")"]))
        upper.append(
            ot.NumericalMathFunction(inVarsBounds, ["sqrt(" + formula + ")"]))
    integrand = ot.NumericalMathFunction(inVars, ["1.0"])
    value = algo.integrate(integrand, a, b, lower, upper)[0]
    print("dim=", n + 1, ", volume= %.12g" %
          value, ", calls=", integrand.getCallsNumber())
