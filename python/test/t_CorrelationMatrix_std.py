#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

matrix1 = ot.CorrelationMatrix(2)
matrix1.setName("matrix1")
print("matrix1 = ", repr(matrix1))

p = ot.Point((1., 3.))
print("matrix1*", p, "=", matrix1 * p)

s = 3.
print("matrix1*", s, "=", matrix1 * s)
print(s, "*matrix1=", s * matrix1)
print("matrix1/", s, "=", matrix1 / s)

# from SymmetricMatrix
sym = ot.SymmetricMatrix(3)
sym[0, 0] = 1.0e-02
sym[1, 1] = 1.0e-02
sym[2, 2] = 1.0e-02
sym[0, 1] = 7.0e-04
ot.CorrelationMatrix(sym)
print("ok")
