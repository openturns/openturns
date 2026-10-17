#! /usr/bin/env python

import math

import openturns as ot
import openturns.testing as ott


eps = 1e-2
# Instance creation
myFunc = ot.SymbolicFunction(
    ["x1", "x2"], ["x1*sin(x2)", "cos(x1+x2)", "(x2+1)*exp(x1-2*x2)"]
)
print("myFunc (before substitution) = ", repr(myFunc))
epsilon = ot.Point(myFunc.getInputDimension(), eps)
inPoint = ot.Point(epsilon.getDimension(), 1.0)
myGradient = ot.CenteredFiniteDifferenceGradient(epsilon, myFunc.getEvaluation())

print("myGradient=", repr(myGradient))
print("myFunc.gradient(", repr(inPoint), ")=", repr(myFunc.gradient(inPoint)))
print("myGradient.gradient(", repr(inPoint), ")=", repr(myGradient.gradient(inPoint)))

# Substitute the gradient
myFunc.setGradient(myGradient)
print("myFunc (after substitution) = ", repr(myFunc))

print(
    "myFunc.gradient(",
    repr(inPoint),
    ")=",
    repr(myFunc.gradient(inPoint)),
    " (after substitution)",
)

# Base class FiniteDifferenceGradient coverage
evaluation = myFunc.getEvaluation()
gradient = ot.FiniteDifferenceGradient(epsilon, evaluation)
print("base epsilon=", gradient.getEpsilon())
gradient = ot.FiniteDifferenceGradient(eps, evaluation)
print("base epsilon=", gradient.getEpsilon())
gradient = ot.FiniteDifferenceGradient(ot.ConstantStep(epsilon), evaluation)
print("base evaluation=", gradient.getEvaluation().getClassName())
print(
    "base inputDim=",
    gradient.getInputDimension(),
    "outputDim=",
    gradient.getOutputDimension(),
)
print("base repr=", repr(gradient))
with ott.assert_raises(ValueError):
    ot.FiniteDifferenceGradient(ot.Point(3, eps), evaluation)
with ott.assert_raises(TypeError):
    ot.FiniteDifferenceGradient(ot.Point([eps, 0.0]), evaluation)
with ott.assert_raises(TypeError):
    ot.FiniteDifferenceGradient(0.0, evaluation)
with ott.assert_raises(ValueError):
    ot.FiniteDifferenceGradient(ot.ConstantStep(ot.Point(3, eps)), evaluation)
with ott.assert_raises(TypeError):
    ot.FiniteDifferenceGradient(
        ot.ConstantStep(ot.Point([eps, 0.0])), evaluation
    )
with ott.assert_raises(RuntimeError):
    gradient.gradient(inPoint)
assert gradient == ot.FiniteDifferenceGradient(epsilon, evaluation)
assert gradient != ot.FiniteDifferenceGradient(ot.Point(2, 1e-5), evaluation)
gradient.setFiniteDifferenceStep(ot.ConstantStep(ot.Point(2, 1e-5)))
print("base step=", gradient.getFiniteDifferenceStep().getEpsilon())
print("base default epsilon=", ot.FiniteDifferenceGradient().getEpsilon())
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("finiteDifferenceGradient.xml"))
study.add("gradient", gradient)
study.save()
loaded = ot.Study()
loaded.setStorageManager(ot.XMLStorageManager("finiteDifferenceGradient.xml"))
loaded.load()
assert loaded.hasObject("gradient")

# Per-output finite difference steps, see issue #3284: the outputs have
# very different scales, no single step fits both
stiffFunc = ot.SymbolicFunction(["x"], ["x^3", "sin(1e6*x)"])
stiffPoint = [1.0]
stiffRef = [3.0, 1.0e6 * math.cos(1.0e6)]
stiffGradient = ot.CenteredFiniteDifferenceGradient(
    ot.ConstantStep([1.0e-5]), stiffFunc.getEvaluation()
)
stiffGradient.setFiniteDifferenceStep(1, ot.ConstantStep([1.0e-9]))
stiffValue = stiffGradient.gradient(stiffPoint)
ott.assert_almost_equal(stiffValue[0, 0], stiffRef[0], 1e-5, 1e-5)
ott.assert_almost_equal(stiffValue[0, 1], stiffRef[1], 1e-5, 1e-5)
# broadcast setter still covers all outputs at once
stiffGradient.setFiniteDifferenceStep(ot.ConstantStep([1.0e-5]))
ott.assert_almost_equal(
    list(stiffGradient.getFiniteDifferenceStep(0).getEpsilon()), [1.0e-5]
)
ott.assert_almost_equal(
    list(stiffGradient.getFiniteDifferenceStep(1).getEpsilon()), [1.0e-5]
)
# Out-of-range output indices are rejected by both setter and getter
with ott.assert_raises(Exception):
    stiffGradient.setFiniteDifferenceStep(99, ot.ConstantStep([1.0e-5]))
with ott.assert_raises(Exception):
    stiffGradient.getFiniteDifferenceStep(99)
