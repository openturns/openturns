#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

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
