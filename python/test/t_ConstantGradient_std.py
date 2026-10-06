#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


inputDimension = 3
outputDimension = 2
# Constant term
constant = ot.Matrix(inputDimension, outputDimension)
constant[0, 0] = 1.0
constant[1, 0] = 2.0
constant[2, 0] = 5.0
constant[0, 1] = 7.0
constant[1, 1] = 9.0
constant[2, 1] = 3.0

myGradient = ot.ConstantGradient(constant)
myGradient.setName("constantGradient")
inPoint = ot.Point(inputDimension)
inPoint[0] = 7.0
inPoint[1] = 8.0
inPoint[2] = 9.0
outMatrix = myGradient.gradient(inPoint)
print("myGradient=", repr(myGradient))
print(myGradient.getName(), "( ", repr(inPoint), " ) = ", repr(outMatrix))

# Equality tests
g1 = ot.ConstantGradient(constant)
assert g1 == myGradient, "same constant should be equal"
assert not (g1 != myGradient), "same constant should not be different"
otherConstant = ot.Matrix(inputDimension, outputDimension)
otherConstant[0, 0] = 42.0
g2 = ot.ConstantGradient(otherConstant)
assert not (g1 == g2), "different constant should not be equal"
assert g1 != g2, "different constant should be different"

# Alternate constructors and accessors
defaultGradient = ot.ConstantGradient()
assert defaultGradient.getInputDimension() == 0
assert defaultGradient.getOutputDimension() == 0
_ = str(defaultGradient)
_ = repr(defaultGradient)
ott.assert_almost_equal(myGradient.getConstant(), constant)
assert myGradient.getInputDimension() == inputDimension
assert myGradient.getOutputDimension() == outputDimension
ott.assert_almost_equal(outMatrix, constant)
_ = str(myGradient)
_ = repr(myGradient)
assert myGradient.isActualImplementation()
assert myGradient == myGradient
assert not (myGradient == "dummy")
assert myGradient != "dummy"
assert not (
    myGradient == ot.ConstantHessian(ot.SymmetricTensor(inputDimension, outputDimension))
)
callsBefore = myGradient.getCallsNumber()
ott.assert_almost_equal(myGradient.gradient(inPoint), constant)
assert myGradient.getCallsNumber() == callsBefore + 1
assert myGradient.getParameter().getDimension() == 0
myGradient.setParameter([1.0])
assert myGradient.getParameter() == ot.Point([1.0])
myGradient.setParameter(ot.Point())
marginalGradient = myGradient.getMarginal(0)
assert marginalGradient.getOutputDimension() == 1
ott.assert_almost_equal(
    marginalGradient.gradient(inPoint), ot.Matrix([[1.0], [2.0], [5.0]])
)
fullMarginal = myGradient.getMarginal([0, 1])
assert fullMarginal == myGradient
with ott.assert_raises(Exception):
    myGradient.getMarginal(5)
with ott.assert_raises(Exception):
    myGradient.getMarginal([0, 0])
with ott.assert_raises(Exception):
    myGradient.gradient(ot.Point(inputDimension + 1))
with ott.assert_raises(Exception):
    myGradient.gradient(ot.Point(inputDimension - 1))
