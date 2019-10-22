#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

mesh = ot.RegularGrid(0.0, 0.1, 11)


def f(X):
    size = 11
    Y = [ot.Point(X)*i for i in range(size)]
    return Y


inputDim = 2
outputDim = 2
function = ot.PythonPointToFieldFunction(inputDim, mesh, outputDim, f)
function.setInputDescription(['u1', 'u2'])
function.setOutputDescription(['v1', 'v2'])

# freeze u1=5.0
parametric = ot.ParametricPointToFieldFunction(function, [0], [5.0])

# properties
print ('dim=', parametric.getInputDimension(), parametric.getOutputDimension())
print ('description=', parametric.getInputDescription(),
       parametric.getOutputDescription())
print ('mesh=', parametric.getOutputMesh())

# op(Point)
x = [1.0]
y = parametric(x)
print('y=', y)
print ('calls=', parametric.getCallsNumber())


# op(Sample)
X = [[1.0], [7.0]]
Y = parametric(X)
print('Y=', Y)
print ('calls=', parametric.getCallsNumber())
