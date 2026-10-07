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
linear = ot.Matrix(inputDimension, outputDimension)
linear[0, 0] = 1.0
linear[1, 0] = 2.0
linear[2, 0] = 3.0
linear[0, 1] = 4.0
linear[1, 1] = 5.0
linear[2, 1] = 6.0

# myFunction = linear * (X- center) + constant
myFunction = ot.LinearEvaluation(center, constant, linear)
myFunction.setName("linearFunction")
inPoint = ot.Point(inputDimension)
inPoint[0] = 7.0
inPoint[1] = 8.0
inPoint[2] = 9.0
outPoint = myFunction(inPoint)
print("myFunction=", repr(myFunction))
print(myFunction.getName(), "( ", repr(inPoint), " ) = ", repr(outPoint))

# Alternate constructors and accessors
defaultEvaluation = ot.LinearEvaluation()
assert defaultEvaluation.getInputDimension() == 0
assert defaultEvaluation.getOutputDimension() == 0
_ = str(defaultEvaluation)
_ = repr(defaultEvaluation)
assert myFunction.getCenter() == center
ott.assert_almost_equal(myFunction.getConstant(), constant)
ott.assert_almost_equal(myFunction.getLinear(), linear)
assert myFunction.getInputDimension() == inputDimension
assert myFunction.getOutputDimension() == outputDimension
assert myFunction.isLinear()
assert myFunction.isLinearlyDependent(0)
assert myFunction.isLinearlyDependent(2)
ott.assert_almost_equal(outPoint, [46.0, 119.5])
_ = str(myFunction)
_ = repr(myFunction)
assert myFunction.isActualImplementation()
assert myFunction == myFunction
assert not (myFunction == "dummy")
assert myFunction != "dummy"
with ott.assert_raises(Exception):
    myFunction == ot.ConstantEvaluation()
secondPoint = ot.Point([1.0, 2.0, 3.0])
sampleOut = myFunction(ot.Sample([inPoint, secondPoint]))
assert sampleOut.getSize() == 2
ott.assert_almost_equal(sampleOut[0], outPoint)
ott.assert_almost_equal(sampleOut[1], myFunction(secondPoint))
callsBefore = myFunction.getCallsNumber()
myFunction(inPoint)
assert myFunction.getCallsNumber() == callsBefore + 1
assert myFunction.getParameterDimension() == 0
assert myFunction.getParameter().getDimension() == 0
myFunction.setParameter([0.5])
assert myFunction.getParameter() == ot.Point([0.5])
_ = myFunction.parameterGradient(inPoint)
myFunction.setParameter(ot.Point())
marginal0 = myFunction.getMarginal(0)
assert marginal0.getOutputDimension() == 1
ott.assert_almost_equal(marginal0(inPoint), [outPoint[0]])
assert myFunction.getMarginal([0, 1]) == myFunction
with ott.assert_raises(Exception):
    myFunction.getMarginal(5)
with ott.assert_raises(Exception):
    myFunction.getMarginal([0, 0])
with ott.assert_raises(Exception):
    ot.LinearEvaluation(center, [1.0, 2.0, 3.0], linear)
with ott.assert_raises(Exception):
    ot.LinearEvaluation(ot.Point(inputDimension + 1), constant, linear)
with ott.assert_raises(Exception):
    myFunction(ot.Point(inputDimension + 1))
with ott.assert_raises(Exception):
    myFunction(ot.Sample(2, inputDimension + 1))
with ott.assert_raises(Exception):
    myFunction.isLinearlyDependent(inputDimension + 1)
