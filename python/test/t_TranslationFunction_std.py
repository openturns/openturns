#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

inputDimension = 3

myFunction = TranslationFunction([2.0, 3.0, -4.0])
myFunction.setName("translationFunction")
inPoint = Point(inputDimension)
inPoint[0] = 7.0
inPoint[1] = 8.0
inPoint[2] = 9.0
outPoint = myFunction(inPoint)
print("myFunction=", repr(myFunction))
print(myFunction.getName(), "( ", repr(inPoint), " ) = ", repr(outPoint))
print(myFunction.getName(), ".gradient( ", repr(inPoint), " ) = ",
      repr(myFunction.gradient(inPoint)))
print(myFunction.getName(), ".hessian( ", repr(inPoint), " ) = ",
      repr(myFunction.hessian(inPoint)))
