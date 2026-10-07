#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

inputDimension = 3
outputDimension = 2
# Center
center = ot.Point(inputDimension)
center[0] = -1
center[1] = 0.5
center[2] = 1
# Constant term
constant = ot.Point(outputDimension)
constant[0] = -1.0
constant[1] = 2.0
# Linear term
linear = ot.Matrix(outputDimension, inputDimension)
linear[0, 0] = 1.0
linear[1, 0] = 2.0
linear[0, 1] = 3.0
linear[1, 1] = 4.0
linear[0, 2] = 5.0
linear[1, 2] = 6.0

# myFunction = linear * (X- center) + constant
myFunction = ot.LinearFunction(center, constant, linear)
myFunction.setName("linearFunction")
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

# Alternate constructors and accessors
defaultFunction = ot.LinearFunction()
assert defaultFunction.getInputDimension() == 0
assert defaultFunction.getOutputDimension() == 0
_ = str(defaultFunction)
_ = repr(defaultFunction)
ott.assert_almost_equal(outPoint, [69.5, 96.0])
ott.assert_almost_equal(myFunction.gradient(inPoint), linear.transpose())
ott.assert_almost_equal(
    myFunction.hessian(inPoint), ot.SymmetricTensor(inputDimension, outputDimension)
)
ott.assert_almost_equal(myFunction.getEvaluation()(inPoint), outPoint)
ott.assert_almost_equal(
    myFunction.getGradient().gradient(inPoint), linear.transpose()
)
ott.assert_almost_equal(
    myFunction.getHessian().hessian(inPoint),
    ot.SymmetricTensor(inputDimension, outputDimension),
)
sampleOut = myFunction(ot.Sample([inPoint, inPoint]))
assert sampleOut.getSize() == 2
ott.assert_almost_equal(sampleOut, [[69.5, 96.0], [69.5, 96.0]])
assert myFunction.isLinear()
assert myFunction.isLinearlyDependent(0)
_ = str(myFunction)
_ = repr(myFunction)
assert myFunction == myFunction
assert not (myFunction == "dummy")
assert myFunction != "dummy"
with ott.assert_raises(Exception):
    myFunction == ot.ConstantFunction(3, [1.0, 2.0])
assert myFunction.getCallsNumber() >= 1
assert myFunction.getParameter().getDimension() == 0
marginal1 = myFunction.getMarginal(1)
assert marginal1.getOutputDimension() == 1
ott.assert_almost_equal(marginal1(inPoint), [outPoint[1]])
with ott.assert_raises(Exception):
    myFunction([1.0])
with ott.assert_raises(Exception):
    myFunction(ot.Sample(2, inputDimension + 1))
with ott.assert_raises(Exception):
    myFunction.gradient([1.0])
with ott.assert_raises(Exception):
    myFunction.hessian([1.0])
with ott.assert_raises(Exception):
    myFunction.getMarginal(5)
with ott.assert_raises(Exception):
    myFunction.getMarginal([0, 0])
with ott.assert_raises(Exception):
    ot.LinearFunction(center, [1.0, 2.0, 3.0], linear)
with ott.assert_raises(Exception):
    ot.LinearFunction(ot.Point(inputDimension + 1), constant, linear)
