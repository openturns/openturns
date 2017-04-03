#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    # We create a numerical point of dimension 4
    point = Point(4)
    ref_point = point

    point[0] = 101.
    point[1] = 102.
    point[2] = 103.
    point[3] = 104.

    print("point = ", repr(ref_point))

    # We create a 'constant' RandomVector from the Point
    vect = RandomVector(ConstantRandomVector(point))
    print("vect=", vect)

    # Check standard methods of class RandomVector
    print("vect dimension=", vect.getDimension())
    print("vect realization (first )=", repr(vect.getRealization()))
    print("vect realization (second)=", repr(vect.getRealization()))
    print("vect realization (third )=", repr(vect.getRealization()))
    print("vect sample =", repr(vect.getSample(5)))

# except TestFailed, ex :
except:
    import sys
    print("t_RandomVector_constant.py", sys.exc_info()[0], sys.exc_info()[1])
