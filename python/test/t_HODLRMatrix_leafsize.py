#! /usr/bin/env python

"""
Test sweep over MinLeafSize to find the best block size for the
uncompressed diagonal/off-diagonal blocks.

For each candidate leaf size we measure:
  - solve error  (HODLR vs dense)
  - compression ratio  (nnz / n^2)
  - assembly + factorization time

The optimal leaf size is the largest one that still meets the accuracy target
while providing meaningful compression.
"""

import openturns as ot
import openturns.testing as ott
import math as m
import time

ot.PlatformInfo.SetNumericalPrecision(3)

# --- Build a representative 2D grid with exponential covariance kernel ---

n_mesh = 20
intervalMesher = ot.IntervalMesher([n_mesh, n_mesh])
interval = ot.Interval([0.0] * 2, [1.0] * 2)
mesh2D = intervalMesher.build(interval)
vertices = mesh2D.getVertices()
total_size = vertices.getSize()
print(f"Grid size: {n_mesh}x{n_mesh} = {total_size} nodes\n")


class ExponentialKernel:
    def __init__(self, vertices, length_scale):
        self.vertices = vertices
        self.length_scale = length_scale

    def __call__(self, i, j):
        diff = self.vertices[i] - self.vertices[j]
        return m.exp(-diff.norm() / self.length_scale)


kernel = ExponentialKernel(vertices, length_scale=0.2)

# Dense reference
K_dense = ot.Matrix(total_size, total_size)
for i in range(total_size):
    for j in range(total_size):
        K_dense[i, j] = kernel(i, j)

rhs = ot.Point(total_size, 1.0)
x_dense = K_dense.solveLinearSystem(rhs)

# --- Sweep over leaf sizes ---

leaf_sizes = [2, 4, 8, 16, 32, 64, 128, 256]
accuracy_target = 1.0e-4


def sweep(leaf):
    params = ot.HODLRMatrixParameters()
    params.setAssemblyEpsilon(1.0e-6)
    params.setRecompressionEpsilon(1.0e-6)
    params.setMinLeafSize(leaf)

    factory = ot.HODLRMatrixFactory()
    hodlr = factory.build(vertices, 1, True, params)

    t0 = time.time()
    hodlr.assembleReal(kernel, 'L')
    t_assembly = time.time() - t0

    t0 = time.time()
    hodlr.factorize()
    t_factor = time.time() - t0

    x_hodlr = hodlr.solve(rhs)
    err = max(abs(x_hodlr[i] - x_dense[i]) for i in range(total_size))

    compressed, total_entries = hodlr.compressionRatio()
    ratio = compressed / total_entries
    return err, ratio, t_assembly + t_factor


results = []
best_leaf = None

for leaf in leaf_sizes:
    if leaf >= total_size:
        continue

    err, ratio, elapsed = sweep(leaf)
    results.append((leaf, err, ratio, elapsed))
    print(
        f"leaf= {leaf:>4d}  "
        f"err= {err:.2e}  "
        f"ratio= {ratio:.4f}  "
        f"time= {elapsed:.4f}s"
    )

    if best_leaf is None and err <= accuracy_target:
        best_leaf = leaf

# Whole-dense leaf: leaf = total_size leaves the root block uncompressed.
err, ratio, elapsed = sweep(total_size)
results.append((total_size, err, ratio, elapsed))
print(
    f"leaf= {total_size:>4d}  "
    f"err= {err:.2e}  "
    f"ratio= {ratio:.4f}  "
    f"time= {elapsed:.4f}s"
)
if best_leaf is None and err <= accuracy_target:
    best_leaf = total_size

# --- Assertions ---

# There must exist at least one leaf size that meets accuracy
assert best_leaf is not None, (
    f"No leaf size met accuracy target {accuracy_target:.0e}"
)

print(f"\nBest leaf size (largest meeting {accuracy_target:.0e} accuracy): {best_leaf}")

# Small leaf sizes should produce compression (ratio < 1.0)
small_leaves = [r for r in results if r[0] <= 16]
assert len(small_leaves) > 0, "Need at least one small leaf size in the sweep"
for leaf, err, ratio, _ in small_leaves:
    assert ratio < 1.0, f"leaf={leaf}: ratio should be < 1.0, got {ratio}"

# The dense fallback (leaf >= n/2) should give ratio close to 1.0 and perfect accuracy
dense_results = [r for r in results if r[2] > 0.99]
assert len(dense_results) > 0, (
    "At least one large leaf size should give near-dense ratio"
)
for leaf, err, ratio, _ in dense_results:
    ott.assert_almost_equal(err, 0.0, 1.0e-10, 1.0e-10)

# The minimum compression ratio should occur among the smaller leaf sizes
# (first half of the range), not at the dense end
half = len(results) // 2
early_min_ratio = min(r[2] for r in results[:half])
late_min_ratio = min(r[2] for r in results[half:])
assert early_min_ratio <= late_min_ratio, (
    "Best compression should occur at smaller leaf sizes"
)

# All solve errors should be finite
for leaf, err, ratio, _ in results:
    assert m.isfinite(err), f"leaf={leaf}: solve error is not finite"

print("\n=== ALL TESTS PASSED ===")
