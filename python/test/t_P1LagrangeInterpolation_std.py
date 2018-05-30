#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

mesh = ot.RegularGrid(0.0, 1.0, 4)
values = [(x, 2.0 * x, x * x) for x in mesh.getValues()]
outPoint = [2.5]
interpolation = ot.P1LagrangeInterpolation(
    mesh, ot.Mesh([outPoint]), len(values[0]))
print("Interpolation=", interpolation)
print("Values at", outPoint, "=", interpolation(values))
