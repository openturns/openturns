#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ott.setRandomGenerator()

# The 1D interface
dim = 2
a = ot.Point(dim, -1.0)
b = ot.Point(dim, 2.0)
domain = ot.Domain(ot.Interval(a, b))
p1 = ot.Point(dim, 0.5)
p2 = ot.Point(dim, 2.5)
print("Domain=", domain)
# Accessors
print("Dimension=", domain.getDimension())
# Contains
print("is point ", p1, " inside ", domain, "? ", p1 in domain)
print("is point ", p2, " inside ", domain, "? ", p2 in domain)
# Draw
domain = ot.Interval([-2.0, 0.0, -1], [2., 3., 1.])
U = ot.Uniform(-3, 3)
dist = ot.JointDistribution([U, U, U])
grid = domain.draw(dist.getSample(30))
print(grid)
