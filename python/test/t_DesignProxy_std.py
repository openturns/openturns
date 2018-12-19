#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

basisSize = 3
sampleSize = 3

X = ot.Sample(sampleSize, 1)
for i in range(sampleSize):
    X[i, 0] = i + 1.0

Y = ot.Sample(sampleSize, 1)

phis = []
for j in range(basisSize):
    phis.append(ot.SymbolicFunction(['x'], ['x^' + str(j + 1)]))
basis = ot.Basis(phis)
for i in range(basisSize):
    print(ot.FunctionCollection(basis)[i](X))

proxy = ot.DesignProxy(X, basis)
full = range(basisSize)

design = proxy.computeDesign(full)
print(design)

proxy.setWeight([0.5] * sampleSize)
design = proxy.computeDesign(full)
print(design)

proxy = ot.DesignProxy(design)
full = range(basisSize)

design = proxy.computeDesign(full)
print(design)
