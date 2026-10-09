#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import numpy as np

ot.TESTPREAMBLE()

# Adversarial configuration: HMAT GP sampling must use the Cholesky factor
# regardless of the global factorization method. With "LU" the hmat-oss gemv
# would reconstruct C.z instead of L.z, silently sampling N(0, C^2).
ot.ResourceMap.SetAsString("HMatrix-FactorizationMethod", "LU")

mesh = ot.IntervalMesher([30]).build(ot.Interval([0.0], [1.0]))
vertices = mesh.getVertices()
n = vertices.getSize()
covModel = ot.MaternModel([0.2], [1.0], 2.5)

# Dense reference matrix (the HMAT path does not apply any nugget)
A = np.array(
    [[covModel.computeAsScalar(ot.Point(vertices[i]), ot.Point(vertices[j])) for j in range(n)]
     for i in range(n)]
)


def sample_gp(samplingMethod, numberOfRealizations):
    gp = ot.GaussianProcess(covModel, mesh)
    gp.setSamplingMethod(samplingMethod)
    rows = []
    for _ in range(numberOfRealizations):
        field = gp.getRealization()
        rows.append([field[i, 0] for i in range(n)])
    return np.array(rows)


def rel_fro(M1, M2):
    return np.linalg.norm(M1 - M2) / np.linalg.norm(M2)


# === Test 1: HMAT sampling must have covariance A (not A^2) even with LU set ===
print("=== Test 1: HMAT GP sampling reproduces the covariance ===")
ot.RandomGenerator.SetSeed(42)
Y = sample_gp(ot.GaussianProcess.HMAT, 5000)
Khat = np.cov(Y, rowvar=False)

err_vs_A = rel_fro(Khat, A)
err_vs_A2 = rel_fro(Khat, A.dot(A))
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

# === Test 2: HMAT and Cholesky sampling agree statistically ===
print("=== Test 2: HMAT agrees with Cholesky sampling ===")
ot.RandomGenerator.SetSeed(42)
Yc = sample_gp(ot.GaussianProcess.CHOLESKY, 5000)
Kc = np.cov(Yc, rowvar=False)
diff = np.linalg.norm(Khat - Kc) / np.linalg.norm(Kc)
ott.assert_almost_equal(ot.Point([diff]), ot.Point([0.0]), 0.15, 0.15)
print("  diff=%.3e" % diff)
print("  PASS")

print("\n=== ALL TESTS PASSED ===")
