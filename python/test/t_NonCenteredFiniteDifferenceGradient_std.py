#! /usr/bin/env python

import math

import openturns as ot
import openturns.testing as ott


eps = 1e-4
# Instance creation
myFunc = ot.SymbolicFunction(
    ["x1", "x2"], ["x1*sin(x2)", "cos(x1+x2)", "(x2+1)*exp(x1-2*x2)"]
)
epsilon = ot.Point(myFunc.getInputDimension(), eps)
inPoint = ot.Point(epsilon.getDimension(), 1.0)
myGradient = ot.NonCenteredFiniteDifferenceGradient(epsilon, myFunc.getEvaluation())

print("myGradient=", repr(myGradient))
print("myFunc.gradient(", repr(inPoint), ")=", repr(myFunc.gradient(inPoint)))
print("myGradient.gradient(", repr(inPoint), ")=", repr(myGradient.gradient(inPoint)))
# Substitute the gradient
# DEBUG ici
myFunc.setGradient(ot.NonCenteredFiniteDifferenceGradient(myGradient))
print(
    "myFunc.gradient(",
    repr(inPoint),
    ")=",
    repr(myFunc.gradient(inPoint)),
    " (after substitution)",
)

# Per-output finite difference steps, see issue #3284
stiffFunc = ot.SymbolicFunction(["x"], ["x^3", "sin(1e6*x)"])
stiffPoint = [1.0]
stiffRef = [3.0, 1.0e6 * math.cos(1.0e6)]
stiffGradient = ot.NonCenteredFiniteDifferenceGradient(
    ot.ConstantStep([1.0e-5]), stiffFunc.getEvaluation()
)
stiffGradient.setFiniteDifferenceStep(1, ot.ConstantStep([1.0e-10]))
stiffValue = stiffGradient.gradient(stiffPoint)
ott.assert_almost_equal(stiffValue[0, 0], stiffRef[0], 1e-3, 1e-3)
ott.assert_almost_equal(stiffValue[0, 1], stiffRef[1], 1e-3, 1e-3)
