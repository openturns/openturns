#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    matrix1 = SymmetricMatrix(2)
    matrix1.setName("matrix1")
    matrix1[0, 0] = 1.0
    matrix1[1, 0] = 5.0
    matrix1[1, 1] = 12.0
    print("matrix1=", matrix1)
    print("matrix1=")
    print(matrix1.__str__())

    pt = Point(0)
    pt.add(5.0)
    pt.add(0.0)
    print("pt=", pt)

    result = matrix1.solveLinearSystem(pt)
    print("result=", result)
    #    print "verif. ", matrix1 * result - pt

    determinant = matrix1.computeDeterminant()
    print("determinant= %.1f" % determinant)

    b = Matrix(2, 3)
    b[0, 0] = 5.0
    b[1, 0] = 0.0
    b[0, 1] = 10.0
    b[1, 1] = 1.0
    b[0, 2] = 15.0
    b[1, 2] = 2.0
    result2 = Matrix()
    result2 = matrix1.solveLinearSystem(b)
    print("result2=", result2)
    print("result2=")
    print(result2.__str__())

    ev = matrix1.computeEigenValues()
    print("ev=", ev)

    ev, evect = matrix1.computeEV()
    print("ev=", ev)
    print("evect=", repr(evect))
    print("evect=")
    print(evect.__str__())

    # Check the high dimension determinant computation
    matrix3 = SymmetricMatrix(3)
    matrix3[0, 0] = 1.0
    matrix3[0, 1] = 2.0
    matrix3[0, 2] = 3.0
    matrix3[1, 1] = 2.5
    matrix3[1, 2] = -3.5
    matrix3[2, 2] = 2.5

    print("matrix3=")
    print(matrix3.__str__())
    # sign = 0.0
    # value = matrix3.computeLogAbsoluteDeterminant(sign)
    # print "log(|det|)=", value, ", sign=", sign
    value = matrix3.computeDeterminant()
    print("det=", value)

except:
    import sys
    print("t_SymmetricMatrixLapack_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
