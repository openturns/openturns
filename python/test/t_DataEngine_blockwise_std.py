#! /usr/bin/env python

import math
import openturns as ot

ot.TESTPREAMBLE()


def max_abs_diff(a, b, step):
    err = 0.0
    n = len(a)
    k = 0
    while k < n:
        err = max(err, abs(a[k] - b[k]))
        k += step
    return err


# Scaled SPD system: G = M^T M + n I, n = 1000 (multi-block, non-divisible sizes)
n = 1000
M = ot.DataContainer(n, n, 0.0)
for j in range(n):
    base = j * n
    for i in range(n):
        M[base + i] = math.sin(0.01 * i + 0.03 * j) + 0.5 * math.cos(0.02 * i * j)
G = ot.AlgebraEngine.ComputeGram(M, True)
for i in range(n):
    G[i + i * n] += n

# Cholesky dense vs blockwise: bs = 1, 7 (non-divisor), 384 (default), 2000 (> n)
L = ot.AlgebraEngine.ComputeCholesky(G)
for bs in [1, 7, 384, 2000]:
    Lb = ot.AlgebraEngine.ComputeCholeskyBlockwise(G, bs)
    assert max_abs_diff(L, Lb, 9973) < 1e-6
print("Scaled ComputeCholeskyBlockwise: OK")

# SPD solve with 3 right-hand sides, dense vs blockwise (block path is SPD-only)
B = ot.DataContainer(n, 3, 0.0)
for j in range(3):
    base = j * n
    for i in range(n):
        B[base + i] = math.cos(0.05 * i + j)
X = ot.AlgebraEngine.SolveLinearSystemSPD(G, B)
Xb = ot.AlgebraEngine.SolveLinearSystemBlockwise(G, B, 100)
assert max_abs_diff(X, Xb, 991) < 1e-6
print("Scaled SolveLinearSystemBlockwise: OK")

# Tall rectangular least-squares: 1200 x 100, dense vs blockwise
m, p = 1200, 100
A = ot.DataContainer(m, p, 0.0)
for j in range(p):
    base = j * m
    for i in range(m):
        A[base + i] = math.sin(0.01 * i + 0.11 * j) + (1.0 if i % p == j else 0.0)
b = ot.DataContainer(m, 1.0)
x = ot.AlgebraEngine.SolveLinearSystemRectangular(A, b)
for bs in [1, 100, 500]:
    xb = ot.AlgebraEngine.SolveLinearSystemRectangularBlockwise(A, b, bs)
    assert max_abs_diff(x, xb, 17) < 1e-6
print("Scaled SolveLinearSystemRectangularBlockwise: OK")

# Statistics at scale: 200000 x 4 row-major (crosses the parallel threshold)
rows, dim = 200000, 4
S = ot.DataContainer(rows, dim, 0.0, ot.DataContainer.ROW_MAJOR)
total = rows * dim
for k in range(total):
    S[k] = ((k * 2654435761) % 1000) / 1000.0
mean = ot.StatisticsEngine.ComputeMean(S)
var = ot.StatisticsEngine.ComputeVariance(S)
cov = ot.StatisticsEngine.ComputeCovariance(S)
# bs = 3000 does not divide 200000: exercises the trailing partial block
meanB = ot.StatisticsEngine.ComputeMeanBlockwise(S, 3000)
varB = ot.StatisticsEngine.ComputeVarianceBlockwise(S, 3000)
covB = ot.StatisticsEngine.ComputeCovarianceBlockwise(S, 3000)
assert max_abs_diff(mean, meanB, 1) < 1e-9
assert max_abs_diff(var, varB, 1) < 1e-9
assert max_abs_diff(cov, covB, 1) < 1e-9
# default block sizes also agree (single code path, default keys)
assert max_abs_diff(mean, ot.StatisticsEngine.ComputeMeanBlockwise(S), 1) < 1e-9
assert max_abs_diff(cov, ot.StatisticsEngine.ComputeCovarianceBlockwise(S), 1) < 1e-9
print("Scaled statistics Blockwise: OK")

# Streaming statistics twins: dense vs blockwise at several block sizes
t = ot.DataContainer(7, 3, 0.0, ot.DataContainer.ROW_MAJOR)
for k in range(21):
    t[k] = ((k * 37) % 11) - 5.0
for bs in [1, 2, 5, 100]:
    s = ot.StatisticsEngine.ComputeSum(t)
    sb = ot.StatisticsEngine.ComputeSumBlockwise(t, bs)
    assert max_abs_diff(s, sb, 1) == 0.0
    assert max_abs_diff(ot.StatisticsEngine.ComputeMin(t), ot.StatisticsEngine.ComputeMinBlockwise(t, bs), 1) == 0.0
    assert max_abs_diff(ot.StatisticsEngine.ComputeMax(t), ot.StatisticsEngine.ComputeMaxBlockwise(t, bs), 1) == 0.0
    assert max_abs_diff(ot.StatisticsEngine.ComputeStandardDeviation(t), ot.StatisticsEngine.ComputeStandardDeviationBlockwise(t, bs), 1) < 1e-12
    assert max_abs_diff(ot.StatisticsEngine.ComputeRawMoment(t, 3), ot.StatisticsEngine.ComputeRawMomentBlockwise(t, 3, bs), 1) < 1e-12
    assert max_abs_diff(ot.StatisticsEngine.ComputeCentralMoment(t, 3), ot.StatisticsEngine.ComputeCentralMomentBlockwise(t, 3, bs), 1) < 1e-12
# order-0 moments return ones on both paths
assert list(ot.StatisticsEngine.ComputeRawMomentBlockwise(t, 0, 2)) == [1.0, 1.0, 1.0]
assert list(ot.StatisticsEngine.ComputeCentralMomentBlockwise(t, 0, 2)) == [1.0, 1.0, 1.0]
print("Streaming statistics Blockwise: OK")

# Composed statistics twins: skew/kurtosis (both flags), Pearson, ECDF
big7 = ot.DataContainer(50, 3, 0.0, ot.DataContainer.ROW_MAJOR)
for k in range(150):
    big7[k] = math.sin(0.3 * k) + 0.5 * math.cos(0.11 * k + 1.0)
loc = ot.DataContainer(3, 0.25)
for bs in [1, 7, 100]:
    for adjusted in [True, False]:
        assert max_abs_diff(ot.StatisticsEngine.ComputeSkewness(big7, adjusted), ot.StatisticsEngine.ComputeSkewnessBlockwise(big7, adjusted, bs), 1) < 1e-12
        assert max_abs_diff(ot.StatisticsEngine.ComputeKurtosis(big7, adjusted), ot.StatisticsEngine.ComputeKurtosisBlockwise(big7, adjusted, bs), 1) < 1e-12
    assert max_abs_diff(ot.StatisticsEngine.ComputePearsonCorrelation(big7), ot.StatisticsEngine.ComputePearsonCorrelationBlockwise(big7, bs), 1) < 1e-12
    assert abs(ot.StatisticsEngine.ComputeEmpiricalCDF(big7, loc, False) - ot.StatisticsEngine.ComputeEmpiricalCDFBlockwise(big7, loc, False, bs)) == 0.0
    assert abs(ot.StatisticsEngine.ComputeEmpiricalCDF(big7, loc, True) - ot.StatisticsEngine.ComputeEmpiricalCDFBlockwise(big7, loc, True, bs)) == 0.0
print("Composed statistics Blockwise: OK")

# Transpose twin: bitwise-identical at several tile sizes, non-square shapes
tall = ot.DataContainer(17, 5, 0.0)
for k in range(85):
    tall[k] = math.sin(0.2 * k) - 0.5
refT = ot.AlgebraEngine.Transpose(tall)
assert refT.getSize() == 5 and refT.getDimension() == 17
for bs in [1, 4, 5, 16, 100]:
    tb = ot.AlgebraEngine.TransposeBlockwise(tall, bs)
    assert len(tb) == len(refT)
    assert max_abs_diff(refT, tb, 1) == 0.0
print("TransposeBlockwise: OK")


# Singular values twin: tall (30x20) and wide (12x20) vs dense dgesdd path
def scaled_matrix(m, p):
    a = ot.DataContainer(m, p, 0.0)
    for j in range(p):
        base = j * m
        for i in range(m):
            a[base + i] = math.sin(0.13 * i + 0.29 * j) + 0.5 * math.cos(0.07 * i * j + 0.5)
    return a


for (m, p) in [(30, 20), (12, 20)]:
    a = scaled_matrix(m, p)
    ref = ot.AlgebraEngine.ComputeSingularValues(a)
    for bs in [1, 7, 100]:
        sb = ot.AlgebraEngine.ComputeSingularValuesBlockwise(a, bs)
        assert sb.getSize() == min(m, p)
        assert max_abs_diff(ref, sb, 1) / (1.0 + ref[0]) < 1e-8
print("ComputeSingularValuesBlockwise: OK")

print("=== ALL BLOCKWISE SCALE TESTS PASSED ===")
