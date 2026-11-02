#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Constructor with size
point1 = ot.PointWithDescription(2, 0.1)
assert point1[0] == 0.1

point1[0] = 100.0
point1[1] = 101.0
assert len(point1.getDescription()) == 2

# Copy constructor
point2 = ot.PointWithDescription(point1)
assert point2[0] == 100.0

# Description
assert len(point2.getDescription()) == len(point1.getDescription())
desc = ["point1_X", "point1_Y"]
point1.setDescription(desc)

assert point1.getDescription() == desc

point3 = ot.PointWithDescription(point1)
assert point1.getDescription() == desc

assert point1 == point3
assert point1 != point2

point4 = ot.PointWithDescription([0.5] * 2, desc)
assert point4.getDescription() == desc
