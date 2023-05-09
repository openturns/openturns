#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

basisSize = 3
sampleSize = 3

X = ot.Sample(sampleSize, 1)
for i in range(sampleSize):
    X[i, 0] = i + 1.0

Y = ot.Sample(sampleSize, 1)

phis = []
for j in range(basisSize):
    phis.append(ot.SymbolicFunction(["x"], ["x^" + str(j + 1)]))
basis = ot.Basis(phis)
for i in range(basisSize):
    print(ot.FunctionCollection(basis)[i](X))

proxy = ot.DesignProxy(X, basis)
full = range(basisSize)

design = proxy.computeDesign(full)
print(design)

try:
    proxy.computeDesign(range(5))
    assert False, "should not go here"
except TypeError:
    print("ok")

partial = [0, 2]
print(proxy.getBasis(partial))

proxy = ot.DesignProxy(ot.Matrix(design))
full = range(basisSize)

design = proxy.computeDesign(full)
print(design)

try:
    proxy.computeDesign(range(5))
    assert False, "should not go here"
except TypeError:
    print("ok")
