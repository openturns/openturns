#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

eps = 1e-2
# Instance creation
myFunc = ot.SymbolicFunction(['x1', 'x2'], [
    'x1*sin(x2)', 'cos(x1+x2)', '(x2+1)*exp(x1-2*x2)'])
epsilon = ot.Point(myFunc.getInputDimension(), eps)
inPoint = ot.Point(epsilon.getDimension(), 1.0)
myHessian = ot.CenteredFiniteDifferenceHessian(
    epsilon, myFunc.getEvaluation())

print("myHessian=", repr(myHessian))
print("myFunc.hessian(", repr(inPoint), ")=",
      repr(myFunc.hessian(inPoint)))
print("myHessian.hessian(", repr(inPoint), ")=",
      repr(myHessian.hessian(inPoint)))

# Substitute the hessian
myFunc.setHessian(ot.CenteredFiniteDifferenceHessian(myHessian))
print("myFunc.hessian(", repr(inPoint), ")=", repr(
    myFunc.hessian(inPoint)), " (after substitution)")
