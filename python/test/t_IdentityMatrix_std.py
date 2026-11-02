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

# extra coverage (no prints: output is checked against .expout)
default = ot.IdentityMatrix()
assert default.getDimension() == 0
assert repr(default).startswith("class=IdentityMatrix")

# read-only element access
assert matrix1[0, 0] == 1.0
assert matrix1[0, 1] == 0.0
assert matrix1[1, 0] == 0.0

# products with the other matrix types are identities
mat = ot.Matrix([[1.0, 2.0], [3.0, 4.0]])
assert matrix1 * mat == mat
sq = ot.SquareMatrix([[1.0, 2.0], [3.0, 4.0]])
assert matrix1 * sq == sq
sym = ot.SymmetricMatrix([[1.0, 2.0], [2.0, 4.0]])
assert matrix1 * sym == sym
corr = ot.CorrelationMatrix([[1.0, 0.5], [0.5, 1.0]])
assert matrix1 * corr == corr

# in-place variants
matB = ot.Matrix([[5.0], [6.0]])
assert matrix1.solveLinearSystemInPlace(matB) == matB
sign = 0.0
det_inplace, sign_inplace = matrix1.computeLogAbsoluteDeterminantInPlace()
assert det_inplace == 0.0
assert sign_inplace == 1.0
assert matrix1.computeDeterminantInPlace() == 1.0
assert list(matrix1.computeEigenValuesInPlace()) == [1.0, 1.0]
ev_inplace, v_inplace = matrix1.computeEVInPlace()
assert list(ev_inplace) == [1.0, 1.0]
assert v_inplace == matrix1
assert list(matrix1.computeSingularValuesInPlace()) == [1.0, 1.0]
svd_ev, svd_u, svd_vt = matrix1.computeSVDInPlace()
assert list(svd_ev) == [1.0, 1.0]
assert svd_u == matrix1
assert svd_vt == matrix1
assert matrix1.computeCholeskyInPlace() == matrix2
