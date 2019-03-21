#! /usr/bin/env python

from __future__ import print_function
import openturns as ot


eps = 0.2
# Instance creation
myFunc = ot.SymbolicFunction(['x1', 'x2'], [
    'x1*sin(x2)', 'cos(x1+x2)', '(x2+1)*exp(x1-2*x2)'])
center = ot.Point(myFunc.getInputDimension())
for i in range(center.getDimension()):
    center[i] = 1.0 + i
myTaylor = ot.LinearTaylor(center, myFunc)
myTaylor.run()
responseSurface = ot.Function(myTaylor.getMetaModel())
print("myTaylor=", repr(myTaylor))
print("responseSurface=", repr(responseSurface))
print("myFunc(", repr(center), ")=", repr(myFunc(center)))
print("responseSurface(", repr(center), ")=",
      repr(responseSurface(center)))
inPoint = ot.Point(center)
inPoint[0] += eps
inPoint[1] -= eps / 2
print("myFunc(", repr(inPoint), ")=", repr(myFunc(inPoint)))
print("responseSurface(", repr(inPoint), ")=",
      repr(responseSurface(inPoint)))
