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
            ot.SymbolicFunction(inVarsBounds, ["-sqrt(" + formula + ")"]))
        upper.append(
            ot.SymbolicFunction(inVarsBounds, ["sqrt(" + formula + ")"]))
    integrand = ot.SymbolicFunction(inVars, ["1.0"])
    value = algo.integrate(integrand, a, b, lower, upper)[0]
    print("dim=", n + 1, ", volume= %.12g" %
          value, ", calls=", integrand.getCallsNumber())
# Second, integrate a multi-valued function
bounds = ot.Interval([-1.0] * 3, [1.0] * 3)
vars = ["x0", "x1", "x2"]
formulas = ["x0^2 + 2*x1^2 + 3*x2^2", "x2^2 + 2*x1^2 + 3*x0^2"]
integrand = ot.SymbolicFunction(vars, formulas)
value = algo.integrate(integrand, bounds)
print("value=", value, ", calls=", integrand.getCallsNumber())
