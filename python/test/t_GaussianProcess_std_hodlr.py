#! /usr/bin/env python

import math

import openturns as ot
import openturns.testing as ott

ot.ResourceMap.SetAsUnsignedInteger("HODLRMatrix-MinLeafSize", 4)
ot.ResourceMap.SetAsScalar("HODLRMatrix-AssemblyEpsilon", 1.0e-8)
ot.ResourceMap.SetAsScalar("HODLRMatrix-RecompressionEpsilon", 1.0e-8)

mesh = ot.IntervalMesher([30]).build(ot.Interval([0.0], [1.0]))
vertices = mesh.getVertices()
n = vertices.getSize()
covModel = ot.MaternModel([0.2], [1.0], 2.5)

# Dense reference matrix, including the nugget added by discretizeHODLRMatrix
nugget = ot.ResourceMap.GetAsScalar("HODLRMatrix-Nugget")
A = covModel.discretize(mesh)
for i in range(n):
    A[i, i] += nugget


def sample_gp(samplingMethod, numberOfRealizations):
    gp = ot.GaussianProcess(covModel, mesh)
    gp.setSamplingMethod(samplingMethod)
    rows = []
    for _ in range(numberOfRealizations):
        field = gp.getRealization()
        rows.append([field[i, 0] for i in range(n)])
    return ot.Sample(rows)


def rel_fro(M1, M2):
    # element-wise relative Frobenius norm; computed by hand because the
    # matrix "-" operator can share the copy-on-write buffer of its operands
    # (and even mutate it), and because frobeniusNorm() of a symmetric
    # (half-stored) matrix only sums the stored triangle
    num = 0.0
    den = 0.0
    for i in range(n):
        for j in range(n):
            d = M1[i, j] - M2[i, j]
            num += d * d
            den += M2[i, j] * M2[i, j]
    return math.sqrt(num / den)


# === Test 1: HODLRMatrix.applyFactor matches the dense Cholesky factor ===
print("=== Test 1: applyFactor matches the dense Cholesky factor ===")
hodlr = covModel.discretizeHODLRMatrix(mesh, ot.HODLRMatrixParameters())
hodlr.factorize()
L = A.computeCholesky()
ot.RandomGenerator.SetSeed(0)
normal = ot.Normal(n)
for k in range(5):
    x = normal.getRealization()
    y = ot.Point(n, 0.0)
    hodlr.applyFactor(y, ot.Point(x))
    y_ref = L * x
    ott.assert_almost_equal(y, ot.Point(y_ref), 1.0e-5, 1.0e-5)

# gemv must still reproduce the full product A x (L L^T x)
x_test = normal.getRealization()
y_dense = A * x_test
y_gemv = ot.Point(n, 0.0)
hodlr.gemv('N', 1.0, x_test, 0.0, y_gemv)
ott.assert_almost_equal(y_gemv, ot.Point(y_dense), 1.0e-5, 1.0e-5)
print("  PASS")

# === Test 2: HODLR GP sampling must have covariance A (not A^2) ===
print("=== Test 2: HODLR GP sampling reproduces the covariance ===")
ot.RandomGenerator.SetSeed(42)
Y = sample_gp(ot.GaussianProcess.HODLR, 5000)
Khat = Y.computeCovariance()

err_vs_A = rel_fro(Khat, A)
err_vs_A2 = rel_fro(Khat, A * A)
# with a correct sampler the empirical covariance matches A, and is much
# closer to A than to A^2 (the previous implementation sampled N(0, A^2))
ott.assert_almost_equal(ot.Point([err_vs_A]), ot.Point([0.0]), 0.15, 0.15)
assert err_vs_A < err_vs_A2, (
    "sampled covariance should be closer to A than to A^2 "
    "(err_vs_A=%.3e, err_vs_A2=%.3e)" % (err_vs_A, err_vs_A2)
)
# vertex variance is ~1: N(0, A^2) would give a variance of about sum_j A(0,j)^2
var0 = Khat[0, 0]
ott.assert_almost_equal(ot.Point([var0]), ot.Point([1.0]), 0.15, 0.15)
print("  err_vs_A=%.3e, err_vs_A2=%.3e, var0=%.3f" % (err_vs_A, err_vs_A2, var0))
print("  PASS")

# === Test 3: HODLR and Cholesky sampling agree statistically ===
print("=== Test 3: HODLR agrees with Cholesky sampling ===")
ot.RandomGenerator.SetSeed(42)
Yc = sample_gp(ot.GaussianProcess.CHOLESKY, 5000)
Kc = Yc.computeCovariance()
diff = rel_fro(Khat, Kc)
ott.assert_almost_equal(ot.Point([diff]), ot.Point([0.0]), 0.15, 0.15)
print("  diff=%.3e" % diff)
print("  PASS")

print("\n=== ALL TESTS PASSED ===")
