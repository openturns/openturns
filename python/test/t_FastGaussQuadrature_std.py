#! /usr/bin/env python

import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Invalid arguments
with ott.assert_raises(TypeError):
    otexp.FastGaussQuadrature(ot.Point(0), ot.Point(0))
with ott.assert_raises(TypeError):
    otexp.FastGaussQuadrature(ot.Point(2), ot.Point(1))

# n=1: Gamma matrix with a single diagonal entry
nodes, weights = otexp.FastGaussQuadrature([3.0], [0.0])
ott.assert_almost_equal(nodes, [3.0], 1.0e-12, 1.0e-12)
ott.assert_almost_equal(weights, [1.0], 1.0e-12, 1.0e-12)

# n=2 Hermite matrix: nodes at +/-1, weights 1/2
nodes, weights = otexp.FastGaussQuadrature([0.0, 0.0], [0.0, 1.0])
ott.assert_almost_equal(nodes, [-1.0, 1.0], 1.0e-12, 1.0e-12)
ott.assert_almost_equal(weights, [0.5, 0.5], 1.0e-12, 1.0e-12)

# n=3 Hermite matrix: nodes at -sqrt(3), 0, sqrt(3), weights 1/6, 2/3, 1/6
nodes, weights = otexp.FastGaussQuadrature([0.0, 0.0, 0.0],
                                           [0.0, 1.0, math.sqrt(2.0)])
ott.assert_almost_equal(nodes, [-math.sqrt(3.0), 0.0, math.sqrt(3.0)],
                        1.0e-12, 1.0e-12)
ott.assert_almost_equal(weights, [1.0 / 6.0, 2.0 / 3.0, 1.0 / 6.0],
                        1.0e-12, 1.0e-12)

# n=4 Legendre matrix: the rule integrates polynomials of degree up to 7
# w.r.t. the uniform probability measure on [-1, 1]
n = 4
gamma = [0.0] * n
b = [0.0] + [j / math.sqrt(4.0 * j * j - 1.0) for j in range(1, n)]
nodes, weights = otexp.FastGaussQuadrature(gamma, b)
assert all(w > 0.0 for w in weights)
assert all(x0 < x1 for x0, x1 in zip(nodes, nodes[1:]))
assert nodes[0] >= -1.0 and nodes[-1] <= 1.0
ott.assert_almost_equal(sum(weights), 1.0, 1.0e-12, 1.0e-12)
for m in range(2 * n):
    integral = sum((w * x**m for w, x in zip(weights, nodes)))
    ref = 1.0 / (m + 1.0) if m % 2 == 0 else 0.0
    ott.assert_almost_equal(integral, ref, 1.0e-12, 1.0e-12, f"degree {m}")
