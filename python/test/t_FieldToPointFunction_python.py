#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

class FUNC(ot.OpenTURNSPythonFieldToPointFunction):

    def __init__(self):
        # first argument:
        super(FUNC, self).__init__(2, 2, 1)
        self.setInputDescription(['R', 'S'])
        self.setOutputDescription(['T', 'U'])
        
    def _exec(self, X):
        Y = X.getValues().computeMean()
        return Y

F = FUNC()
print(('in_dim=' + str(F.getInputDimension())
       + ' out_dim=' + str(F.getOutputDimension())
       + ' spatial_dim=' + str(F.getSpatialDimension())))

X = ot.Field(ot.RegularGrid(0.0, 0.1, 11), ot.Normal(2).getSample(11))
print(F(X))

Xsample = ot.ProcessSample(5, X)
print(F(Xsample))

# Instance creation
myFunc = ot.FieldToPointFunction(F)

# Copy constructor
newFunc = ot.FieldToPointFunction(myFunc)

print(('myFunc input dimension= ' + str(myFunc.getInputDimension())))
print(('myFunc output dimension= ' + str(myFunc.getOutputDimension())))

print(myFunc(X))

print(myFunc(Xsample))

def myPyFunc(X):
    Y = X.getValues().computeMean()
    return Y

in_dim = 3
out_dim = 3
spatial_dim = 3

myFunc = ot.PythonFieldToPointFunction(in_dim, out_dim, spatial_dim, myPyFunc)
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
# Evaluation over a single field
X = ot.Field(mesh3D, values)
print('X=', X)
Y = myFunc(X)
print('Y=', Y)
print('myFunc input dimension=', myFunc.getInputDimension())
print('myFunc output dimension=', myFunc.getOutputDimension())
print('myFunc spatial dimension=', myFunc.getSpatialDimension())
print('called ', myFunc.getCallsNumber(), ' times')
# Evaluation over a process sample
X = ot.ProcessSample(5, X)
print('X=', X)
Y = myFunc(X)
print('Y=', Y)
print('myFunc input dimension=', myFunc.getInputDimension())
print('myFunc output dimension=', myFunc.getOutputDimension())
print('myFunc spatial dimension=', myFunc.getSpatialDimension())
print('called ', myFunc.getCallsNumber(), ' times')
