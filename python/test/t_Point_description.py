#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Constructor with size
point1 = ot.PointWithDescription(2)
point1[0] = 100.0
point1[1] = 101.0

# Copy constructor
point2 = ot.PointWithDescription(point1)

# Description
print("point1.getDescription() = ", repr(point1.getDescription()))
print("point2.getDescription() = ", repr(point2.getDescription()))

description = ot.Description(2)
description[0] = "point1_X"
description[1] = "point1_Y"

point1.setDescription(description)

print("point1.getDescription() = ", repr(point1.getDescription()))
print("point2.getDescription() = ", repr(point2.getDescription()))
