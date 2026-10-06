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
constant = ot.Matrix(inputDimension, outputDimension)
constant[0, 0] = 1.0
constant[1, 0] = 2.0
constant[2, 0] = 5.0
constant[0, 1] = 7.0
constant[1, 1] = 9.0
constant[2, 1] = 3.0
# Linear term
linear = ot.SymmetricTensor(inputDimension, outputDimension)
linear[0, 0, 0] = 7.0
linear[0, 0, 1] = -7.0
linear[0, 1, 0] = 8.0
linear[0, 1, 1] = -8.0
linear[0, 2, 0] = 9.0
linear[0, 2, 1] = -9.0
linear[1, 0, 0] = 8.0
linear[1, 0, 1] = -8.0
linear[1, 1, 0] = 10.0
linear[1, 1, 1] = -10.0
linear[1, 2, 0] = 11.0
linear[1, 2, 1] = -11.0
linear[2, 0, 0] = 9.0
linear[2, 0, 1] = -9.0
linear[2, 1, 0] = 11.0
linear[2, 1, 1] = -11.0
linear[2, 2, 0] = 12.0
linear[2, 2, 1] = -12.0

# myFunction = linear * (X- center) + constant
myGradient = ot.LinearGradient(center, constant, linear)
myGradient.setName("linearGradient")
inPoint = ot.Point(inputDimension)
inPoint[0] = 7.0
inPoint[1] = 8.0
inPoint[2] = 9.0
outMatrix = myGradient.gradient(inPoint)
print("myGradient=", repr(myGradient))
print(myGradient.getName(), "( ", repr(inPoint), " ) = ", repr(outMatrix))

# Alternate constructors and accessors
defaultGradient = ot.LinearGradient()
assert defaultGradient.getInputDimension() == 0
assert defaultGradient.getOutputDimension() == 0
_ = str(defaultGradient)
_ = repr(defaultGradient)
assert myGradient.getCenter() == center
ott.assert_almost_equal(myGradient.getConstant(), constant)
ott.assert_almost_equal(myGradient.getLinear(), linear)
assert myGradient.getInputDimension() == inputDimension
assert myGradient.getOutputDimension() == outputDimension
expectedGradient = ot.Matrix(inputDimension, outputDimension)
expectedGradient[0, 0] = 189.0
expectedGradient[1, 0] = 229.0
expectedGradient[2, 0] = 255.5
expectedGradient[0, 1] = -181.0
expectedGradient[1, 1] = -218.0
expectedGradient[2, 1] = -247.5
ott.assert_almost_equal(outMatrix, expectedGradient)
_ = str(myGradient)
_ = repr(myGradient)
assert myGradient.isActualImplementation()
assert myGradient == myGradient
assert not (myGradient == "dummy")
assert myGradient != "dummy"
with ott.assert_raises(Exception):
    myGradient == ot.ConstantGradient()
callsBefore = myGradient.getCallsNumber()
ott.assert_almost_equal(myGradient.gradient(inPoint), expectedGradient)
assert myGradient.getCallsNumber() == callsBefore + 1
assert myGradient.getParameter().getDimension() == 0
myGradient.setParameter([1.0])
assert myGradient.getParameter() == ot.Point([1.0])
myGradient.setParameter(ot.Point())
marginalGradient = myGradient.getMarginal(0)
assert marginalGradient.getOutputDimension() == 1
ott.assert_almost_equal(
    marginalGradient.gradient(inPoint), ot.Matrix([[189.0], [229.0], [255.5]])
)
assert myGradient.getMarginal([0, 1]) == myGradient
with ott.assert_raises(Exception):
    myGradient.getMarginal(5)
with ott.assert_raises(Exception):
    myGradient.getMarginal([0, 0])
with ott.assert_raises(Exception):
    myGradient.gradient(ot.Point(inputDimension + 1))
with ott.assert_raises(Exception):
    myGradient.gradient(ot.Point(inputDimension - 1))
with ott.assert_raises(Exception):
    ot.LinearGradient(center, ot.Matrix(2, outputDimension), linear)
with ott.assert_raises(Exception):
    ot.LinearGradient(ot.Point(inputDimension + 1), constant, linear)
