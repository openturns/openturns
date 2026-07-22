#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# pStudentOrthantND (Ridgway SMC algorithm)
print("MVT orthant d=1 exact")
L = ot.TriangularMatrix(1)
L[0, 0] = 1.0
p = ot.DistFunc.pStudentOrthantND([-1.0], [1.0], L, 5.0, 100)
expected = ot.DistFunc.pStudent(5.0, 1.0) - ot.DistFunc.pStudent(5.0, -1.0)
ott.assert_almost_equal(p, expected)

print("MVT orthant d=2 independent")
L = ot.TriangularMatrix(2)
L[0, 0] = 1.0
L[1, 0] = 0.0
L[1, 1] = 1.0
p = ot.DistFunc.pStudentOrthantND([-1.0, -1.0], [1.0, 1.0], L, 5.0, 100, 0.5, 2000)
expected = 0.428287
ott.assert_almost_equal(p, expected, 5e-2)

print("MVT orthant d=2 correlated")
sigma = ot.CovarianceMatrix(2)
sigma[0, 0] = 1.0
sigma[1, 0] = 0.5
sigma[1, 1] = 2.0
p = ot.DistFunc.pStudentOrthantND([-1.0, -2.0], [1.0, 2.0], sigma, 10.0, 100, 0.5, 500)
ott.assert_almost_equal(p, 0.5, 2e-1)

print("MVT orthant d=2 with mu")
sigma = ot.CovarianceMatrix(2)
sigma[0, 0] = 1.0
sigma[1, 0] = 0.3
sigma[1, 1] = 1.0
a = [-1.0, -1.0]
b = [1.0, 1.0]
p = ot.DistFunc.pStudentOrthantND(a, b, [0.5, -0.5], sigma, 10.0, 100, 0.5, 500)
ott.assert_almost_equal(p, 0.3808, 2e-1)

print("Error: dimension mismatch")
L = ot.TriangularMatrix(2)
L[0, 0] = 1.0
L[1, 0] = 0.0
L[1, 1] = 1.0
with ott.assert_raises(RuntimeError):
    ot.DistFunc.pStudentOrthantND([-1.0], [1.0], L, 5.0)

print("Error: a > b")
L = ot.TriangularMatrix(1)
L[0, 0] = 1.0
with ott.assert_raises(TypeError):
    ot.DistFunc.pStudentOrthantND([1.0], [-1.0], L, 5.0)

print("Error: M = 0")
L = ot.TriangularMatrix(1)
L[0, 0] = 1.0
with ott.assert_raises(TypeError):
    ot.DistFunc.pStudentOrthantND([-1.0], [1.0], L, 5.0, 0)

print("Error: L(i,i) <= 0")
L = ot.TriangularMatrix(1)
L[0, 0] = 0.0
with ott.assert_raises(TypeError):
    ot.DistFunc.pStudentOrthantND([-1.0], [1.0], L, 5.0)

print("Error: nu <= 0")
L = ot.TriangularMatrix(1)
L[0, 0] = 1.0
with ott.assert_raises(TypeError):
    ot.DistFunc.pStudentOrthantND([-1.0], [1.0], L, 0.0)

print("Edge: a == b")
L = ot.TriangularMatrix(1)
L[0, 0] = 1.0
p = ot.DistFunc.pStudentOrthantND([0.0], [0.0], L, 5.0)
ott.assert_almost_equal(p, 0.0, 0.0, 1e-15)

print("Edge: wide interval")
L = ot.TriangularMatrix(2)
L[0, 0] = 1.0
L[1, 0] = 0.0
L[1, 1] = 1.0
p = ot.DistFunc.pStudentOrthantND([-10.0, -10.0], [10.0, 10.0], L, 5.0)
ott.assert_almost_equal(p, 1.0, 1.0, 1e-10)

print("MVT orthant d=3 independent")
L = ot.TriangularMatrix(3)
L[0, 0] = 1.0
L[1, 0] = 0.0
L[2, 0] = 0.0
L[1, 1] = 1.0
L[2, 1] = 0.0
L[2, 2] = 1.0
p = ot.DistFunc.pStudentOrthantND(
    [-1.0, -1.0, -1.0], [1.0, 1.0, 1.0], L, 5.0, 100, 0.5, 2000
)
expected = 0.300434
ott.assert_almost_equal(p, expected, 1e-1)
