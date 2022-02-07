#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

# We create a numerical point of dimension 4
point = ot.Point([101., 102., 103., 104.])

print("point = ", repr(point))

# We create a 'constant' RandomVector from the Point
vect = ot.RandomVector(ot.ConstantRandomVector(point))
print("vect=", vect)

# Check standard methods of class RandomVector
print("vect dimension=", vect.getDimension())
print("vect realization (first )=", repr(vect.getRealization()))
print("vect realization (second)=", repr(vect.getRealization()))
print("vect realization (third )=", repr(vect.getRealization()))
print("vect sample =", repr(vect.getSample(5)))

