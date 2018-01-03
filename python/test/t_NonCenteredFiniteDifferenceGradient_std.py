#! /usr/bin/env python

from __future__ import print_function
import openturns as ot


eps = 1e-4
# Instance creation
myFunc = ot.SymbolicFunction(['x1', 'x2'], [
    'x1*sin(x2)', 'cos(x1+x2)', '(x2+1)*exp(x1-2*x2)'])
epsilon = ot.Point(myFunc.getInputDimension(), eps)
inPoint = ot.Point(epsilon.getDimension(), 1.0)
myGradient = ot.NonCenteredFiniteDifferenceGradient(
    epsilon, myFunc.getEvaluation())

print("myGradient=", repr(myGradient))
print("myFunc.gradient(", repr(inPoint), ")=",
      repr(myFunc.gradient(inPoint)))
print("myGradient.gradient(", repr(inPoint), ")=",
      repr(myGradient.gradient(inPoint)))
# Substitute the gradient
# DEBUG ici
myFunc.setGradient(
    ot.NonCenteredFiniteDifferenceGradient(myGradient))
print("myFunc.gradient(", repr(inPoint), ")=", repr(
    myFunc.gradient(inPoint)), " (after substitution)")
