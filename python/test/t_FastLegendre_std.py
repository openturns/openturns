#! /usr/bin/env python

import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Invalid argument: n == 0
with ott.assert_raises(TypeError):
    otexp.FastLegendre(0)

# n=1: single node at 0, weight 2
nodes, weights = otexp.FastLegendre(1)
ott.assert_almost_equal(nodes, [0.0], 1.0e-12, 1.0e-12)
ott.assert_almost_equal(weights, [2.0], 1.0e-12, 1.0e-12)

# Cross-check against LegendreFactory (Golub-Welsch reference): the measure is
# the uniform distribution on [-1, 1]; LegendreFactory normalizes the weights
# to sum to 1 while the FastLegendre weights sum to 2, hence the factor 2
refFactory = ot.LegendreFactory()
for n in [3, 8, 20, 100]:
    nodes, weights = otexp.FastLegendre(n)
    refNodes, refWeights = refFactory.getNodesAndWeights(n)
    ott.assert_almost_equal(nodes, refNodes, 1.0e-10, 1.0e-12, f"n={n} nodes")
    ott.assert_almost_equal(weights, [2.0 * w for w in refWeights], 1.0e-10,
                            1.0e-12, f"n={n} weights")

# Exactness check: the n-point rule integrates polynomials of degree up to
# 2n-1 exactly w.r.t. the uniform probability measure on [-1, 1]
for n in [2, 5]:
    nodes, weights = otexp.FastLegendre(n)
    for m in range(2 * n):
        integral = sum((w * x**m for w, x in zip(weights, nodes)))
        ref = 2.0 / (m + 1.0) if m % 2 == 0 else 0.0
        ott.assert_almost_equal(integral, ref, 1.0e-10, 1.0e-12,
                                f"n={n} degree {m}")

# Structural properties: symmetry, positive weights summing to 2
n = 16
nodes, weights = otexp.FastLegendre(n)
assert all(w > 0.0 for w in weights)
for i in range(n):
    ott.assert_almost_equal(nodes[i] + nodes[n - 1 - i], 0.0, 1.0e-10, 1.0e-12)
    ott.assert_almost_equal(weights[i], weights[n - 1 - i], 1.0e-10, 1.0e-12)
assert all(x0 < x1 for x0, x1 in zip(nodes, nodes[1:]))
ott.assert_almost_equal(sum(weights), 2.0, 1.0e-10, 1.0e-10)

# Large-n sanity check
n = 4096
nodes, weights = otexp.FastLegendre(n)
assert all(x > -1.0 and x < 1.0 and math.isfinite(x) for x in nodes)
assert all(w > 0.0 and math.isfinite(w) for w in weights)
assert all(x0 < x1 for x0, x1 in zip(nodes, nodes[1:]))
ott.assert_almost_equal(sum(weights), 2.0, 1.0e-8, 1.0e-8)
