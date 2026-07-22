#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# pNormal
xMin = 0.1
xMax = 0.9
nX = 10
grid = [0.0] * nX
for iX in range(nX):
    x = xMin + (xMax - xMin) * iX / (nX - 1)
    grid[iX] = x
    print(
        f"pNormal({x:.12g})={ot.DistFunc.pNormal(x):.6g}, complementary={ot.DistFunc.pNormal(x, True):.6g}"
    )
print("pNormal(", grid, "=", ot.DistFunc.pNormal(grid))
# qNormal
qMin = 0.0
qMax = 1.0
nQ = 11
grid = [0.0] * nQ
for iQ in range(nQ):
    q = qMin + (qMax - qMin) * iQ / (nQ - 1)
    grid[iQ] = q
    print(
        f"qNormal({q:.12g})={ot.DistFunc.qNormal(q):.6g}, complementary={ot.DistFunc.qNormal(q, True):.6g}"
    )
print("qNormal(", grid, ")=", ot.DistFunc.qNormal(grid))
# rNormal
nR = 10
for iR in range(nR):
    print(f"rNormal()={ot.DistFunc.rNormal():.6g}")
# pNormal2D
print(
    f"pNormal2D(1.0, 1.0, 0.9)={ot.DistFunc.pNormal2D(1.0, 1.0, 0.9):.6g}, complementary={ot.DistFunc.pNormal2D(1.0, 1.0, 0.9, True):.6g}"
)
print(
    f"pNormal2D(-1.0, -1.0, 1.0, 1.0, 0.9)={ot.DistFunc.pNormal2D(-1.0, -1.0, 1.0, 1.0, 0.9):.6g}"
)
# pNormal3D rectangular
print(
    f"pNormal3D(-1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 0.5, 0.2, 0.0)={ot.DistFunc.pNormal3D(-1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 0.5, 0.2, 0.0):.6g}"
)

# pNormalND (Genz algorithm)
print("MVN d=1 exact")
L = ot.TriangularMatrix(1)
L[0, 0] = 1.0
p = ot.DistFunc.pNormalND([-1.0], [1.0], L, 100)
expected = ot.DistFunc.pNormal(1.0) - ot.DistFunc.pNormal(-1.0)
ott.assert_almost_equal(p, expected)

print("MVN d=2 independent")
L = ot.TriangularMatrix(2)
L[0, 0] = 1.0
L[1, 0] = 0.0
L[1, 1] = 1.0
p = ot.DistFunc.pNormalND([-1.0, -1.0], [1.0, 1.0], L, 10000)
p1 = ot.DistFunc.pNormal(1.0) - ot.DistFunc.pNormal(-1.0)
expected = p1 * p1
ott.assert_almost_equal(p, expected, 1e-2)

print("MVN d=2 correlated")
sigma = ot.CovarianceMatrix(2)
sigma[0, 0] = 1.0
sigma[1, 0] = 0.5
sigma[1, 1] = 2.0
p = ot.DistFunc.pNormalND([-1.0, -2.0], [1.0, 2.0], sigma, 10000)
ott.assert_almost_equal(p, 0.5, 2e-1)

print("MVN d=2 with mu")
sigma = ot.CovarianceMatrix(2)
sigma[0, 0] = 1.0
sigma[1, 0] = 0.3
sigma[1, 1] = 1.0
p = ot.DistFunc.pNormalND([-1.0, -1.0], [1.0, 1.0], [0.5, -0.5], sigma, 10000)
ott.assert_almost_equal(p, 0.3808, 2e-1)

print("Error: dimension mismatch")
L = ot.TriangularMatrix(2)
L[0, 0] = 1.0
L[1, 0] = 0.0
L[1, 1] = 1.0
with ott.assert_raises(RuntimeError):
    ot.DistFunc.pNormalND([-1.0], [1.0], L)

print("Error: a > b")
L = ot.TriangularMatrix(1)
L[0, 0] = 1.0
with ott.assert_raises(TypeError):
    ot.DistFunc.pNormalND([1.0], [-1.0], L)

print("Error: n = 0")
L = ot.TriangularMatrix(1)
L[0, 0] = 1.0
with ott.assert_raises(TypeError):
    ot.DistFunc.pNormalND([-1.0], [1.0], L, 0)

print("Error: L(i,i) <= 0")
L = ot.TriangularMatrix(1)
L[0, 0] = 0.0
with ott.assert_raises(TypeError):
    ot.DistFunc.pNormalND([-1.0], [1.0], L)

print("Edge: a == b")
L = ot.TriangularMatrix(1)
L[0, 0] = 1.0
p = ot.DistFunc.pNormalND([0.0], [0.0], L)
ott.assert_almost_equal(p, 0.0, 0.0, 1e-15)

print("Edge: wide interval")
L = ot.TriangularMatrix(2)
L[0, 0] = 1.0
L[1, 0] = 0.0
L[1, 1] = 1.0
p = ot.DistFunc.pNormalND([-10.0, -10.0], [10.0, 10.0], L)
ott.assert_almost_equal(p, 1.0, 1.0, 1e-10)

print("MVN d=3 independent")
L = ot.TriangularMatrix(3)
L[0, 0] = 1.0
L[1, 0] = 0.0
L[2, 0] = 0.0
L[1, 1] = 1.0
L[2, 1] = 0.0
L[2, 2] = 1.0
p = ot.DistFunc.pNormalND([-1.0, -1.0, -1.0], [1.0, 1.0, 1.0], L, 10000)
p1 = ot.DistFunc.pNormal(1.0) - ot.DistFunc.pNormal(-1.0)
expected = p1 * p1 * p1
ott.assert_almost_equal(p, expected, 1e-2)
