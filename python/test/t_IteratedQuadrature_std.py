#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# First, compute the volume of the unit ball in R^n
a = -1.0
b = 1.0
formula = "1.0"
lower = list()
upper = list()
algo = ot.IteratedQuadrature(
    ot.GaussKronrod(20, 1.0e-6, ot.GaussKronrodRule(ot.GaussKronrodRule.G3K7))
)
for n in range(3):
    inVars = ot.Description.BuildDefault(n + 1, "x")
    inVarsBounds = inVars[0:n]
    if n > 0:
        formula += "-" + inVars[n - 1] + "^2"
        lower.append(ot.SymbolicFunction(inVarsBounds, ["-sqrt(" + formula + ")"]))
        upper.append(ot.SymbolicFunction(inVarsBounds, ["sqrt(" + formula + ")"]))
    integrand = ot.SymbolicFunction(inVars, ["1.0"])
    value = algo.integrate(integrand, a, b, lower, upper)[0]
    print(
        "dim=", n + 1, ", volume= %.12g" % value, ", calls=", integrand.getCallsNumber()
    )
# n=2: 3D ball volume = 4*pi/3
ott.assert_almost_equal(value, 4.0 * m.pi / 3.0, 1e-4, 1e-4)

# Second, integrate a multi-valued function
bounds = ot.Interval([-1.0] * 3, [1.0] * 3)
vars = ["x0", "x1", "x2"]
formulas = ["x0^2 + 2*x1^2 + 3*x2^2", "x2^2 + 2*x1^2 + 3*x0^2"]
integrand = ot.SymbolicFunction(vars, formulas)
value = algo.integrate(integrand, bounds)
print("value=", value, ", calls=", integrand.getCallsNumber())
# integral of x0^2+2*x1^2+3*x2^2 over [-1,1]^3 = 8/3 + 16/3 + 24/3 = 16
# integral of x2^2+2*x1^2+3*x0^2 over [-1,1]^3 = 8/3 + 16/3 + 24/3 = 16
ott.assert_almost_equal(value[0], 16.0, 1e-3, 1e-3)
ott.assert_almost_equal(value[1], 16.0, 1e-3, 1e-3)

# Test with GaussLegendre inner algorithm
algo.setAlgorithm(ot.GaussLegendre([10]))
integrand2 = ot.SymbolicFunction(["x0", "x1"], ["1.0"])
lower2 = [ot.SymbolicFunction(["x0"], ["-sqrt(1 - x0^2)"])]
upper2 = [ot.SymbolicFunction(["x0"], ["sqrt(1 - x0^2)"])]
value2 = algo.integrate(integrand2, a, b, lower2, upper2)[0]
print("2D ball volume with GaussLegendre=", value2)
ott.assert_almost_equal(value2, m.pi, 1e-3, 1e-3)

# Test the integrate(f, interval) overload (1D)
algo.setAlgorithm(ot.GaussKronrod(20, 1.0e-6, ot.GaussKronrodRule(ot.GaussKronrodRule.G3K7)))
f1d = ot.SymbolicFunction(["x"], ["sin(x)"])
value1d = algo.integrate(f1d, ot.Interval(-1.0, 1.0))
ref1d = -m.cos(1.0) + m.cos(-1.0)  # = 2*sin(1) via antiderivative
ott.assert_almost_equal(value1d[0], ref1d, 1e-6, 1e-6)

# Test the integrate(f, interval) overload (3D)
f3d = ot.SymbolicFunction(["x", "y", "z"], ["x * y * z"])
value3d = algo.integrate(f3d, ot.Interval([-1.0] * 3, [1.0] * 3))
# integral of x*y*z over [-1,1]^3 = 0 (odd function)
ott.assert_almost_equal(value3d[0], 0.0, 1e-6, 1e-6)

# Test accessors
assert algo.getAlgorithm() is not None

# Test validation: wrong number of lower bound functions
with ott.assert_raises(TypeError):
    integrand_bad = ot.SymbolicFunction(["x0", "x1", "x2"], ["1.0"])
    algo.integrate(integrand_bad, -1.0, 1.0, [], [])

# Test validation: mismatched lower/upper bound counts
with ott.assert_raises(TypeError):
    lower_bad = [ot.SymbolicFunction(["x0"], ["-1.0"])]
    upper_bad = []
    algo.integrate(integrand_bad, -1.0, 1.0, lower_bad, upper_bad)

# Test validation: wrong input dimension of a bound function
with ott.assert_raises(TypeError):
    lower_bad2 = [ot.SymbolicFunction(["x0", "x1"], ["-1.0"])]
    upper_bad2 = [ot.SymbolicFunction(["x0", "x1"], ["1.0"])]
    integrand_bad2 = ot.SymbolicFunction(["x0", "x1"], ["1.0"])
    algo.integrate(integrand_bad2, -1.0, 1.0, lower_bad2, upper_bad2)

# Test validation: wrong output dimension of a bound function
with ott.assert_raises(TypeError):
    lower_bad3 = [ot.SymbolicFunction(["x0"], ["-1.0", "0.0"])]
    upper_bad3 = [ot.SymbolicFunction(["x0"], ["1.0", "0.0"])]
    integrand_bad3 = ot.SymbolicFunction(["x0", "x1"], ["1.0"])
    algo.integrate(integrand_bad3, -1.0, 1.0, lower_bad3, upper_bad3)

# Test with check=False: mismatched lower/upper bound counts
with ott.assert_raises(TypeError):
    lower_bad4 = [ot.SymbolicFunction(["x0"], ["-1.0"])]
    upper_bad4 = [ot.SymbolicFunction(["x0"], ["1.0"]), ot.SymbolicFunction(["x0", "x1"], ["1.0"])]
    integrand_bad4 = ot.SymbolicFunction(["x0", "x1"], ["1.0"])
    algo.integrate(integrand_bad4, -1.0, 1.0, lower_bad4, upper_bad4, False)

# Test with check=False: bound count mismatch with integrand dimension
with ott.assert_raises(TypeError):
    lower_bad5 = [ot.SymbolicFunction(["x0"], ["-1.0"])]
    upper_bad5 = [ot.SymbolicFunction(["x0"], ["1.0"])]
    integrand_bad5 = ot.SymbolicFunction(["x0", "x1", "x2"], ["1.0"])
    algo.integrate(integrand_bad5, -1.0, 1.0, lower_bad5, upper_bad5, False)


# Test with a PythonFunction integrand (not parallel-safe): the local
# integrals are then evaluated serially
def py_integrand(x):
    return [x[0] * x[0] + x[1] * x[1]]


algo.setAlgorithm(ot.GaussKronrod(20, 1.0e-6, ot.GaussKronrodRule(ot.GaussKronrodRule.G3K7)))
py_f = ot.PythonFunction(2, 1, py_integrand)
lower_py = [ot.SymbolicFunction(["x0"], ["-sqrt(max(0.0, 1.0 - x0^2))"])]
upper_py = [ot.SymbolicFunction(["x0"], ["sqrt(max(0.0, 1.0 - x0^2))"])]
value_py = algo.integrate(py_f, a, b, lower_py, upper_py)[0]
print("2D ball of x^2+y^2 with PythonFunction=", value_py)
# integral of (x0^2+x1^2) over the unit disk = pi/2
ott.assert_almost_equal(value_py, m.pi / 2.0, 1e-3, 1e-3)

# Test repr and str
print("repr=", repr(algo))
print("str=", str(algo))

print("OK")
