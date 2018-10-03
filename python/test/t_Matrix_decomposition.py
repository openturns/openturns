#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()


def quadM(m, n):
    res = Matrix(m, n)
    for i in range(m):
        for j in range(n):
            res[i, j] = (i + 1.0) ** (j + 1.0)
    return res


def testQR(m, n, full, keep):
    matrix1 = quadM(m, n)
    print("M=", matrix1)
    Q, R = matrix1.computeQR(full, keep)
    print("full=", full, "keep=", keep)
    print('Q= ', Q)
    print('R=', R)
    print('Q*R=', Q * R)
    if keep:
        print('M2=', matrix1)


try:
    # Square case
    matrix1 = quadM(3, 3)
    matrix1.setName("matrix1")
    print("matrix1 = ", repr(matrix1))

    result1 = matrix1.computeSingularValues()
    print("svd (svd only)= ", repr(result1))

    result1, u, v = matrix1.computeSVD(True)
    print("svd (svd + U, V full)= ", repr(result1))
    result1, u, v = matrix1.computeSVD(False)
    print("svd (svd + U, V small)= ", repr(result1),
          ", U=", repr(u), ", v=", repr(v))

    # Rectangular case, m < n
    matrix1 = quadM(3, 5)
    matrix1.setName("matrix1")
    print("matrix1 = ", repr(matrix1))

    result1 = matrix1.computeSingularValues()
    print("svd (svd only)= ", repr(result1))

    result1, u, v = matrix1.computeSVD(True)
    print("svd (svd + U, V full)= ", repr(result1))
    result1, u, v = matrix1.computeSVD(False)
    print("svd (svd + U, V small)= ", repr(result1),
          ", U=", repr(u), ", v=", repr(v))

    # Rectangular case, m > n
    matrix1 = quadM(5, 3)
    matrix1.setName("matrix1")
    print("matrix1 = ", repr(matrix1))

    result1 = matrix1.computeSingularValues()
    print("svd (svd only)= ", repr(result1))

    result1, u, v = matrix1.computeSVD(True)
    print("svd (svd + U, V full)= ", repr(result1))
    # result1, u, v = matrix1.computeSVD(False)
    # print "svd (svd + U, V small)= ", repr(result1), ", U=", repr(u), ",
    # v=", repr(v)

    for iFull in range(2):
        for iKeep in range(2):
            testQR(3, 3, iFull == 1, iKeep == 1)
            testQR(3, 5, iFull == 1, iKeep == 1)
            testQR(5, 3, iFull == 1, iKeep == 1)
except:
    import sys
    print("t_Matrix_decomposition.py", sys.exc_info()[0], sys.exc_info()[1])
