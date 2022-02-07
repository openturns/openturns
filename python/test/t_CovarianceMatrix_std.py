#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

matrix1 = ot.CovarianceMatrix(2)
print("matrix1 (default)=" + repr(matrix1))
matrix1[0, 0] = 1.0
matrix1[1, 0] = 0.5
matrix1[1, 1] = 1.0
print("matrix1 (initialized)=" + repr(matrix1))

pt = ot.Point()
pt.add(5.0)
pt.add(0.0)
print("pt=", repr(pt))

result = ot.Point()
result = matrix1.solveLinearSystem(pt)
print("result=" + repr(result))

determinant = matrix1.computeDeterminant()
print("determinant=%.6f" % determinant)

ev = ot.ScalarCollection(2)
ev = matrix1.computeEigenValues()
print("ev=" + repr(ev))

if matrix1.isPositiveDefinite():
    isSPD = "true"
else:
    isSPD = "false"
print("isSPD=", isSPD)

matrix2 = matrix1.computeCholesky()
print("matrix2=" + repr(matrix2))

b = ot.Matrix(2, 3)
b[0, 0] = 5.0
b[1, 0] = 0.0
b[0, 1] = 10.0
b[1, 1] = 1.0
b[0, 2] = 15.0
b[1, 2] = 2.0
result2 = matrix1.solveLinearSystem(b, True)
print("result2=" + repr(result2))

matrix3 = ot.CovarianceMatrix(3)
matrix3[1, 0] = float('nan')
try:
    print("ev=", matrix3.computeSingularValues())
except:
    print("ok")

# from SymmetricMatrix
sym = ot.SymmetricMatrix(3)
sym[0, 0] = 1.0e-02
sym[1, 1] = 1.0e-02
sym[2, 2] = 1.0e-02
sym[0, 1] = 7.0e-04
ot.CovarianceMatrix(sym)
print("ok")
