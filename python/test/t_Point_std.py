#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

# Default constructor */
point1 = ot.Point()
ref_point1 = point1

# Check method add()
point1.add(0.)
point1.add(1.)

size = ref_point1.getDimension()
print("size of point1 = ", size)

val1 = ref_point1[0]
val2 = ref_point1[1]
print("point1 = ", repr(ref_point1))
print("point1[0] = %.6f" % val1)
print("point1[1] = %.6f" % val2)

# Constructor with size
point2 = ot.Point(2)
ref_point2 = point2

# Check operator[] methods
point2[0] = 10.
point2[1] = 11.

val1 = ref_point2[0]
val2 = ref_point2[1]
print("point2[0] = %.6f" % val1)
print("point2[1] = %.6f" % val2)

# Copy constructor
point3 = ot.Point(ref_point1)
ref_point3 = point3

val1 = ref_point3[0]
val2 = ref_point3[1]
print("point3[0] = %.6f" % val1)
print("point3[1] = %.6f" % val2)

# Constructor from indices
indices = ot.Indices(5)
indices.fill()
point = ot.Point(indices)
print("point=", point)

# Constructor from python sequence
point4 = ot.Point((1000., 1001.))
i = 0
for coord in point4:
    print("point4[%d] = %.6f" % (i, coord))
    i += 1

# Comparison operator
if ref_point2 != ref_point2:
    print(
        "OT::Base::Type::Point.operator == does NOT return the correct value. Says that point2 and point2 are DIFFERENT though they are equal.")
    raise RuntimeError()

if ref_point2 == ref_point3:
    print(
        "OT::Base::Type::Point.operator == does NOT return the correct value. Says that point2 and point3 are EQUAL though they are different.")
    raise RuntimeError()

# Addition/Subtraction/Product operators
point5 = ref_point1 + ref_point2
ref_point5 = point5
print("point5 = ", repr(ref_point5))

point6 = ref_point1 - ref_point2
ref_point6 = point6
print("point6 = ", repr(ref_point6))

point7 = ref_point5
point7 += ref_point1 + ref_point2
ref_point7 = point7
print("point7 = ", repr(ref_point7))

point8 = ref_point6
point8 -= ref_point1 - ref_point2
ref_point8 = point8
print("point8 = ", repr(ref_point8))

point9 = ot.Point(2, 5.0) / 2.0
print("point9 = ", point9)

point10 = ot.Point(2, 5.0)
point10 /= 2.0
print("point10 = ", point10)

# We create an orthonormal base (O,i,j)
i = ot.Point(2)
i[0] = 1
i[1] = 0
print("i = ", repr(i))

j = ot.Point(2)
j[0] = 0
j[1] = 1
print("j = ", repr(j))

# BUG PYTHON for I = Point(2. * i)
I = ot.Point(i * 2.)
print("I = ", repr(I))

J = ot.Point(j * 3.)
print("J = ", repr(J))

dotProduct = I.dot(J)
print("dotProduct = %.6f" % dotProduct)

# slicing
point8 = ot.Point((1., 2., 3., 4., 5.))
print('point8[2] = ', point8[2])
print('point8[1:3] = ', point8[1:3])
print('point8[:] = ', point8[:])
point8[1:3] = (88, 99)
print('point8 = ', point8)
point8[0:5] = ot.Point(5, 9)
print('point8 = ', point8)

# equality
pt1 = ot.Point([1.1, 2.2, 3.3])
pt2 = ot.Point((1.1, 2.2, 3.3))
print("Equality ? ", (pt1 == pt2))

# non-regression for #754
pt = ot.Point([-3, -5, 6])
print('pt = ', pt)

# last
print('pt[-1]=', pt[-1])
pt[-1] = 8.
print('pt[-1]=', pt[-1])

print(0 + pt1)
try:
    1 + pt1
except:
    print('1+Point -> ok')

# unary minus
x = ot.Point([1, 2, 3])
print(-x)

try:
    ot.Point(None)
except Exception:
    print('Point(None) -> ok')
