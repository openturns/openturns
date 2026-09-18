#! /usr/bin/env python

import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Invalid arguments
with ott.assert_raises(TypeError):
    otexp.FastJacobi(0, 0.0, 0.0)
with ott.assert_raises(TypeError):
    otexp.FastJacobi(4, -1.0, 0.0)
with ott.assert_raises(TypeError):
    otexp.FastJacobi(4, 0.0, -1.5)

# n=1: single node at (beta-alpha)/(alpha+beta+2), weight one
alpha, beta = 0.5, 1.5
nodes, weights = otexp.FastJacobi(1, alpha, beta)
ott.assert_almost_equal(nodes, [0.25], 1.0e-12, 1.0e-12)
ott.assert_almost_equal(weights, [1.0], 1.0e-12, 1.0e-12)

# n=1 edge case alpha + beta == -1
nodes, weights = otexp.FastJacobi(1, -0.5, -0.5)
ott.assert_almost_equal(nodes, [0.0], 1.0e-12, 1.0e-12)
ott.assert_almost_equal(weights, [1.0], 1.0e-12, 1.0e-12)

# Cross-check against JacobiFactory (Golub-Welsch reference): both use the
# weight (1-x)^alpha (1+x)^beta, (alpha, beta) = (-0.5, -0.5) is the
# alpha + beta == -1 edge case
for alpha, beta in [(0.0, 0.0), (0.5, 1.5), (2.0, 0.5), (-0.5, -0.5)]:
    refFactory = ot.JacobiFactory(alpha, beta)
    for n in [4, 10, 30]:
        nodes, weights = otexp.FastJacobi(n, alpha, beta)
        refNodes, refWeights = refFactory.getNodesAndWeights(n)
        ott.assert_almost_equal(nodes, refNodes, 1.0e-9, 1.0e-12,
                                f"a={alpha} b={beta} n={n} nodes")
        ott.assert_almost_equal(weights, refWeights, 1.0e-9, 1.0e-12,
                                f"a={alpha} b={beta} n={n} weights")

# Structural properties: nodes in [-1, 1] and strictly increasing,
# positive weights summing to one
n = 16
alpha, beta = 2.0, 0.5
nodes, weights = otexp.FastJacobi(n, alpha, beta)
assert all(w > 0.0 for w in weights)
assert all(-1.0 < x < 1.0 for x in nodes)
assert all(x0 < x1 for x0, x1 in zip(nodes, nodes[1:]))
ott.assert_almost_equal(sum(weights), 1.0, 1.0e-10, 1.0e-10)

# Large-n sanity check
n = 1024
alpha, beta = 0.5, 1.5
nodes, weights = otexp.FastJacobi(n, alpha, beta)
assert all(x > -1.0 and x < 1.0 and math.isfinite(x) for x in nodes)
assert all(math.isfinite(w) for w in weights)
ott.assert_almost_equal(sum(weights), 1.0, 1.0e-9, 1.0e-9)
