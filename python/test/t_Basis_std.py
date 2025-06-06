#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

basis = ot.Basis()
print("basis =", basis)
basisSize = 3

phis = []
for j in range(basisSize):
    phis.append(ot.SymbolicFunction(["x"], ["x^" + str(j + 1)]))
basis = ot.Basis(phis)
print("basis =", basis)

assert basis.getSize() == 3, "wrong size"
assert basis.getInputDimension() == 1, "wrong input dim"
assert basis.getOutputDimension() == 1, "wrong output dim"
assert basis.getSubBasis([1, 2]).getSize() == 2, "wrong subsize"
assert basis.isFinite(), "!finite"
assert not basis.isOrthogonal(), "orthogonal"

degree = 2
poly = []
factory = ot.MonomialFunctionFactory()
for i in range(degree + 1):
    poly.append(factory.build(i))
with ott.assert_raises(RuntimeError):
    basis = ot.Basis(poly)

# check basis dimensions
f1 = ot.SymbolicFunction(["x", "y"], ["x+y"])
f2 = ot.SymbolicFunction(["x", "y"], ["x+y", "x-y"])
f3 = ot.SymbolicFunction(["x"], ["x+1"])
with ott.assert_raises(TypeError):
    b = ot.Basis([f1, f2])
with ott.assert_raises(TypeError):
    b = ot.Basis([f1, f3])
