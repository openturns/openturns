#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # Constructor with size
    point1 = PointWithDescription(2)
    point1[0] = 100.
    point1[1] = 101.

    # Copy constructor
    point2 = PointWithDescription(point1)

    # Description
    print("point1.getDescription() = ", repr(point1.getDescription()))
    print("point2.getDescription() = ", repr(point2.getDescription()))

    description = Description(2)
    description[0] = "point1_X"
    description[1] = "point1_Y"

    point1.setDescription(description)

    print("point1.getDescription() = ", repr(point1.getDescription()))
    print("point2.getDescription() = ", repr(point2.getDescription()))

except:
    import sys
    print("t_Point_description.py",
          sys.exc_info()[0], sys.exc_info()[1])
