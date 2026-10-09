#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math

ot.TESTPREAMBLE()


def check_orthonormality(factory, degree_max=6):
    m = degree_max + 1
    n, w = factory.getNodesAndWeights(m)
    for i in range(degree_max + 1):
        pI = factory.build(i)
        for j in range(i + 1):
            pJ = factory.build(j)
            val = sum(w[k] * pI(n[k]) * pJ(n[k]) for k in range(m))
            expected = 1.0 if i == j else 0.0
            ott.assert_almost_equal(val, expected, 1e-12)


def check_coefficients(factory, x_values):
    for i in range(10):
        p = factory.build(i)
        coeffs = p.getCoefficients()
        for x in x_values:
            y_coeff = sum(c * (x**j) for j, c in enumerate(coeffs))
            y_op = p(x)
            ott.assert_almost_equal(y_coeff, y_op)


# Standard 2-arg factory
jacobi = ot.JacobiFactory(2.5, 3.5)
check_orthonormality(jacobi)
check_coefficients(jacobi, [0.0, 0.5, -0.5])

# Bounded 4-arg factory
# Width 3 (not 2) so (bs-as)/(bi-ai)=2/3 vs inverse 3/2 is visible
alpha = 2.5
beta = 3.5
a = -1.0
b = 2.0
jacobi_bounded = ot.JacobiFactory(alpha, beta, a, b)
check_orthonormality(jacobi_bounded)
check_coefficients(jacobi_bounded, [a, (a + b) * 0.5, b])
# Affine map from [-1, 2] to standard [-1, 1]: a_=2/3, b_=-1/3
ott.assert_almost_equal(jacobi_bounded.getA(), 2.0 / 3.0)
ott.assert_almost_equal(jacobi_bounded.getB(), -1.0 / 3.0)

# Edge case: Jacobi exponents summing to -1, i.e. Beta shape parameters
# summing to 1. Beta(0.5, 0.5) maps to exponents (-0.5, -0.5), which uses
# the canceled recurrence form; the 2-point rule is +-1/sqrt(2), weights 0.5
jacobi_chebyshev = ot.JacobiFactory(0.5, 0.5)
nodes, weights = jacobi_chebyshev.getNodesAndWeights(2)
ref = 1.0 / math.sqrt(2.0)
ott.assert_almost_equal(nodes[0], -ref, 1e-12)
ott.assert_almost_equal(nodes[1], ref, 1e-12)
ott.assert_almost_equal(weights[0], 0.5, 1e-12)
ott.assert_almost_equal(weights[1], 0.5, 1e-12)

# Asymptotic Hale-Townsend path, including the small-block guard:
# force the asymptotic path at n=21 so the interior half-blocks are at the
# JacobiBoundaryNodes limit, then check a large rule on the default path
defaultThreshold = ot.ResourceMap.GetAsUnsignedInteger("FastJacobi-AsymptoticThreshold")
ot.ResourceMap.SetAsUnsignedInteger("FastJacobi-AsymptoticThreshold", 21)
try:
    nodes, weights = ot.JacobiFactory(2.5, 3.5).getNodesAndWeights(21)
    assert len(nodes) == 21
    for k in range(21):
        assert weights[k] >= 0.0
        if k > 0:
            assert nodes[k] > nodes[k - 1]
    ott.assert_almost_equal(sum(weights), 1.0, 1e-12)
finally:
    ot.ResourceMap.SetAsUnsignedInteger("FastJacobi-AsymptoticThreshold", defaultThreshold)
nodes, weights = ot.JacobiFactory(2.5, 3.5).getNodesAndWeights(150)
assert len(nodes) == 150
for k in range(150):
    assert weights[k] >= 0.0
    if k > 0:
        assert nodes[k] > nodes[k - 1]
ott.assert_almost_equal(sum(weights), 1.0, 1e-12)
