#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import openturns.experimental as otexp

ot.TESTPREAMBLE()

# Check the parameter constructor
mean = [0.0] * 2
std = [1.0] * 2
R = ot.CorrelationMatrix([[1.0, 0.8], [0.8, 1.0]])
distribution = ot.Normal(mean, std, R)
# These functions are obtained by using the Gram-Schmidt algorithm to make orthonormal
# wrt the distribution the first 2D tensorized Hermite polynomials
f0 = ot.SymbolicFunction(["x0", "x1"], ["1.0"])
f1 = ot.SymbolicFunction(["x0", "x1"], ["x0"])
f2 = ot.SymbolicFunction(["x0", "x1"], ["-4.0 * x0 / 3.0 + 5.0 * x1 / 3.0"])
f3 = ot.SymbolicFunction(["x0", "x1"], ["-1.0 / sqrt(2.0) + x0^2 / sqrt(2)"])
f4 = ot.SymbolicFunction(
    ["x0", "x1"],
    [
        "-4.0 / 3.0 - 1.885618083165693 * (-1.0 / sqrt(2.0) + x0^2 / sqrt(2.0)) + 5.0 * x0 * x1 / 3.0"
    ],
)
f5 = ot.SymbolicFunction(
    ["x0", "x1"],
    [
        "2.5141574442076222 + 16.0 / 9.0 * (-1.0 / sqrt(2.0) + x0^2 / sqrt(2.0)) - 3.142696805266918 * x0 * x1 + 25.0 / 9.0 * (-1.0 / sqrt(2.0) + x1^2 / sqrt(2.0))"
    ],
)
initialBasis = [f0, f1, f2, f3, f4, f5]
factory = otexp.FiniteOrthogonalFunctionFactory(initialBasis, distribution)
x = [0.5] * 2
kMax = len(initialBasis)
distribution = factory.getMeasure()
initialBasis = factory.getFunctionsCollection()
factory.setFunctionsCollection(initialBasis)
print("Factory=", factory)
print("Factory=", repr(factory))
functions = list()
for k in range(kMax):
    fk = factory.build(k)
    functions.append(fk)
    value = fk(x)
    print(f"FiniteOrthogonalFunction_{k}({x})={value}")

# Check orthonormality
M = ot.SymmetricMatrix(kMax)
integrationAlgo = ot.GaussLegendre([48] * 2)
for m in range(kMax):
    for n in range(m + 1):

        def wrapper(x):
            return functions[m](x) * functions[n](x)[0] * distribution.computePDF(x)

        kernel = ot.PythonFunction(distribution.getDimension(), 1, wrapper)
        value = integrationAlgo.integrate(kernel, distribution.getRange())[0]
        if abs(value) >= 1.0e-6:
            M[m, n] = value
ott.assert_almost_equal(M, ot.IdentityMatrix(kMax))
