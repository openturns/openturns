#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import math as m


ot.PlatformInfo.SetNumericalPrecision(6)
# 1D example
mesh1D = ot.Mesh()
print("Default 1D mesh=", mesh1D)
vertices = ot.Sample(0, 1)
vertices.add([0.5])
vertices.add([1.5])
vertices.add([2.1])
vertices.add([2.7])
simplicies = [[]] * 3
simplicies[0] = [0, 1]
simplicies[1] = [1, 2]
simplicies[2] = [2, 3]
mesh1D = ot.Mesh(vertices, simplicies)
mesh1Ddomain = ot.MeshDomain(mesh1D)
tree = ot.KDTree(vertices)
enclosingSimplex = ot.EnclosingSimplexAlgorithm(vertices, simplicies)
print("1D mesh=", mesh1D)
print("Is empty? ", mesh1D.isEmpty())
print("vertices=", mesh1D.getVertices())
print("simplices=", mesh1D.getSimplices())
print("volume=", "%.3f" % mesh1D.getVolume())
print("simplices volume=", mesh1D.computeSimplicesVolume())
p = [1.3]
print("is p=", p, " in mesh? ", p in mesh1Ddomain)
point = [1.8]
print("Nearest index(", point, ")=", tree.query(point))
simplex = enclosingSimplex.query(point)
found, coordinates = mesh1D.checkPointInSimplexWithCoordinates(point, simplex)
print("Nearest index(", point, "), found=", found,
      "simplex=", int(simplex), "coordinates=", coordinates)
point = [-1.8]
simplex = enclosingSimplex.query(point)
found, coordinates = mesh1D.checkPointInSimplexWithCoordinates(point, simplex)
print("Nearest index(", point, "), found=", found,
      "simplex=", int(simplex), "coordinates=", coordinates)
points = [[-0.25], [2.25]]
print("Nearest index(", points, ")=", tree.query(points))
print("P1 gram=\n", mesh1D.computeP1Gram())
# 2D case
vertices = [[0.0, 0.0], [1.0, 0.0], [1.0, 1.0],
            [1.5, 1.0], [2.0, 1.5], [0.5, 1.5]]
simplicies = [[0, 1, 2], [1, 2, 3], [2, 3, 4], [2, 4, 5], [0, 2, 5]]

mesh2D = ot.Mesh(vertices, simplicies)
tree = ot.KDTree(vertices)
print("2D mesh=", mesh2D)
print("volume=", "%.3f" % mesh2D.getVolume())
print("simplices volume=", mesh2D.computeSimplicesVolume())
point = [1.8] * 2
print("Nearest index(", point, ")=", tree.query(point))
points = [[-0.25] * 2, [2.25] * 2]
print("Nearest index(", points, ")=", tree.query(points))
print("P1 gram=\n", mesh2D.computeP1Gram())
# 3D case

vertices = ot.Sample(0, 3)

vertices.add([0.0, 0.0, 0.0])
vertices.add([0.0, 0.0, 1.0])
vertices.add([0.0, 1.0, 0.0])
vertices.add([0.0, 1.0, 1.0])
vertices.add([1.0, 0.0, 0.0])
vertices.add([1.0, 0.0, 1.0])
vertices.add([1.0, 1.0, 0.0])
vertices.add([1.0, 1.0, 1.0])

simplicies = [[]] * 6
simplicies[0] = [0, 1, 2, 4]
simplicies[1] = [3, 5, 6, 7]
simplicies[2] = [1, 2, 3, 6]
simplicies[3] = [1, 2, 4, 6]
simplicies[4] = [1, 3, 5, 6]
simplicies[5] = [1, 4, 5, 6]

mesh3D = ot.Mesh(vertices, simplicies)
tree = ot.KDTree(vertices)
print("3D mesh=", mesh3D)
print("volume=", "%.3f" % mesh3D.getVolume())
print("simplices volume=", mesh3D.computeSimplicesVolume())
point = [1.8] * 3
print("Nearest index(", point, ")=", tree.query(point))
points = [[-0.25] * 3, [2.25] * 3]
print("Nearest index(", points, ")=", tree.query(points))
print("P1 gram=\n", mesh3D.computeP1Gram())
rotation = ot.SquareMatrix(3)
rotation[0, 0] = m.cos(m.pi / 3.0)
rotation[0, 1] = m.sin(m.pi / 3.0)
rotation[1, 0] = -m.sin(m.pi / 3.0)
rotation[1, 1] = m.cos(m.pi / 3.0)
rotation[2, 2] = 1.0

# isregular bug
time_grid = ot.RegularGrid(0.0, 0.2, 40963)
mesh = ot.Mesh(time_grid)
print(mesh.isRegular())
