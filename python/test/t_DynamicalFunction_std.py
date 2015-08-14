#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

# Create an intance
myFunc = ot.DynamicalFunction()

print('myFunc=', myFunc)
# Get the input and output description
print('myFunc input description=', myFunc.getInputDescription())
print('myFunc output description=', myFunc.getOutputDescription())
# Get the input and output dimension, based on description
print('myFunc input dimension=', myFunc.getInputDimension())
print('myFunc output dimension=', myFunc.getOutputDimension())
print('myFunc spatial dimension=', myFunc.getSpatialDimension())
# Get the number of calls
print('called ', myFunc.getCallsNumber(), ' times')


def myPyFunc(X):
    mesh = X.getMesh()
    Y = ot.Field(mesh, ot.Normal().getSample(mesh.getVerticesNumber()))
    return Y

in_dim = 3
out_dim = 1
spatial_dim = 3

myFunc = ot.PythonDynamicalFunction(in_dim, out_dim, spatial_dim, myPyFunc)
print('myFunc=', myFunc)

vertices = []
vertices.append([0.0, 0.0, 0.0])
vertices.append([0.0, 0.0, 1.0])
vertices.append([0.0, 1.0, 0.0])
vertices.append([0.0, 1.0, 1.0])
vertices.append([1.0, 0.0, 0.0])
vertices.append([1.0, 0.0, 1.0])
vertices.append([1.0, 1.0, 0.0])
vertices.append([1.0, 1.0, 1.0])
simplicies = []
simplicies.append([0, 1, 2, 4])
simplicies.append([3, 5, 6, 7])
simplicies.append([1, 2, 3, 6])
simplicies.append([1, 2, 4, 6])
simplicies.append([1, 3, 5, 6])
simplicies.append([1, 4, 5, 6])
mesh3D = ot.Mesh(vertices, simplicies)
values = ot.Normal(spatial_dim).getSample(mesh3D.getVerticesNumber())
X = ot.Field(mesh3D, values)
print('X=', X)
Y = myFunc(X)
print('Y=', Y)
print('myFunc input dimension=', myFunc.getInputDimension())
print('myFunc output dimension=', myFunc.getOutputDimension())
print('myFunc spatial dimension=', myFunc.getSpatialDimension())
print('called ', myFunc.getCallsNumber(), ' times')
