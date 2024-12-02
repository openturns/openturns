#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

inputDimension = 2
constant = [1, 2, 3]

f = otexp.ConstantFunction(inputDimension, constant)

print(f)
print(repr(f))

assert f.getInputDimension() == inputDimension
assert f.getOutputDimension() == len(constant)
assert f == f

x = [5, 6]
y = f(x)
print(y)
ott.assert_almost_equal(y, constant)

dx = f.gradient(x)
ott.assert_almost_equal(dx, ot.Matrix(inputDimension, len(constant)))

dx2 = f.hessian(x)
ott.assert_almost_equal(dx2, ot.SymmetricTensor(inputDimension, len(constant)))

f02 = f.getMarginal([0, 2])
print(f02)
y02 = f02(x)
ott.assert_almost_equal(y02, [1, 3])
