#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# We create a numerical point of dimension 4
point = ot.Point([101.0, 102.0, 103.0, 104.0])

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
