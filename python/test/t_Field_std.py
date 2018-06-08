#! /usr/bin/env python

from __future__ import print_function
import os
import openturns as ot

ot.PlatformInfo.SetNumericalPrecision(6)

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

s = 3
values = ot.Normal(s).getSample(mesh3D.getVerticesNumber())
field = ot.Field(mesh3D, values)
tree = ot.KDTree(vertices)
print('field=', field)
print('input dim=', field.getInputDimension())
print('value[4]=', field.getValueAtIndex(4))
print('value[4, 0]=%.6g' % field[4, 0])
print('nearest[2]=', field.getValueAtIndex(tree.query(field[2])))
print('mesh=', field.getMesh())
print('input mean=', field.getInputMean())
print('deformed=', field.asDeformedMesh())
print('description=', field.getDescription())

fname = 'field.vtk'
field.exportToVTKFile(fname)
with open(fname) as f:
    data = f.read()
    print(data)
os.remove(fname)

scalarField = ot.Field(mesh3D, ot.Sample(len(vertices), [1.0]))
print('deformed with complement=', scalarField.asDeformedMesh([1], [0, 2, 3]))
