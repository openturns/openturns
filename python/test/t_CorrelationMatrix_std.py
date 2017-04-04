#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    matrix1 = CorrelationMatrix(2)
    matrix1.setName("matrix1")
    print("matrix1 = ", repr(matrix1))

    p = Point((1., 3.))
    print("matrix1*", p, "=", matrix1 * p)

    s = 3.
    print("matrix1*", s, "=", matrix1 * s)
    print(s, "*matrix1=", s * matrix1)
    print("matrix1/", s, "=", matrix1 / s)

    Vars = ['X', 'Y', 'Z']
    Map = {}
    Map['X'] = {}
    Map['X']['Y'] = 0.1
    Map['X']['Z'] = 0.7
    Map['Y'] = {}
    Map['Y']['Z'] = 0.8
    matrix2 = getCorrelationMatrixFromMap(Vars, Map)
    print(matrix2)

except:
    import traceback
    traceback.print_exc()
