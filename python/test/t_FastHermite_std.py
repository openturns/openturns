#! /usr/bin/env python

import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Invalid argument: n == 0
with ott.assert_raises(TypeError):
    otexp.FastHermite(0)

# n=1: single node at 0, weight one
nodes, weights = otexp.FastHermite(1)
ott.assert_almost_equal(nodes, [0.0])
ott.assert_almost_equal(weights, [1.0], 1.0e-5, 1.0e-8, "n=1 weight")

# Cross-check against HermiteFactory (Golub-Welsch reference)
refFactory = ot.HermiteFactory()
for n in [4, 12, 40, 1024]:
    nodes, weights = otexp.FastHermite(n)
    refNodes, refWeights = refFactory.getNodesAndWeights(n)
    ott.assert_almost_equal(nodes, refNodes, 1.0e-10, 1.0e-12, f"n={n} nodes")
    ott.assert_almost_equal(weights, refWeights, 1.0e-10, 1.0e-12,
                            f"n={n} weights")

# Exactness check: the n-point rule integrates polynomials of degree up to
# 2n-1 exactly w.r.t. N(0, 1)
for n in [2, 4, 8]:
    nodes, weights = otexp.FastHermite(n)
    for m in range(2 * n):
        integral = sum((w * x**m for w, x in zip(weights, nodes)))
        if m % 2 == 1:
            ref = 0.0
        else:
            ref = 1.0
            for j in range(1, m, 2):
                ref *= j
        ott.assert_almost_equal(integral, ref, 1.0e-7, 1.0e-9,
                                f"n={n} degree {m}")

# Structural properties: positivity, symmetry
n = 16
nodes, weights = otexp.FastHermite(n)
assert all(w > 0.0 for w in weights)
for i in range(n):
    ott.assert_almost_equal(nodes[i] + nodes[n - 1 - i], 0.0, 1.0e-10, 1.0e-12)
    ott.assert_almost_equal(weights[i], weights[n - 1 - i], 1.0e-10, 1.0e-12)

# Large-n sanity check: weights non-negative and summing to 1, nodes finite
# and ascending
n = 1024
nodes, weights = otexp.FastHermite(n)
assert all(w >= 0.0 and math.isfinite(w) for w in weights)
assert all(math.isfinite(x) for x in nodes)
assert all(x0 < x1 for x0, x1 in zip(nodes, nodes[1:]))
ott.assert_almost_equal(sum(weights), 1.0, 1.0e-9, 1.0e-9)
