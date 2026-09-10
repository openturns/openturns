#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m
import random
import itertools
import os

ot.PlatformInfo.SetNumericalPrecision(3)
ot.ResourceMap.SetAsUnsignedInteger("HODLRMatrix-MinLeafSize", 4)
ot.ResourceMap.SetAsScalar("HODLRMatrix-AssemblyEpsilon", 1.0e-6)
ot.ResourceMap.SetAsScalar("HODLRMatrix-RecompressionEpsilon", 1.0e-6)


class HODLRTestAssemblyFunction:
    def __init__(self, vertices, lengthScale=0.1):
        self.vertices = vertices
        self.lengthScale = lengthScale

    def __call__(self, i, j):
        pt1 = self.vertices[i]
        pt2 = self.vertices[j]
        diff = pt1 - pt2
        return m.exp(-diff.norm() / self.lengthScale)


def build_dense_matrix(assembly_func, size):
    K = ot.Matrix(size, size)
    for i in range(size):
        for j in range(size):
            K[i, j] = assembly_func(i, j)
    return K


def make_params(epsilon=1.0e-6, leaf=4):
    p = ot.HODLRMatrixParameters()
    p.setAssemblyEpsilon(epsilon)
    p.setRecompressionEpsilon(epsilon)
    p.setMinLeafSize(leaf)
    return p


def build_hodlr(vertices, af, symmetric=True, params=None):
    if params is None:
        params = make_params()
    factory = ot.HODLRMatrixFactory()
    hodlr = factory.build(vertices, 1, symmetric, params)
    hodlr.assembleReal(af, 'L')
    hodlr.factorize()
    return hodlr


def capture_stderr(callable):
    """Run callable with the process stderr redirected and return its output."""
    r, w = os.pipe()
    saved = os.dup(2)
    os.dup2(w, 2)
    try:
        callable()
    finally:
        ot.Log.Flush()
        os.dup2(saved, 2)
        os.close(saved)
        os.close(w)
    return os.read(r, 65536).decode(errors="replace")


def setup_20():
    vertices = ot.Sample([[i * 0.1] for i in range(20)])
    af = HODLRTestAssemblyFunction(vertices, 0.1)
    hodlr = build_hodlr(vertices, af)
    K = build_dense_matrix(af, 20)
    return vertices, af, hodlr, K


# === Test 1: getNbRows / getNbColumns ===
print("=== Test 1: getNbRows / getNbColumns ===")
vertices, af, hodlr, K = setup_20()
ott.assert_almost_equal(ot.Point([hodlr.getNbRows()]), ot.Point([20]))
ott.assert_almost_equal(ot.Point([hodlr.getNbColumns()]), ot.Point([20]))
print("  PASS")

# === Test 2: copy + re-assemble ===
print("\n=== Test 2: copy ===")
hodlr_copy = hodlr.copy()
ott.assert_almost_equal(
    ot.Point([hodlr_copy.getNbRows()]),
    ot.Point([hodlr.getNbRows()])
)
# copy() drops the tree: it reports its true condition until re-assembled
with ott.assert_raises(TypeError):
    hodlr_copy.logDeterminant()
# copy() preserves dimension but not tree -- re-assemble
hodlr_copy.assembleReal(af, 'L')
hodlr_copy.factorize()
b = ot.Point(20, 1.0)
ott.assert_almost_equal(hodlr_copy.solve(b), hodlr.solve(b), 1.0e-10, 1.0e-10)
print("  PASS")

# === Test 3: __repr__ / __str__ ===
print("\n=== Test 3: __repr__ / __str__ ===")
repr_str = repr(hodlr)
str_str = str(hodlr)
assert len(repr_str) > 0, "repr() should not be empty"
assert len(str_str) > 0, "str() should not be empty"
print("  PASS")

# === Test 4: getDiagonal ===
print("\n=== Test 4: getDiagonal ===")
diag = hodlr.getDiagonal()
ott.assert_almost_equal(ot.Point([diag.getSize()]), ot.Point([20]))
# exp(-||x_i-x_j||/L) at i==j is exp(0)=1
ott.assert_almost_equal(diag, ot.Point(20, 1.0), 0.15, 0.15)
print("  PASS")

# === Test 5: solve(Point) ===
print("\n=== Test 5: solve(Point) ===")
b5 = ot.Point(20, 1.0)
x_h = hodlr.solve(b5)
x_d = K.solveLinearSystem(b5)
ott.assert_almost_equal(x_h, x_d, 1.0e-4, 1.0e-4)
print("  PASS")

# === Test 6: solve(Matrix) - multi-RHS ===
print("\n=== Test 6: solve(Matrix) ===")
B = ot.Matrix(20, 3)
for i in range(20):
    B[i, 0] = 1.0
    B[i, 1] = float(i) / 20.0
    B[i, 2] = m.sin(float(i))
X_h = hodlr.solve(B)
X_d = K.solveLinearSystem(B)
for col in range(3):
    ott.assert_almost_equal(
        ot.Point([X_h[i, col] for i in range(20)]),
        ot.Point([X_d[i, col] for i in range(20)]),
        1.0e-4, 1.0e-4
    )
print("  PASS")

# === Test 7: scale ===
print("\n=== Test 7: scale ===")
hodlr_s = hodlr.copy()
hodlr_s.assembleReal(af, 'L')
hodlr_s.factorize()
scale_factor = 3.5
hodlr_s.scale(scale_factor)
# scale() rebuilds the tree with a scaled evaluator and invalidates the
# factorization until factorize() is called again
b7 = ot.Point(20, 1.0)
with ott.assert_raises(TypeError):
    hodlr_s.solve(b7)
hodlr_s.factorize()
# solve(alpha*A, b) = solve(A, b)/alpha
x_s = hodlr_s.solve(b7)
x_orig = hodlr.solve(b7)
ott.assert_almost_equal(x_s, x_orig / scale_factor, 1.0e-8, 1.0e-8)
# logDet(alpha*A) = n*log(alpha) + logDet(A)
ott.assert_almost_equal(
    ot.Point([hodlr_s.logDeterminant()]),
    ot.Point([hodlr.logDeterminant() + 20 * m.log(scale_factor)]),
    1.0e-8, 1.0e-8
)
# diagonal is scaled too
ott.assert_almost_equal(
    hodlr_s.getDiagonal(), hodlr.getDiagonal() * scale_factor, 1.0e-10, 1.0e-10
)
print("  PASS")

# === Test 8: addIdentity ===
print("\n=== Test 8: addIdentity ===")
hodlr_ai = hodlr.copy()
hodlr_ai.assembleReal(af, 'L')
hodlr_ai.factorize()
alpha = 2.0
hodlr_ai.addIdentity(alpha)
ott.assert_almost_equal(
    hodlr_ai.getDiagonal(), hodlr.getDiagonal() + ot.Point(20, alpha), 1.0e-10, 1.0e-10
)
print("  PASS")

# === Test 9: compressionRatio / fullrkRatio ===
print("\n=== Test 9: compressionRatio / fullrkRatio ===")
cr = hodlr.compressionRatio()
fr = hodlr.fullrkRatio()
assert cr[0] > 0, "stored entries should be positive"
assert cr[0] <= cr[1], "compressed should be <= total"
assert fr[1] > 0, "rank should be positive"
print(f"  compressionRatio: stored={cr[0]}, total={cr[1]}")
print(f"  fullrkRatio: full={fr[0]}, rank={fr[1]}")
print("  PASS")

# === Test 10: HODLRMatrixParameters getters/setters ===
print("\n=== Test 10: HODLRMatrixParameters getters ===")
params = ot.HODLRMatrixParameters()
params.setAssemblyEpsilon(1.0e-3)
params.setRecompressionEpsilon(1.0e-5)
params.setMinLeafSize(16)
ott.assert_almost_equal(
    ot.Point([params.getAssemblyEpsilon()]), ot.Point([1.0e-3])
)
ott.assert_almost_equal(
    ot.Point([params.getRecompressionEpsilon()]), ot.Point([1.0e-5])
)
ott.assert_almost_equal(ot.Point([params.getMinLeafSize()]), ot.Point([16]))
print("  PASS")

# === Test 12: LLt factorization ===
print("\n=== Test 12: LLt factorization ===")
n12 = 50
vertices12 = ot.Sample([[i * 0.02] for i in range(n12)])
af12 = HODLRTestAssemblyFunction(vertices12, 0.1)
params12 = make_params(leaf=25)
params12.setMaxRank(50)
hodlr12 = build_hodlr(vertices12, af12, params=params12)
K12 = build_dense_matrix(af12, n12)
b12 = ot.Point(n12, 1.0)
x_h12 = hodlr12.solve(b12)
x_d12 = K12.solveLinearSystem(b12)
ott.assert_almost_equal(x_h12, x_d12, 1.0e-4, 1.0e-4)
print("  PASS")

# === Test 13: solveLower ===
print("\n=== Test 13: solveLower ===")
n13 = 20
vertices13 = ot.Sample([[i * 0.05] for i in range(n13)])
af13 = HODLRTestAssemblyFunction(vertices13, 0.1)
b13 = ot.Point(n13, 1.0)
# solveLower(b) = L^{-1} b, solveLower(b, True) = L^{-T} b
params13 = make_params(leaf=25)
params13.setMaxRank(50)
hodlr13 = build_hodlr(vertices13, af13, params=params13)
K13 = ot.CovarianceMatrix(n13)
for i in range(n13):
    for j in range(n13):
        K13[i, j] = af13(i, j)
L13 = K13.computeCholesky()
x_lower = hodlr13.solveLower(b13)
x_forward = L13.solveLinearSystem(b13)
ott.assert_almost_equal(x_lower, x_forward, 1.0e-4, 1.0e-4)
x_lower_t = hodlr13.solveLower(b13, True)
x_backward = L13.transpose().solveLinearSystem(b13)
ott.assert_almost_equal(x_lower_t, x_backward, 1.0e-4, 1.0e-4)
# L^{-T} L^{-1} b must reproduce the full solve C^{-1} b
x_composed = hodlr13.solveLower(hodlr13.solveLower(b13), True)
x_full = hodlr13.solve(b13)
ott.assert_almost_equal(x_composed, x_full, 1.0e-4, 1.0e-4)
print("  PASS (Point)")

B13 = ot.Matrix(n13, 2)
for i in range(n13):
    B13[i, 0] = 1.0
    B13[i, 1] = float(i)
X_lower = hodlr13.solveLower(B13)
X_forward = L13.solveLinearSystem(B13)
ott.assert_almost_equal(X_lower, X_forward, 1.0e-4, 1.0e-4)
print("  PASS (Matrix)")

# === Test 13b: gemv with LLt factorization ===
print("\n=== Test 13b: gemv with LLt ===")
# gemv must return A * x (not L * x) when the matrix is Cholesky-factorized
x13b = ot.Point([float(i) / n12 for i in range(n12)])
y13b = ot.Point(n12, 0.0)
hodlr12.gemv('N', 1.0, x13b, 0.0, y13b)
y13b_dense = ot.Point(n12, 0.0)
for i in range(n12):
    s = 0.0
    for j in range(n12):
        s += af12(i, j) * x13b[j]
    y13b_dense[i] = s
ott.assert_almost_equal(y13b, y13b_dense, 1.0e-4, 1.0e-4)
print("  PASS")

# === Test 14: solve with trans ===
print("\n=== Test 14: solve with trans=True ===")
b14 = ot.Point(20, 1.0)
x_normal = hodlr.solve(b14, False)
# The transposed solve is not implemented: trans=True must be rejected
with ott.assert_raises(RuntimeError):
    hodlr.solve(b14, True)
print("  PASS")

# === Test 15: 1D line, n=50 ===
print("\n=== Test 15: 1D line, n=50 ===")
vertices50 = ot.Sample([[i * 0.02] for i in range(50)])
af50 = HODLRTestAssemblyFunction(vertices50, 0.1)
params15 = make_params(leaf=25)
params15.setMaxRank(50)
hodlr50 = build_hodlr(vertices50, af50, params=params15)
K50 = build_dense_matrix(af50, 50)
b50 = ot.Point(50, 1.0)
ott.assert_almost_equal(hodlr50.solve(b50), K50.solveLinearSystem(b50), 1.0e-6, 1.0e-6)
print(f"  logDet= {hodlr50.logDeterminant():.6f}")
print("  PASS")

# === Test 16: gemv ===
print("\n=== Test 16: gemv ===")
x16 = [float(i) / 20 for i in range(20)]
x16_pt = ot.Point(x16)
y16 = ot.Point(20, 0.0)
hodlr.gemv('N', 1.0, x16_pt, 0.0, y16)
# Compare against dense matvec: y = K * x
y16_dense = ot.Point(20, 0.0)
for i in range(20):
    s = 0.0
    for j in range(20):
        s += af(i, j) * x16[j]
    y16_dense[i] = s
ott.assert_almost_equal(y16, y16_dense, 1.0e-6, 1.0e-6)
print("  PASS")

# === Test 17: gemv with alpha/beta ===
print("\n=== Test 17: gemv with alpha/beta ===")
x17 = [1.0] * 20
y17_init = [2.0] * 20
x17_pt = ot.Point(x17)
y17_pt = ot.Point(y17_init)
hodlr.gemv('N', 3.0, x17_pt, -1.0, y17_pt)
# y = 3*A*x - 1*y_init
y17_ref = ot.Point(20, 0.0)
for i in range(20):
    s = 0.0
    for j in range(20):
        s += af(i, j)
    y17_ref[i] = 3.0 * s - 2.0
ott.assert_almost_equal(y17_pt, y17_ref, 1.0e-10, 1.0e-10)
print("  PASS")

# === Test 18: HODLRMatrixFactory __repr__ ===
print("\n=== Test 18: HODLRMatrixFactory __repr__ ===")
factory = ot.HODLRMatrixFactory()
repr_f = repr(factory)
assert len(repr_f) > 0, "repr() should not be empty"
print("  PASS")

# === Test 19: HODLRMatrixParameters __repr__ / __str__ ===
print("\n=== Test 19: HODLRMatrixParameters __repr__ / __str__ ===")
assert len(repr(params)) > 0, "repr() should not be empty"
assert len(str(params)) > 0, "str() should not be empty"
print("  PASS")

# === Test 20: 2D grid, n=100 ===
print("\n=== Test 20: 2D grid, n=100 ===")
n20 = 9
intervalMesher = ot.IntervalMesher([n20, n20])
interval = ot.Interval([0.0] * 2, [1.0] * 2)
mesh2D = intervalMesher.build(interval)
vertices20 = mesh2D.getVertices()
size20 = vertices20.getSize()
af20 = HODLRTestAssemblyFunction(vertices20, 0.2)
params20 = make_params(leaf=25)
params20.setMaxRank(50)
hodlr20 = build_hodlr(vertices20, af20, params=params20)
K20 = build_dense_matrix(af20, size20)
b20 = ot.Point(size20, 1.0)
ott.assert_almost_equal(hodlr20.solve(b20), K20.solveLinearSystem(b20), 1.0e-4, 1.0e-4)
print("  PASS")

# === Test 21: relative nugget via discretizeHODLRMatrix ===
print("\n=== Test 21: relative nugget ===")
nugget = 1.0e-4
ot.ResourceMap.SetAsScalar("HODLRMatrix-Nugget", nugget)
cov21 = ot.MaternModel([2.0], [1.0], 2.5)
grid21 = ot.Box([40]).generate()
grid21 *= 2.0
grid21 -= 1.0
n21 = grid21.getSize()
params21 = make_params(leaf=5)
params21.setMaxRank(40)
hodlr21 = cov21.discretizeHODLRMatrix(grid21, params21)
# kernel has unit amplitude: the diagonal grows by the relative nugget
diag_ref21 = ot.Point(n21, 1.0 + nugget)
ott.assert_almost_equal(hodlr21.getDiagonal(), diag_ref21, 1.0e-9, 1.0e-9)
hodlr21.factorize()
b21 = ot.Point(n21, 1.0)
K21 = ot.CovarianceMatrix(cov21.discretize(grid21))
for i in range(n21):
    K21[i, i] += nugget
ott.assert_almost_equal(hodlr21.solve(b21), K21.solveLinearSystem(b21), 1.0e-6, 1.0e-6)
ld21 = hodlr21.logDeterminant()
ld_ref21 = K21.computeLogAbsoluteDeterminant()[0]
ott.assert_almost_equal(ot.Point([ld21]), ot.Point([ld_ref21]), 1.0e-4, 1.0e-4)

# direct applyNugget on a factory-built matrix
ot.ResourceMap.SetAsScalar("HODLRMatrix-Nugget", 2.0e-3)
factory21 = ot.HODLRMatrixFactory()
hodlr21b = factory21.build(grid21, 1, True, make_params(leaf=5))
hodlr21b.assembleReal(HODLRTestAssemblyFunction(grid21, 0.1), 'L')
hodlr21b.applyNugget()
ott.assert_almost_equal(hodlr21b.getDiagonal(), ot.Point(n21, 1.0 + 2.0e-3), 1.0e-9, 1.0e-9)

# a zero nugget disables the regularization
ot.ResourceMap.SetAsScalar("HODLRMatrix-Nugget", 0.0)
hodlr21c = cov21.discretizeHODLRMatrix(grid21, params21)
ott.assert_almost_equal(hodlr21c.getDiagonal(), ot.Point(n21, 1.0), 1.0e-9, 1.0e-9)
ot.ResourceMap.SetAsScalar("HODLRMatrix-Nugget", 1.0e-8)
print("  PASS")

# === Test 22: adaptive max rank (tolerance-driven by default) ===
print("\n=== Test 22: adaptive max rank ===")
# the default max rank is zero: each block is compressed up to the
# assembly epsilon instead of being capped
params22 = ot.HODLRMatrixParameters()
ott.assert_almost_equal(ot.Point([params22.getMaxRank()]), ot.Point([0]))
# long-correlation 2D case: the adaptive default is accurate, a small
# explicit cap starves the blocks and degrades the solve
im22 = ot.IntervalMesher([8, 8])
mesh22 = im22.build(ot.Interval([0.0, 0.0], [1.0, 1.0]))
vertices22 = mesh22.getVertices()
vertices22 *= 2.0
vertices22 -= 1.0
n22 = vertices22.getSize()
cov22 = ot.MaternModel([1.0, 1.0], [1.0], 2.5)
K22 = ot.CovarianceMatrix(cov22.discretize(vertices22))
b22 = ot.Point(n22, 1.0)
x_dense22 = K22.solveLinearSystem(b22)

hodlr_adaptive22 = cov22.discretizeHODLRMatrix(vertices22, make_params(leaf=4))
hodlr_adaptive22.factorize()
x_adaptive22 = hodlr_adaptive22.solve(b22)
err_adaptive22 = (x_adaptive22 - x_dense22).norm() / x_dense22.norm()
assert err_adaptive22 < 1.0e-2, (
    f"adaptive rank solve error {err_adaptive22:.2e} should be small"
)

params_capped22 = make_params(leaf=4)
params_capped22.setMaxRank(2)
hodlr_capped22 = cov22.discretizeHODLRMatrix(vertices22, params_capped22)
hodlr_capped22.factorize()
x_capped22 = hodlr_capped22.solve(b22)
err_capped22 = (x_capped22 - x_dense22).norm() / x_dense22.norm()
assert err_capped22 > err_adaptive22, (
    "an explicit small max rank should degrade the solve accuracy"
)
print(f"  adaptive err= {err_adaptive22:.2e}, capped(maxRank=2) err= {err_capped22:.2e}")
print("  PASS")

# === Test 23: full-rank blocks fall back to dense storage ===
print("\n=== Test 23: dense fallback for full-rank blocks ===")
# Use a random SPD kernel: its off-diagonal blocks have no low-rank structure,
# so the ACA cannot converge below the tolerance and reaches min(rows, cols),
# and each block is stored densely (U = A01, V = I) instead of being truncated.
previous_nugget23 = ot.ResourceMap.GetAsScalar("HODLRMatrix-Nugget")
try:
    ot.ResourceMap.SetAsScalar("HODLRMatrix-Nugget", 0.0)
    n23 = 40
    random.seed(1234)
    B = [[random.random() - 0.5 for _ in range(n23)] for __ in range(n23)]
    K23 = ot.Matrix(n23, n23)
    b23 = ot.Point(n23, 1.0)
    for i in range(n23):
        for j in range(n23):
            s = sum(B[i][k] * B[j][k] for k in range(n23))
            K23[i, j] = s + (1.0 if i == j else 0.0)
    x_dense23 = K23.solveLinearSystem(b23)

    def af23(i, j):
        return K23[i, j]

    vertices23 = ot.Sample([[float(i)] for i in range(n23)])
    params23 = make_params(leaf=5)
    params23.setMaxRank(20)
    hodlr23 = build_hodlr(vertices23, af23, params=params23)
    x_h23 = hodlr23.solve(b23)
    err23 = (x_h23 - x_dense23).norm() / x_dense23.norm()
    assert err23 < 1.0e-8, f"dense fallback solve error {err23:.2e} should be small"
    cr23 = hodlr23.compressionRatio()
    # every block reached its full rank: the tree is stored densely
    assert cr23[0] == cr23[1], (
        f"full-rank tree should be stored densely ({cr23[0]}/{cr23[1]})"
    )
    print(f"  stored/total= {cr23[0]}/{cr23[1]}, solve err= {err23:.2e}")
    print("  PASS")
finally:
    ot.ResourceMap.SetAsScalar("HODLRMatrix-Nugget", previous_nugget23)

# === Test 24: spatial ordering (KDTree permutation) ===
print("\n=== Test 24: spatial ordering ===")
# the flag is ON by default
params24 = ot.HODLRMatrixParameters()
assert params24.getUseSpatialOrdering(), "spatial ordering should be ON by default"
# a factory-built matrix carries a valid permutation
factory24 = ot.HODLRMatrixFactory()
grid24 = ot.Sample([[i * 0.1] for i in range(40)])
hodlr24 = factory24.build(grid24, 1, True, params24)
perm24 = hodlr24.getPermutation()
assert perm24.getSize() == 40
assert sorted(perm24) == list(range(40)), "factory permutation must be a permutation"
# an already-ordered 1D grid yields the identity permutation
assert perm24 == list(range(40)), "an ordered 1D grid should give the identity permutation"
# scrambled vertices yield a non-trivial permutation
order24 = list(range(40))
random.Random(0).shuffle(order24)
scrambled24 = ot.Sample([[grid24[i][0]] for i in order24])
hodlr_sc24 = factory24.build(scrambled24, 1, True, params24)
assert hodlr_sc24.getPermutation() != list(range(40)), (
    "scrambled vertices should give a non-identity permutation"
)
# invalid permutations are rejected. The size is validated before the entry
# values, so the duplicate and bounds cases must use full-size permutations
# (size 40) to reach the value validation; the error message pins down which
# check actually fired.
with ott.assert_raises(TypeError):
    hodlr24.setPermutation([0, 1, 2])
caught = False
try:
    hodlr24.setPermutation([0] * 40)
except TypeError as e:
    msg = str(e)
    if "appears more than once" in msg:
        caught = True
assert caught, "a duplicate full-size permutation should be rejected as such"
caught = False
try:
    hodlr24.setPermutation(list(range(40))[:-1] + [40])
except TypeError as e:
    msg = str(e)
    if "out of range" in msg:
        caught = True
assert caught, "an out-of-range full-size permutation should be rejected as such"
# disabling the flag removes the permutation
params_off24 = make_params(leaf=4)
params_off24.setUseSpatialOrdering(False)
hodlr_off24 = factory24.build(scrambled24, 1, True, params_off24)
assert hodlr_off24.getPermutation().getSize() == 0, (
    "flag OFF should disable the permutation"
)
# scrambled 2D grid with a short correlation length: the spatial ordering
# preserves the accuracy and compresses the tree, while the blind split of
# the unordered vertices falls back to dense storage
previous_nugget24 = ot.ResourceMap.GetAsScalar("HODLRMatrix-Nugget")
try:
    # the diagonal is compared to the exact 1.0 entries, so no nugget may be
    # added during the HODLR assembly
    ot.ResourceMap.SetAsScalar("HODLRMatrix-Nugget", 0.0)
    im24 = ot.IntervalMesher([15, 15])
    mesh24 = im24.build(ot.Interval([0.0, 0.0], [1.0, 1.0]))
    vertices24 = mesh24.getVertices()
    n24 = vertices24.getSize()
    order2d24 = list(range(n24))
    random.Random(1).shuffle(order2d24)
    scrambled2d24 = ot.Sample([vertices24[i] for i in order2d24])
    cov24 = ot.MaternModel([0.2, 0.2], [1.0], 2.5)
    K24 = ot.CovarianceMatrix(cov24.discretize(scrambled2d24))
    b24 = ot.Point(n24, 1.0)
    x_dense24 = K24.solveLinearSystem(b24)
    hodlr_on24 = cov24.discretizeHODLRMatrix(scrambled2d24, make_params(leaf=4))
    hodlr_on24.factorize()
    x_on24 = hodlr_on24.solve(b24)
    err_on24 = (x_on24 - x_dense24).norm() / x_dense24.norm()
    hodlr_off24 = cov24.discretizeHODLRMatrix(scrambled2d24, params_off24)
    hodlr_off24.factorize()
    x_off24 = hodlr_off24.solve(b24)
    err_off24 = (x_off24 - x_dense24).norm() / x_dense24.norm()
    assert err_on24 < 1.0e-2, (
        f"spatial ordering solve error {err_on24:.2e} should be small"
    )
    cr_on24 = hodlr_on24.compressionRatio()
    cr_off24 = hodlr_off24.compressionRatio()
    assert cr_on24[0] < cr_off24[0], (
        "spatial ordering should compress the tree better than the blind split "
        f"({cr_on24[0]}/{cr_on24[1]} vs {cr_off24[0]}/{cr_off24[1]})"
    )
    # the permutation preserves the diagonal
    ott.assert_almost_equal(hodlr_on24.getDiagonal(), ot.Point(n24, 1.0), 1.0e-9, 1.0e-9)
    print(f"  on err= {err_on24:.2e}, on storage= {cr_on24[0]}/{cr_on24[1]}, off storage= {cr_off24[0]}/{cr_off24[1]}")
    print("  PASS")
finally:
    ot.ResourceMap.SetAsScalar("HODLRMatrix-Nugget", previous_nugget24)

# === Test 25: ACA with max-element (partial) pivoting ===
print("\n=== Test 25: ACA with max-element pivoting ===")
# The ACA is the only low-rank assembly method and uses max-element
# (partial-pivoting) pivots: on a smooth 1D kernel it reaches the assembly
# tolerance with a small rank per block and stays accurate.
previous_nugget25 = ot.ResourceMap.GetAsScalar("HODLRMatrix-Nugget")
try:
    # the solve is compared against the nugget-free dense reference K25, so
    # the HODLR assembly must not add any nugget
    ot.ResourceMap.SetAsScalar("HODLRMatrix-Nugget", 0.0)
    n25 = 2000
    vertices25 = ot.Sample([[i / (n25 - 1.0)] for i in range(n25)])
    cov25 = ot.MaternModel([0.1], [1.0], 2.5)
    b25 = ot.Point(n25, 1.0)
    K25 = ot.CovarianceMatrix(cov25.discretize(vertices25))
    x_dense25 = K25.solveLinearSystem(b25)

    params25 = make_params(leaf=16)
    hodlr25 = cov25.discretizeHODLRMatrix(vertices25, params25)
    hodlr25.factorize()
    x25 = hodlr25.solve(b25)
    err25 = (x25 - x_dense25).norm() / x_dense25.norm()
    assert err25 < 1.0e-2, f"max-element ACA solve error {err25:.2e} should be small"
    cr25 = hodlr25.compressionRatio()
    # the smooth 1D kernel is strongly compressible: less than half the entries
    # of the dense matrix are stored by the factorized tree
    assert cr25[0] < 0.5 * cr25[1], (
        f"max-element ACA should compress the tree ({cr25[0]}/{cr25[1]})"
    )
    print(f"  stored= {cr25[0]}/{cr25[1]}, solve err= {err25:.2e}")
    print("  PASS")
finally:
    ot.ResourceMap.SetAsScalar("HODLRMatrix-Nugget", previous_nugget25)

# === Test 26: gemv after a Cholesky factorization ===
print("\n=== Test 26: gemv after Cholesky factorization ===")
# After LLT the tree describes the Schur complements, so the matrix-vector
# product is assembled as L (L^T x); it must reproduce the dense product.
n26 = 50
vertices26 = ot.Sample([[i * 0.02] for i in range(n26)])
af26 = HODLRTestAssemblyFunction(vertices26, 0.1)
params26 = make_params(leaf=25)
params26.setMaxRank(50)
hodlr26 = build_hodlr(vertices26, af26, params=params26)
x26 = ot.Point([float(i) / n26 for i in range(n26)])
K26 = build_dense_matrix(af26, n26)
y26_dense = ot.Point(n26, 0.0)
for i in range(n26):
    s = 0.0
    for j in range(n26):
        s += K26[i, j] * x26[j]
    y26_dense[i] = s
y26 = ot.Point(n26, 0.0)
hodlr26.gemv('N', 1.0, x26, 0.0, y26)
ott.assert_almost_equal(y26, y26_dense, 1.0e-10, 1.0e-10)
print("  PASS")

# === Test 27: reject a covariance model whose input dimension does not match ===
# === the vertices dimension ===
print("\n=== Test 27: input dimension mismatch is rejected ===")
# A 1D-scale MaternModel discretized on 2D vertices used to silently assemble
# the kernel on the first coordinate only. The assembly must now fail loudly.
vertices27 = ot.Sample([[0.0, 0.0], [0.1, 0.1], [0.2, 0.2]])
cov27 = ot.MaternModel([0.1], [1.0], 2.5)
params27 = make_params(leaf=2)
with ott.assert_raises(TypeError):
    cov27.discretizeHODLRMatrix(vertices27, params27)
# A 2D model on the same 2D vertices must still work
cov27b = ot.IsotropicCovarianceModel(ot.MaternModel([0.1], [1.0], 2.5), 2)
hodlr27 = cov27b.discretizeHODLRMatrix(vertices27, params27)
hodlr27.factorize()
x27 = hodlr27.solve(ot.Point(3, 1.0))
print("  PASS")

# === Test 28: 4D locally-correlated kernel with adaptive rank (MaxRank=0) ===
print("\n=== Test 28: 4D locally-correlated kernel with adaptive rank ===")
# A locally-correlated kernel on a 4D grid has off-diagonal blocks whose
# entries lie below the assembly tolerance relative to the diagonal. With the
# adaptive-rank default (MaxRank=0) the ACA never terminated on such blocks:
# each pivot captured the same noise floor, so the block was stored at the
# full block rank and the assembly/factorization blew up (4D n=20736 took
# ~14s+23s, n=50625 >300s). Negligible blocks must now be stored as rank-0
# factors, keeping both the storage and the solve accurate.
d28 = 4
k28 = 7
span28 = float(k28 - 1)
vertices28 = ot.Sample(
    [[c_j / span28 for c_j in c]
     for c in itertools.product(range(k28), repeat=d28)]
)
n28 = vertices28.getSize()
cov28 = ot.MaternModel([0.01] * d28, [1.0], 2.5)
params28 = make_params(leaf=16)
params28.setMaxRank(0)
previous_nugget28 = ot.ResourceMap.GetAsScalar("HODLRMatrix-Nugget")
try:
    ot.ResourceMap.SetAsScalar("HODLRMatrix-Nugget", 0.0)
    hodlr28 = cov28.discretizeHODLRMatrix(vertices28, params28)
    hodlr28.factorize()
    b28 = ot.Point(n28, 1.0)
    x28 = hodlr28.solve(b28)
    y28 = ot.Point(n28, 0.0)
    hodlr28.gemv('N', 1.0, x28, 0.0, y28)
    err28 = (y28 - b28).norm() / b28.norm()
    assert err28 < 1.0e-12, f"adaptive-rank solve error {err28:.2e} should be small"
    cr28 = hodlr28.compressionRatio()
    # negligible off-diagonal blocks are stored as rank-0: far fewer entries
    # than the dense matrix are kept, unlike the full-rank storage of the bug
    assert cr28[0] < 0.1 * cr28[1], (
        f"negligible blocks must be rank-0, not stored at full rank "
        f"({cr28[0]}/{cr28[1]})"
    )
    print(f"  n= {n28}, solve err= {err28:.2e}, stored= {cr28[0]}/{cr28[1]}")
    print("  PASS")
finally:
    ot.ResourceMap.SetAsScalar("HODLRMatrix-Nugget", previous_nugget28)

# === Test 29: regularization shift reporting and warning ===
print("\n=== Test 29: regularization shift & warning ===")
prev_nugget29 = ot.ResourceMap.GetAsScalar("HODLRMatrix-Nugget")
prev_warn29 = ot.ResourceMap.GetAsScalar("HODLRMatrix-RegularizationWarnThreshold")
try:
    ot.ResourceMap.SetAsScalar("HODLRMatrix-Nugget", 0.0)
    # any positive shift must be flagged
    ot.ResourceMap.SetAsScalar("HODLRMatrix-RegularizationWarnThreshold", 0.0)
    # a well-conditioned matrix factorizes with no regularization and no warning
    ott.assert_almost_equal(ot.Point([hodlr.getRegularizationShift()]), ot.Point([0.0]))
    msg_ok29 = capture_stderr(lambda: hodlr.solve(ot.Point(20, 1.0)))
    assert "regularization shift" not in msg_ok29, "a regular factorization must not warn"
    # a singular matrix (duplicated points) forces a regularization shift
    n29 = 300
    x29 = [i / (n29 - 1.0) for i in range(n29)]
    for i in range(n29 // 3, n29):
        x29[i] = x29[i - n29 // 3]
    vertices29 = ot.Sample([[v] for v in x29])
    cov29 = ot.MaternModel([0.2], [1.0], 2.5)
    hodlr29 = cov29.discretizeHODLRMatrix(vertices29, make_params(leaf=16))
    warn29 = capture_stderr(hodlr29.factorize)
    # the smallest regularization epsilon is applied
    eps29 = ot.ResourceMap.GetAsScalar("HODLRMatrix-RegularizationEpsilon")
    ott.assert_almost_equal(ot.Point([hodlr29.getRegularizationShift()]), ot.Point([eps29]))
    assert "factorization required a regularization shift" in warn29, (
        "a significant regularization shift must emit a warning"
    )
    # the regularized factorization still yields a valid solve
    b29 = ot.Point(n29, 1.0)
    x_sol29 = hodlr29.solve(b29)
    y29 = ot.Point(n29, 0.0)
    hodlr29.gemv('N', 1.0, x_sol29, 0.0, y29)
    err29 = (y29 - b29).norm() / b29.norm()
    assert err29 < 1.0e-10, f"regularized solve error {err29:.2e} should be small"
    print(f"  shift= {hodlr29.getRegularizationShift():.2e}, warning emitted, solve ok")
    print("  PASS")
finally:
    ot.ResourceMap.SetAsScalar("HODLRMatrix-Nugget", prev_nugget29)
    ot.ResourceMap.SetAsScalar("HODLRMatrix-RegularizationWarnThreshold", prev_warn29)

print("\n=== ALL TESTS PASSED ===")
