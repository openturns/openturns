#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

matrix1 = ot.IdentityMatrix(2)
print("matrix1 = ", repr(matrix1))

pt = ot.Point()
pt.add(5.0)
pt.add(0.0)
print("pt = ", repr(pt))

result = ot.Point()
result = matrix1.solveLinearSystem(pt)
print("result = ", repr(result))

determinant = matrix1.computeDeterminant()
print("determinant = %.6f" % determinant)

ev = ot.ScalarCollection(2)
ev = matrix1.computeEigenValues()
print("ev = ", repr(ev))

if matrix1.isPositiveDefinite():
    isSPD = "true"
else:
    isSPD = "false"
print("isSPD = ", isSPD)

matrix2 = matrix1.computeCholesky()
print("matrix2 = ", repr(matrix2))

det, sgn = matrix1.computeLogAbsoluteDeterminant()
print(det)
assert det == 0.0, "wrong det"

sv = matrix1.computeSingularValues()
print(sv)
assert sv == [1.0] * 2, "wrong sv"

ev, u, vT = matrix1.computeSVD()
print(ev, u, vT)
assert ev == [1.0] * 2, "wrong svd"

ev, v = matrix1.computeEV()
print(ev, v)
assert ev == [1.0] * 2, "wrong ev"

tr = matrix1.transpose()
print(tr)
assert tr == matrix1, "transpose"

cov = ot.CovarianceMatrix(2)
prod = matrix1 @ cov
print(prod)
assert prod == matrix1, "product /cov"
