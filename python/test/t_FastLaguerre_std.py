#! /usr/bin/env python

import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Invalid arguments
with ott.assert_raises(TypeError):
    otexp.FastLaguerre(0, 1.0)
with ott.assert_raises(TypeError):
    otexp.FastLaguerre(4, 0.0)

# n=1: single node at k, weight one
nodes, weights = otexp.FastLaguerre(1, 2.5)
ott.assert_almost_equal(nodes, [2.5], 1.0e-12, 1.0e-12)
ott.assert_almost_equal(weights, [1.0], 1.0e-12, 1.0e-12)

# Cross-check against LaguerreFactory (Golub-Welsch reference): the default
# parameterization associates Gamma(k+1, 1, 0) with shape parameter k, i.e.
# FastLaguerre(n, k) matches LaguerreFactory(k - 1)
for k in [0.5, 2.0, 4.5]:
    refFactory = ot.LaguerreFactory(k - 1.0)
    for n in [3, 10, 40]:
        nodes, weights = otexp.FastLaguerre(n, k)
        refNodes, refWeights = refFactory.getNodesAndWeights(n)
        ott.assert_almost_equal(nodes, refNodes, 1.0e-9, 1.0e-12,
                                f"k={k} n={n} nodes")
        ott.assert_almost_equal(weights, refWeights, 1.0e-9, 1.0e-12,
                                f"k={k} n={n} weights")

# Exactness check: the n-point rule integrates polynomials of degree up to
# 2n-1 exactly w.r.t. Gamma(k, 1), whose m-th raw moment is Gamma(k+m)/Gamma(k)
for k in [1.0, 2.5]:
    for n in [3, 6]:
        nodes, weights = otexp.FastLaguerre(n, k)
        for m in range(2 * n):
            integral = sum((w * x**m for w, x in zip(weights, nodes)))
            ref = 1.0
            for j in range(m):
                ref *= k + j
            ott.assert_almost_equal(integral, ref, 1.0e-8, 1.0e-10,
                                    f"k={k} n={n} degree {m}")

# Structural properties: positive weights summing to one, strictly increasing
# nodes within [0, inf)
n = 16
k = 2.0
nodes, weights = otexp.FastLaguerre(n, k)
assert all(w > 0.0 for w in weights)
assert all(x >= 0.0 for x in nodes)
assert all(x0 < x1 for x0, x1 in zip(nodes, nodes[1:]))
ott.assert_almost_equal(sum(weights), 1.0, 1.0e-10, 1.0e-10)

# Large-n sanity check
n = 1024
k = 0.5
nodes, weights = otexp.FastLaguerre(n, k)
assert all(x >= 0.0 and math.isfinite(x) for x in nodes)
assert all(w >= 0.0 and math.isfinite(w) for w in weights)
assert all(x0 < x1 for x0, x1 in zip(nodes, nodes[1:]))
ott.assert_almost_equal(sum(weights), 1.0, 1.0e-9, 1.0e-9)
