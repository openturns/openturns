#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

matrix1 = IdentityMatrix(2)
print("matrix1 = ", repr(matrix1))

pt = Point()
pt.add(5.)
pt.add(0.)
print("pt = ", repr(pt))

result = Point()
result = matrix1.solveLinearSystem(pt)
print("result = ", repr(result))

determinant = matrix1.computeDeterminant()
print("determinant = %.6f" % determinant)

ev = ScalarCollection(2)
ev = matrix1.computeEigenValues()
print("ev = ", repr(ev))

if matrix1.isPositiveDefinite():
    isSPD = "true"
else:
    isSPD = "false"
print("isSPD = ", isSPD)

matrix2 = matrix1.computeCholesky()
print("matrix2 = ", repr(matrix2))
