#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # First : an n by n case
    matrix1 = Matrix(2, 2)
    matrix1[0, 0] = 1.0
    matrix1[1, 0] = 2.0
    matrix1[0, 1] = 5.0
    matrix1[1, 1] = 12.0
    print("matrix1=" + repr(matrix1))

    pt1 = Point()
    pt1.add(5.0)
    pt1.add(0.0)
    print("pt1=" + repr(pt1))

    result1 = Point()
    result1 = matrix1.solveLinearSystem(pt1)
    print("result1=" + repr(result1))

    # Second : an n by p case, n < p
    matrix2 = Matrix(2, 3)
    matrix2[0, 0] = 1.0
    matrix2[1, 0] = 2.0
    matrix2[0, 1] = 5.0
    matrix2[1, 1] = 12.0
    matrix2[0, 2] = 3.0
    matrix2[1, 2] = 4.0
    print("matrix2=" + repr(matrix2))

    pt2 = Point()
    pt2.add(5.0)
    pt2.add(0.0)
    print("pt2=" + repr(pt2))

    result2 = Point()
    result2 = matrix2.solveLinearSystem(pt2)
    print("result2=" + repr(result2))

    # Third : an n by p case, n > p
    matrix3 = Matrix(3, 2)
    matrix3[0, 0] = 1.0
    matrix3[1, 0] = 2.0
    matrix3[2, 0] = 4.0
    matrix3[0, 1] = 5.0
    matrix3[1, 1] = 12.0
    matrix3[2, 1] = 3.0

    print("matrix3=" + repr(matrix3))

    pt3 = Point()
    pt3.add(5.0)
    pt3.add(0.0)
    pt3.add(1.0)
    print("pt3=" + repr(pt3))

    result3 = Point()
    result3 = matrix3.solveLinearSystem(pt3, True)
    print("result3=" + repr(result3))

    b1 = Matrix(2, 4)
    b1[0, 0] = 5.0
    b1[1, 0] = 1.0
    b1[0, 1] = 10.0
    b1[1, 1] = 2.0
    b1[0, 2] = 15.0
    b1[1, 2] = 3.0
    b1[0, 3] = 20.0
    b1[1, 3] = 4.0
    print("b1=" + repr(b1))
    result4 = matrix1.solveLinearSystem(b1, True)
    print("result4=" + repr(result4))
    result4 = matrix1.solveLinearSystem(b1, False)
    print("result4=" + repr(result4))

    result5 = matrix2.solveLinearSystem(b1, True)
    print("result5=" + repr(result5))
    result5 = matrix2.solveLinearSystem(b1, False)
    print("result5=" + repr(result5))

    b2 = Matrix(3, 4)
    b2[0, 0] = 5.0
    b2[1, 0] = 1.0
    b2[2, 0] = -2.0
    b2[0, 1] = 10.0
    b2[1, 1] = 2.0
    b2[2, 1] = -4.0
    b2[0, 2] = 15.0
    b2[1, 2] = 3.0
    b2[2, 2] = -6.0
    b2[0, 3] = 20.0
    b2[1, 3] = 4.0
    b2[2, 3] = -8.0
    print("b2=" + repr(b2))
    result6 = matrix3.solveLinearSystem(b2, True)
    print("result6=" + repr(result6))
    result6 = matrix3.solveLinearSystem(b2, False)
    print("result6 = " + repr(result6))

except:
    import sys
    print("t_MatrixSolveLinearSystem_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
