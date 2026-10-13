#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

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

# Constructor from values and description, see issue #3094
point3 = ot.PointWithDescription(ot.Point([1.0, 2.0]), ot.Description(["a", "b"]))
assert list(point3.getDescription()) == ["a", "b"]
assert list(point3) == [1.0, 2.0]
# plain Python sequences are accepted as well
point4 = ot.PointWithDescription([1.0, 2.0], ["a", "b"])
assert point4 == point3
# mismatched sizes raise
with ott.assert_raises(TypeError):
    ot.PointWithDescription([1.0, 2.0], ["only_one"])
# setDescription accepts a plain Python list
point1.setDescription(["x", "y"])
assert list(point1.getDescription()) == ["x", "y"]
