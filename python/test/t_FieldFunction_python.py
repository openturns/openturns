#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

mesh = ot.RegularGrid(0.0, 0.1, 11)

class FUNC(ot.OpenTURNSPythonFieldFunction):

    def __init__(self):
        # first argument:
        super(FUNC, self).__init__(mesh, 2, mesh, 2)
        self.setInputDescription(['R', 'S'])
        self.setOutputDescription(['T', 'U'])

    def _exec(self, X):
        Y = ot.Field(X.getMesh(), X.getValues()
                     * ([2.0] * X.getValues().getDimension()))
        return Y

F = FUNC()
print('in_dim=' + str(F.getInputDimension())
       + ' out_dim=' + str(F.getOutputDimension())
       + ' spatial_dim=' + str(F.getInputMesh().getDimension()))


X = ot.Field(mesh, ot.Normal(2).getSample(11))
print(F(X))

Xsample = ot.ProcessSample(5, X)
print(F(Xsample))


# Instance creation
myFunc = ot.FieldFunction(F)

# Copy constructor
newFunc = ot.FieldFunction(myFunc)

print(('myFunc input dimension= ' + str(myFunc.getInputDimension())))
print(('myFunc output dimension= ' + str(myFunc.getOutputDimension())))

print(myFunc(X))

print(myFunc(Xsample))

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

def myPyFunc(X):
    mesh = X.getMesh()
    values = X.getValues() * ([2.0] * X.getValues().getDimension())
    values.setDescription(
        ot.Description.BuildDefault(values.getDimension(), "Y"))
    Y = ot.Field(mesh, values)
    return Y

in_dim = 3
out_dim = 3

myFunc = ot.PythonFieldFunction(mesh3D, in_dim, mesh3D, out_dim, myPyFunc)
print('myFunc=', myFunc)

values = ot.Normal(mesh3D.getDimension()).getSample(mesh3D.getVerticesNumber())
# Evaluation over a single field
X = ot.Field(mesh3D, values)
print('X=', X)
Y = myFunc(X)
print('Y=', Y)
print('myFunc input dimension=', myFunc.getInputDimension())
print('myFunc output dimension=', myFunc.getOutputDimension())
print('myFunc spatial dimension=', myFunc.getInputMesh().getDimension())
print('called ', myFunc.getCallsNumber(), ' times')
# Evaluation over a process sample
X = ot.ProcessSample(5, X)
print('X=', X)
Y = myFunc(X)
print('Y=', Y)
print('myFunc input dimension=', myFunc.getInputDimension())
print('myFunc output dimension=', myFunc.getOutputDimension())
print('myFunc spatial dimension=', myFunc.getInputMesh().getDimension())
print('called ', myFunc.getCallsNumber(), ' times')
