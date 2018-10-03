#! /usr/bin/env python

from __future__ import print_function
from openturns import *
import sys

TESTPREAMBLE()

mesh = RegularGrid(0.0, 0.1, 11)


class FUNC(OpenTURNSPythonPointToFieldFunction):

    def __init__(self):
        super(FUNC, self).__init__(2, mesh, 2)
        self.setInputDescription(['R', 'S'])
        self.setOutputDescription(['T', 'U'])
        self.mesh_ = RegularGrid(0.0, 0.1, 11)

    def _exec(self, X):
        size = self.mesh_.getVerticesNumber()
        values = [Point(X) * i for i in range(size)]
        Y = Field(self.mesh_, values)
        return Y


F = FUNC()
print('in_dim=' + str(F.getInputDimension())
      + ' out_dim=' + str(F.getOutputDimension()))

print((F((10.0, 5.0))))

print((F(((10.0, 5.0), (6.0, 7.0)))))


# Instance creation
myFunc = PointToFieldFunction(F)

# Copy constructor
newFunc = PointToFieldFunction(myFunc)

print(('myFunc input dimension= ' + str(myFunc.getInputDimension())))
print(('myFunc output dimension= ' + str(myFunc.getOutputDimension())))

inPt = Point(2, 2.0)
print((repr(inPt)))

outPt = myFunc(inPt)
print((repr(outPt)))

outPt = myFunc((10.0, 11.0))
print((repr(outPt)))

inSample = Sample(10, 2)
for i in range(10):
    inSample[i] = Point((i, i))
print((repr(inSample)))

outSample = myFunc(inSample)
print((repr(outSample)))

outSample = myFunc(((100.0, 100.0), (101.0, 101.0), (102.0, 102.0)))
print((repr(outSample)))
