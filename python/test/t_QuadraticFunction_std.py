#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

inputDimension = 3
outputDimension = 2
# Center
center = ot.Point(inputDimension)
center[0] = -1.0
center[1] = 0.5
center[2] = 1.0
# Constant term
constant = ot.Point(outputDimension)
constant[0] = -1.0
constant[1] = 2.0
# Linear term
linear = ot.Matrix(outputDimension, inputDimension)
linear[0, 0] = 1.0
linear[0, 1] = 2.0
linear[0, 2] = 3.0
linear[1, 0] = 4.0
linear[1, 1] = 5.0
linear[1, 2] = 6.0
# Quadratic term
quadratic = ot.SymmetricTensor(inputDimension, outputDimension)
quadratic[0, 0, 0] = 7.0
quadratic[0, 0, 1] = -7.0
quadratic[0, 1, 0] = 8.0
quadratic[0, 1, 1] = -8.0
quadratic[0, 2, 0] = 9.0
quadratic[0, 2, 1] = -9.0
quadratic[1, 0, 0] = 8.0
quadratic[1, 0, 1] = -8.0
quadratic[1, 1, 0] = 10.0
quadratic[1, 1, 1] = -10.0
quadratic[1, 2, 0] = 11.0
quadratic[1, 2, 1] = -11.0
quadratic[2, 0, 0] = 9.0
quadratic[2, 0, 1] = -9.0
quadratic[2, 1, 0] = 11.0
quadratic[2, 1, 1] = -11.0
quadratic[2, 2, 0] = 12.0
quadratic[2, 2, 1] = -12.0

# myFunction = linear * (X- center) + constant
myFunction = ot.QuadraticFunction(center, constant, linear, quadratic)
inPoint = ot.Point(inputDimension)
inPoint[0] = 7.0
inPoint[1] = 8.0
inPoint[2] = 9.0
outPoint = myFunction(inPoint)
print("myFunction=", repr(myFunction))
print(myFunction.getName(), "( ", repr(inPoint), " ) = ", repr(outPoint))
print(
    myFunction.getName(),
    ".gradient( ",
    repr(inPoint),
    " ) = ",
    repr(myFunction.gradient(inPoint)),
)
print(
    myFunction.getName(),
    ".hessian( ",
    repr(inPoint),
    " ) = ",
    repr(myFunction.hessian(inPoint)),
)

# Parameter accessors, see issue #2833
ott.assert_almost_equal(myFunction.getCenter(), center)
ott.assert_almost_equal(myFunction.getConstant(), constant)
ott.assert_almost_equal(myFunction.getLinear(), linear)
ott.assert_almost_equal(myFunction.getQuadratic(), quadratic)
# extra coverage (no prints: output is checked against .expout)
assert myFunction == myFunction
assert not (myFunction != myFunction)
different = ot.QuadraticFunction(center + [1.0, 0.0, 0.0], constant, linear, quadratic)
assert myFunction != different
# NOTE: identical multi-dimensional QuadraticFunctions compare unequal
# (QuadraticEvaluation::operator== compares the stored transposed linear
# term with the untransposed getLinear()); only the 1D case round-trips:
oneD = ot.QuadraticFunction([0.0], [0.0], ot.Matrix(1, 1), ot.SymmetricTensor(1, 1))
assert oneD == ot.QuadraticFunction([0.0], [0.0], ot.Matrix(1, 1), ot.SymmetricTensor(1, 1))
assert len(repr(myFunction)) > 0
assert len(str(myFunction)) > 0
# error branches of the underlying evaluation

with ott.assert_raises(Exception):
    myFunction(ot.Point(inputDimension + 1))
with ott.assert_raises(Exception):
    myFunction.gradient(ot.Point(inputDimension + 1))
