#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

basis = ot.Basis()
print("basis =", basis)
basisSize = 3

phis = []
for j in range(basisSize):
    phis.append(ot.SymbolicFunction(["x"], ["x^" + str(j + 1)]))
basis = ot.Basis(phis)
print("basis =", basis)

print(basis.getSize())
print(basis.getSubBasis([1, 2]))
print(basis.isFinite())
print(basis.isOrthogonal())
# print(basis[1])
# print(basis[0:2])

# basis[1] = ot.Function(['x'], ['z'], ['x^42'])
# print('basis =', basis)
# basis[0:2] = basis[1:3]
# print('basis =', basis)

try:
    degree = 2
    poly = []
    factory = ot.MonomialFunctionFactory()
    for i in range(degree + 1):
        poly.append(factory.build(i))
    basis = ot.Basis(poly)
except RuntimeError:
    print("ok")
