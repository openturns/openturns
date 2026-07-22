#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()

# pStudent
nuMin = 0.2
nuMax = 5.0
n1 = 5
xMin = 0.1
xMax = 0.9
nX = 10
grid = [0.0] * nX
for i1 in range(n1):
    nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1)
    for iX in range(nX):
        x = xMin + (xMax - xMin) * iX / (nX - 1)
        grid[iX] = x
        print(
            "pStudent(",
            nu,
            ",  %.12g" % x,
            ")=%.6g" % ot.DistFunc.pStudent(nu, x),
            ", complementary=%.6g" % ot.DistFunc.pStudent(nu, x, True),
        )
    print("pStudent(", grid, ")=", ot.DistFunc.pStudent(nu, grid))

nu = 3.0
rho = 0.5
for i in range(5):
    x0 = x1 = 10.0**i
    p = ot.DistFunc.pStudent2D(nu, x0, x1, rho)
    print(f"pStudent2D(nu={nu}, x0={x0}, x1={x1}, rho={rho})={p:.6g}")

# check for nans
for x in [
    -1e300,
    -1e200,
    -1e100,
    1e10,
    -10.0,
    -0.1,
    0.0,
    0.1,
    10.0,
    1e10,
    1e100,
    1e200,
    1e300,
]:
    for nu in [2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5]:
        for tail in [False, True]:
            p = ot.DistFunc.pStudent(nu, x, tail)
            assert m.isfinite(p), "pStudent returns nan"

# qStudent
nuMin = 0.2
nuMax = 5.0
n1 = 5
qMin = 0.1
qMax = 0.9
nQ = 10
grid = [0.0] * nQ
for i1 in range(n1):
    nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1)
    for iQ in range(nQ):
        q = qMin + (qMax - qMin) * iQ / (nQ - 1)
        grid[iQ] = q
        print(
            "qStudent(",
            nu,
            ",  %.12g" % q,
            ")=%.6g" % ot.DistFunc.qStudent(nu, q),
            ", complementary=%.6g" % ot.DistFunc.qStudent(nu, q, True),
        )
print("qStudent(", grid, ")=", ot.DistFunc.qStudent(nu, grid))
# rStudent
nuMin = 0.2
nuMax = 5.0
n1 = 5
nR = 10
for i1 in range(n1):
    nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1)
    for iR in range(nR):
        print("rStudent(", nu, ")=%.6g" % ot.DistFunc.rStudent(nu))

# Check Student.computeCDF and computeProbability for dim=2
nu = 3.0
rho = 0.5
a = [-0.5, -0.5]
b = [0.5, 0.5]
dist = ot.Student(
    nu, [0.0, 0.0], [1.0, 1.0], ot.CorrelationMatrix(2, [1.0, rho, rho, 1.0])
)
cdf_pt = dist.computeCDF([0.5, 0.5])
print(f"Student({nu}).computeCDF([0.5,0.5])={cdf_pt:.6g}")
prob = dist.computeProbability(ot.Interval(a, b))
print(f"Student({nu}).computeProbability([{a[0]},{b[0]}]x[{a[1]},{b[1]}])={prob:.6g}")
# CDF via DistFunc
p_bb = ot.DistFunc.pStudent2D(nu, b[0], b[1], rho)
p_ab = ot.DistFunc.pStudent2D(nu, a[0], b[1], rho)
p_ba = ot.DistFunc.pStudent2D(nu, b[0], a[1], rho)
p_aa = ot.DistFunc.pStudent2D(nu, a[0], a[1], rho)
p_ie = p_bb - p_ab - p_ba + p_aa
print(
    f"pStudent2D(ie, nu={nu}, a1={a[0]}, a2={a[1]}, b1={b[0]}, b2={b[1]}, rho={rho})={p_ie:.6g}"
)
# Tail probability via symmetry
p_tail = ot.DistFunc.pStudent2D(nu, 0.5, 0.5, rho, True)
print(f"pStudent2D(nu={nu}, 0.5, 0.5, rho={rho}, tail=True)={p_tail:.6g}")

# Check Student.computeCDF and computeProbability for dim=3
R3 = ot.CorrelationMatrix(3)
R3[0, 1] = 0.3
R3[0, 2] = 0.2
R3[1, 2] = 0.1
dist3 = ot.Student(nu, [0.0, 0.0, 0.0], [1.0, 1.0, 1.0], R3)
cdf_pt3 = dist3.computeCDF([0.5, 0.5, 0.5])
print(f"Student({nu}).computeCDF([0.5,0.5,0.5])={cdf_pt3:.6g}")
prob3 = dist3.computeProbability(ot.Interval([-0.5, -0.5, -0.5], [0.5, 0.5, 0.5]))
print(f"Student({nu}).computeProbability([-0.5,0.5]^3)={prob3:.6g}")
# CDF via pStudent3D
p3_pt = ot.DistFunc.pStudent3D(nu, 0.5, 0.5, 0.5, R3[0, 1], R3[0, 2], R3[1, 2])
print(f"pStudent3D(nu={nu}, 0.5, 0.5, 0.5, corr)={p3_pt:.6g}")
# Tail
p3_tail = ot.DistFunc.pStudent3D(nu, 0.5, 0.5, 0.5, R3[0, 1], R3[0, 2], R3[1, 2], True)
print(f"pStudent3D(nu={nu}, 0.5, 0.5, 0.5, corr, tail=True)={p3_tail:.6g}")
# Rectangular via inclusion-exclusion
p3_rect = ot.DistFunc.pStudent3D(
    nu, -0.5, -0.5, -0.5, 0.5, 0.5, 0.5, R3[0, 1], R3[0, 2], R3[1, 2]
)
print(f"pStudent3D(nu={nu}, [-0.5,0.5]^3, corr)={p3_rect:.6g}")

# pStudentND (Genz algorithm)
print("MVT d=2 independent")
L = ot.TriangularMatrix(2)
L[0, 0] = 1.0
L[1, 0] = 0.0
L[1, 1] = 1.0
p = ot.DistFunc.pStudentND([-1.0, -1.0], [1.0, 1.0], L, 5.0, 10000)
ott.assert_almost_equal(p, 0.5, 2e-1)

print("MVT d=2 correlated")
sigma = ot.CovarianceMatrix(2)
sigma[0, 0] = 1.0
sigma[1, 0] = 0.5
sigma[1, 1] = 1.0
p = ot.DistFunc.pStudentND([-1.0, -1.0], [1.0, 1.0], sigma, 5.0, 10000)
ott.assert_almost_equal(p, 0.5, 2e-1)

print("MVT d=2 with mu")
sigma = ot.CovarianceMatrix(2)
sigma[0, 0] = 1.0
sigma[1, 0] = 0.3
sigma[1, 1] = 1.0
p = ot.DistFunc.pStudentND([-1.0, -1.0], [1.0, 1.0], [0.5, -0.5], sigma, 5.0, 10000)
ott.assert_almost_equal(p, 0.3485, 2e-1)

print("Error: nu <= 0")
L = ot.TriangularMatrix(1)
L[0, 0] = 1.0
with ott.assert_raises(TypeError):
    ot.DistFunc.pStudentND([-1.0], [1.0], L, 0.0)

print("MVT d=1 exact")
L = ot.TriangularMatrix(1)
L[0, 0] = 1.0
p = ot.DistFunc.pStudentND([-1.0], [1.0], L, 100.0, 10000)
expected = ot.DistFunc.pStudent(100.0, 1.0) - ot.DistFunc.pStudent(100.0, -1.0)
ott.assert_almost_equal(p, expected, 1e-4)
