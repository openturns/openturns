#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

dim = 3
f = ot.IdentityFunction(dim)
print(f)

assert f.getInputDimension() == dim, "input dim"
assert f.getOutputDimension() == dim, "output dim"

x = ot.Point([i + 1.0 for i in range(dim)])
y = f(x)
print(x, y)
assert x == y, "wrong result"
