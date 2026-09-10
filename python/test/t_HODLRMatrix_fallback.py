#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.PlatformInfo.SetNumericalPrecision(3)


class BlockAssemblyFunction:
    """Kernel whose cross-half off-diagonal block is exactly zero.

    Every entry of the off-diagonal blocks is below the matrix-scale assembly
    cutoff (assembly epsilon relative to the diagonal) in
    HODLRNode::lowRankApproxPartialPivot, so the block is stored as an empty
    rank-0 factor instead of being compressed at the capped or full rank.
    """

    def __init__(self, vertices, half, lengthScale=0.2):
        self.vertices = vertices
        self.half = half
        self.lengthScale = lengthScale

    def __call__(self, i, j):
        if (i < self.half) != (j < self.half):
            return 0.0
        return m.exp(-(self.vertices[i] - self.vertices[j]).norm() / self.lengthScale)


def build_dense_matrix(assembly_func, size):
    K = ot.Matrix(size, size)
    for i in range(size):
        for j in range(size):
            K[i, j] = assembly_func(i, j)
    return K


def dense_logdet(K):
    _, R = K.computeQR()
    return sum(m.log(abs(R[i, i])) for i in range(K.getNbRows()))


def make_params(leaf, max_rank):
    p = ot.HODLRMatrixParameters()
    p.setAssemblyEpsilon(1.0e-6)
    p.setRecompressionEpsilon(1.0e-6)
    p.setMinLeafSize(leaf)
    p.setMaxRank(max_rank)
    return p


def build_hodlr(vertices, af, params):
    factory = ot.HODLRMatrixFactory()
    hodlr = factory.build(vertices, 1, True, params)
    hodlr.assembleReal(af, 'L')
    hodlr.factorize()
    return hodlr


def dense_matvec(af, size, x):
    y = ot.Point(size, 0.0)
    for i in range(size):
        s = 0.0
        for j in range(size):
            s += af(i, j) * x[j]
        y[i] = s
    return y


# === Test 1: single-level tree, negligible block stored as rank-0 ===
# The top-level off-diagonal block is 10x10 and is numerically zero (all its
# entries lie below the matrix-scale assembly cutoff), so it must be stored as
# an empty rank-0 factor instead of being compressed at the capped maxRank.
print("=== Test 1: single-level rank-0 block ===")
n1 = 20
vertices1 = ot.Sample([[i * 0.1] for i in range(n1)])
af1 = BlockAssemblyFunction(vertices1, n1 // 2, 0.2)
params1 = make_params(leaf=n1 // 2, max_rank=2)
hodlr1 = build_hodlr(vertices1, af1, params1)
K1 = build_dense_matrix(af1, n1)

b1 = ot.Point(n1, 1.0)
ott.assert_almost_equal(hodlr1.solve(b1), K1.solveLinearSystem(b1), 1.0e-10, 1.0e-10)
print("  PASS (solve Point)")

B1 = ot.Matrix(n1, 3)
for i in range(n1):
    B1[i, 0] = 1.0
    B1[i, 1] = float(i) / n1
    B1[i, 2] = m.sin(float(i))
X_h1 = hodlr1.solve(B1)
X_d1 = K1.solveLinearSystem(B1)
for col in range(3):
    ott.assert_almost_equal(
        ot.Point([X_h1[i, col] for i in range(n1)]),
        ot.Point([X_d1[i, col] for i in range(n1)]),
        1.0e-10, 1.0e-10
    )
print("  PASS (solve Matrix)")

x1 = ot.Point([m.sin(float(i)) for i in range(n1)])
y_h1 = ot.Point(n1, 0.0)
hodlr1.gemv('N', 1.0, x1, 0.0, y_h1)
ott.assert_almost_equal(y_h1, dense_matvec(af1, n1, x1), 1.0e-10, 1.0e-10)
print("  PASS (gemv)")

ott.assert_almost_equal(
    ot.Point([hodlr1.logDeterminant()]),
    ot.Point([dense_logdet(K1)]),
    1.0e-10, 1.0e-10
)
print("  PASS (logDeterminant)")

# The negligible block must store nothing: only the two 10x10 dense leaves
# contribute, i.e. 2 * 10 * 10 = 200 entries. The 10x10 off-diagonal block
# would have added 2 * 10 * maxRank = 40 entries if it were rank-compressed.
cr1 = hodlr1.compressionRatio()
ott.assert_almost_equal(ot.Point([cr1[0]]), ot.Point([200]))
ott.assert_almost_equal(ot.Point([cr1[1]]), ot.Point([400]))
print("  PASS (compressionRatio)")

# === Test 2: fallback block coexists with a regular low-rank block ===
# Tree: [0,20) -> [0,10), [10,20) -> [0,5), [5,10). The top-level block is
# degenerate (fallback), the [5,10)x[0,5) block is a regular ACA block.
print("\n=== Test 2: fallback + regular block ===")
n2 = 20
vertices2 = ot.Sample([[i * 0.1] for i in range(n2)])
af2 = BlockAssemblyFunction(vertices2, n2 // 2, 0.2)
params2 = make_params(leaf=4, max_rank=5)
hodlr2 = build_hodlr(vertices2, af2, params2)
K2 = build_dense_matrix(af2, n2)

b2 = ot.Point(n2, 1.0)
ott.assert_almost_equal(hodlr2.solve(b2), K2.solveLinearSystem(b2), 1.0e-10, 1.0e-10)
print("  PASS (solve)")

x2 = ot.Point([m.sin(float(i)) for i in range(n2)])
y_h2 = ot.Point(n2, 0.0)
hodlr2.gemv('N', 1.0, x2, 0.0, y_h2)
ott.assert_almost_equal(y_h2, dense_matvec(af2, n2, x2), 1.0e-10, 1.0e-10)
print("  PASS (gemv)")

ott.assert_almost_equal(
    ot.Point([hodlr2.logDeterminant()]),
    ot.Point([dense_logdet(K2)]),
    1.0e-10, 1.0e-10
)
print("  PASS (logDeterminant)")

# === Test 3: fallback block with LLt factorization ===
print("\n=== Test 3: fallback with LLt factorization ===")
n3 = 10
vertices3 = ot.Sample([[i * 0.1] for i in range(n3)])
af3 = BlockAssemblyFunction(vertices3, n3 // 2, 0.2)
params3 = make_params(leaf=n3 // 2, max_rank=2)
hodlr3 = build_hodlr(vertices3, af3, params3)
K3 = build_dense_matrix(af3, n3)

b3 = ot.Point(n3, 1.0)
ott.assert_almost_equal(hodlr3.solve(b3), K3.solveLinearSystem(b3), 1.0e-10, 1.0e-10)
print("  PASS (solve)")

# === Test 4: unbounded maxRank fallback (full-rank reconstruction) ===
print("\n=== Test 4: unbounded maxRank fallback ===")
n4 = 10
vertices4 = ot.Sample([[i * 0.1] for i in range(n4)])
af4 = BlockAssemblyFunction(vertices4, n4 // 2, 0.2)
params4 = make_params(leaf=n4 // 2, max_rank=0)
hodlr4 = build_hodlr(vertices4, af4, params4)
K4 = build_dense_matrix(af4, n4)

b4 = ot.Point(n4, 1.0)
ott.assert_almost_equal(hodlr4.solve(b4), K4.solveLinearSystem(b4), 1.0e-10, 1.0e-10)
print("  PASS (solve)")

x4 = ot.Point([m.sin(float(i)) for i in range(n4)])
y_h4 = ot.Point(n4, 0.0)
hodlr4.gemv('N', 1.0, x4, 0.0, y_h4)
ott.assert_almost_equal(y_h4, dense_matvec(af4, n4, x4), 1.0e-10, 1.0e-10)
print("  PASS (gemv)")

# === Test 5: zero regularization attempts is rejected ===
# A zero value would skip the Schur complement loop entirely, leaving child1
# unfactorized and crashing later on an empty Sfactor_ matrix.
print("\n=== Test 5: HODLRMatrix-RegularizationAttempts must be positive ===")
n5 = 20
vertices5 = ot.Sample([[i * 0.1] for i in range(n5)])
af5 = BlockAssemblyFunction(vertices5, n5 // 2, 0.2)
params5 = make_params(leaf=4, max_rank=0)
ot.ResourceMap.SetAsUnsignedInteger("HODLRMatrix-RegularizationAttempts", 0)
hodlr5 = ot.HODLRMatrixFactory().build(vertices5, 1, True, params5)
hodlr5.assembleReal(af5, 'L')
with ott.assert_raises(TypeError):
    hodlr5.factorize()
ot.ResourceMap.SetAsUnsignedInteger("HODLRMatrix-RegularizationAttempts", 60)
print("  PASS")

# === Test 6: singular blocks raise through the factorization ===
# A singular leaf/coupling block must fail loudly (InternalException from the
# LAPACK info check) instead of storing -inf in logDeterminant() and letting
# solve() return garbage. This also makes the outer regularization loop
# reachable.
print("\n=== Test 6: singular block raises in the factorization ===")
n6 = 8
vertices6 = ot.Sample([[i * 0.1] for i in range(n6)])


def zero_assembly(i, j):
    return 0.0


params6 = make_params(leaf=2, max_rank=2)
ot.ResourceMap.SetAsUnsignedInteger("HODLRMatrix-FactorizationIterations", 1)
hodlr6 = ot.HODLRMatrixFactory().build(vertices6, 1, True, params6)
hodlr6.assembleReal(zero_assembly, 'L')
with ott.assert_raises(RuntimeError):
    hodlr6.factorize()
ot.ResourceMap.SetAsUnsignedInteger("HODLRMatrix-FactorizationIterations", 20)
print("  PASS")

# === Test 7: symmetry mismatch between build() and assemble() is rejected ===
# The symmetric flag given at build() time is stored on the implementation and
# validated against the symmetry char of the later assemble() call, so a
# mismatch raises instead of being silently ignored.
print("\n=== Test 7: build()/assemble() symmetry mismatch is rejected ===")
n7 = 8
vertices7 = ot.Sample([[i * 0.1] for i in range(n7)])
af7 = BlockAssemblyFunction(vertices7, n7 // 2, 0.2)
params7 = make_params(leaf=4, max_rank=2)

hodlr7 = ot.HODLRMatrixFactory().build(vertices7, 1, True, params7)
with ott.assert_raises(TypeError):
    hodlr7.assembleReal(af7, 'N')

hodlr8 = ot.HODLRMatrixFactory().build(vertices7, 1, False, params7)
with ott.assert_raises(TypeError):
    hodlr8.assembleReal(af7, 'L')

hodlr9 = ot.HODLRMatrixFactory().build(vertices7, 1, True, params7)
with ott.assert_raises(TypeError):
    hodlr9.assembleReal(af7, 'X')

hodlr10 = ot.HODLRMatrixFactory().build(vertices7, 1, True, params7)
hodlr10.assembleReal(af7, 'L')
hodlr10.factorize()
print("  PASS")

# === Test 8: copies drop the tree but must report their true condition ===
# Copy operations drop the HODLR tree but keep the matrix data (diagonal,
# shiftAccumulated_). They must reset the derived factorization state
# (isFactorized_, isCholesky_, logDet_) so the copy reports "not assembled"
# for factor-dependent operations instead of silently returning stale results,
# while data queries such as getDiagonal still work.
print("\n=== Test 8: a copied HODLRMatrix reports its true condition ===")
n8 = 8
vertices8 = ot.Sample([[i * 0.1] for i in range(n8)])
af8 = BlockAssemblyFunction(vertices8, n8 // 2, 0.2)
params8 = make_params(leaf=4, max_rank=2)
hodlr8 = build_hodlr(vertices8, af8, params8)

b8 = ot.Point(n8, 1.0)
ott.assert_almost_equal(hodlr8.solve(b8), build_dense_matrix(af8, n8).solveLinearSystem(b8), 1.0e-10, 1.0e-10)

copy8 = hodlr8.copy()
ott.assert_almost_equal(copy8.getNbRows(), n8)
ott.assert_almost_equal(copy8.getDiagonal(), hodlr8.getDiagonal())
with ott.assert_raises(TypeError):
    copy8.logDeterminant()
with ott.assert_raises(TypeError):
    copy8.solve(b8)
with ott.assert_raises(TypeError):
    copy8.gemv('N', 1.0, b8, 0.0, ot.Point(n8))
with ott.assert_raises(TypeError):
    copy8.factorize()

copy8.assembleReal(af8, 'L')
copy8.factorize()
ott.assert_almost_equal(copy8.solve(b8), build_dense_matrix(af8, n8).solveLinearSystem(b8), 1.0e-10, 1.0e-10)
ott.assert_almost_equal(hodlr8.solve(b8), build_dense_matrix(af8, n8).solveLinearSystem(b8), 1.0e-10, 1.0e-10)
print("  PASS")

# === Test 9: zero factorization iterations is rejected ===
# A zero value would skip the regularization loop entirely, leave the tree
# unfactorized and yet mark isFactorized_ = true, so solve() would use empty
# factors. It must be rejected at factorize() time.
print("\n=== Test 9: HODLRMatrix-FactorizationIterations must be positive ===")
n9 = 8
vertices9 = ot.Sample([[i * 0.1] for i in range(n9)])
af9 = BlockAssemblyFunction(vertices9, n9 // 2, 0.2)
params9 = make_params(leaf=4, max_rank=2)
ot.ResourceMap.SetAsUnsignedInteger("HODLRMatrix-FactorizationIterations", 0)
hodlr9 = ot.HODLRMatrixFactory().build(vertices9, 1, True, params9)
hodlr9.assembleReal(af9, 'L')
with ott.assert_raises(TypeError):
    hodlr9.factorize()
ot.ResourceMap.SetAsUnsignedInteger("HODLRMatrix-FactorizationIterations", 20)
print("  PASS")

print("\n=== ALL TESTS PASSED ===")
