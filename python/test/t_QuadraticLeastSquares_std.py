#! /usr/bin/env python

from __future__ import print_function
import openturns as ot


myFunc = ot.SymbolicFunction(['x1', 'x2'], [
    'x1*sin(x2)', 'cos(x1+x2)', '(x2+1)*exp(x1-2*x2)'])
data = ot.Sample(9, myFunc.getInputDimension())
point = ot.Point(myFunc.getInputDimension())
point[0] = 0.5
point[1] = 0.5
data[0] = point
point[0] = -1
point[1] = -1
data[1] = point
point[0] = -1
point[1] = 1
data[2] = point
point[0] = 1
point[1] = -1
data[3] = point
point[0] = 1
point[1] = 1
data[4] = point
point[0] = -0.5
point[1] = -0.5
data[5] = point
point[0] = -0.5
point[1] = 0.5
data[6] = point
point[0] = 0.5
point[1] = -0.5
data[7] = point
point[0] = 0.5
point[1] = 0.5
data[8] = point
myLeastSquares = ot.QuadraticLeastSquares(data, myFunc)
myLeastSquares.run()
responseSurface = myLeastSquares.getMetaModel()
print("myLeastSquares=", repr(myLeastSquares))
print("responseSurface=", repr(responseSurface))
inPoint = ot.Point(myFunc.getInputDimension(), 0.2)
print("myFunc(", repr(inPoint), ")=", repr(myFunc(inPoint)))
print("responseSurface(", repr(inPoint), ")=",
      repr(responseSurface(inPoint)))
dataOut = myFunc(data)
myLeastSquares = ot.QuadraticLeastSquares(data, dataOut)
myLeastSquares.run()
responseSurface = myLeastSquares.getMetaModel()
print("myLeastSquares=", repr(myLeastSquares))
print("responseSurface=", repr(responseSurface))
inPoint = ot.Point(myFunc.getInputDimension(), 0.2)
print("myFunc(", repr(inPoint), ")=", repr(myFunc(inPoint)))
print("responseSurface(", repr(inPoint), ")=",
      repr(responseSurface(inPoint)))
