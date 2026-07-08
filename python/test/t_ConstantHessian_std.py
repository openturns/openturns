#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


inputDimension = 3
outputDimension = 2

# Constant term
constant = ot.SymmetricTensor(inputDimension, outputDimension)
constant[0, 0, 0] = 7.0
constant[0, 0, 1] = -7.0
constant[0, 1, 0] = 8.0
constant[0, 1, 1] = -8.0
constant[0, 2, 0] = 9.0
constant[0, 2, 1] = -9.0
constant[1, 0, 0] = 8.0
constant[1, 0, 1] = -8.0
constant[1, 1, 0] = 10.0
constant[1, 1, 1] = -10.0
constant[1, 2, 0] = 11.0
constant[1, 2, 1] = -11.0
constant[2, 0, 0] = 9.0
constant[2, 0, 1] = -9.0
constant[2, 1, 0] = 11.0
constant[2, 1, 1] = -11.0
constant[2, 2, 0] = 12.0
constant[2, 2, 1] = -12.0

myHessian = ot.ConstantHessian(constant)
myHessian.setName("constantHessian")
inPoint = ot.Point(inputDimension)
inPoint[0] = 7.0
inPoint[1] = 8.0
inPoint[2] = 9.0
outTensor = myHessian.hessian(inPoint)
print("myHessian=", repr(myHessian))
print(myHessian.getName(), "( ", repr(inPoint), " ) = ", repr(outTensor))

# Equality tests
h1 = ot.ConstantHessian(constant)
assert h1 == myHessian, "same constant should be equal"
assert not (h1 != myHessian), "same constant should not be different"
otherConstant = ot.SymmetricTensor(inputDimension, outputDimension)
otherConstant[0, 0, 0] = 42.0
h2 = ot.ConstantHessian(otherConstant)
assert not (h1 == h2), "different constant should not be equal"
assert h1 != h2, "different constant should be different"
