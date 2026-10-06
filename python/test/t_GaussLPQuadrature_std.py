#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import openturns.experimental as otexp

ot.TESTPREAMBLE()

# This test requires the HiGHS LP solver (registered under highs_FOUND in CMakeLists)
dim = 2
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * dim)

# Build initial basis: tensorized Hermite polynomials
refBasis = ot.OrthogonalProductPolynomialFactory([ot.HermiteFactory()] * dim)
initialBasis = [refBasis.build(i) for i in range(6)]

factory = otexp.FiniteOrthonormalFunctionFactory(initialBasis, distribution)
functions = [factory.build(k) for k in range(len(initialBasis))]

# Minimal quadrature rule exactly integrating the first n orthonormal functions
# Only stable properties are checked: the node count varies across platforms
n = 3
nodes, weights = factory.buildQuadrature(n)
assert len(nodes) > 0
assert len(weights) == len(nodes)
ott.assert_almost_equal(sum(weights), 1.0, 1e-3, 1e-3)

for k in range(n):
    integral = sum(weights[i] * functions[k](nodes[i])[0] for i in range(len(nodes)))
    if k == 0:
        ott.assert_almost_equal(integral, 1.0, 1e-3, 1e-3)
    else:
        ott.assert_almost_equal(integral, 0.0, 1e-3, 1e-3)

# Same check through the GaussLPQuadrature class directly
quad = otexp.GaussLPQuadrature(functions, distribution)
nodes2, weights2 = quad.build(n)
assert len(nodes2) > 0
assert len(weights2) == len(nodes2)
ott.assert_almost_equal(sum(weights2), 1.0, 1e-3, 1e-3)

for k in range(n):
    integral = sum(weights2[i] * functions[k](nodes2[i])[0] for i in range(len(nodes2)))
    if k == 0:
        ott.assert_almost_equal(integral, 1.0, 1e-3, 1e-3)
    else:
        ott.assert_almost_equal(integral, 0.0, 1e-3, 1e-3)

# Non-unit support volume: moments must include the Jacobian (volume) factor
dist1d = ot.Uniform(2.0, 4.0)
refBasis1d = ot.OrthogonalProductPolynomialFactory([ot.LegendreFactory()])
initialBasis1d = [refBasis1d.build(i) for i in range(4)]
factory1d = otexp.FiniteOrthonormalFunctionFactory(initialBasis1d, dist1d)
functions1d = [factory1d.build(k) for k in range(len(initialBasis1d))]
n1d = 2
nodes1d, weights1d = factory1d.buildQuadrature(n1d)
assert len(nodes1d) > 0
assert len(weights1d) == len(nodes1d)
ott.assert_almost_equal(sum(weights1d), 1.0, 1e-3, 1e-3)
for k in range(n1d):
    integral = sum(weights1d[i] * functions1d[k](nodes1d[i])[0] for i in range(len(nodes1d)))
    if k == 0:
        ott.assert_almost_equal(integral, 1.0, 1e-3, 1e-3)
    else:
        ott.assert_almost_equal(integral, 0.0, 1e-3, 1e-3)

# LP solver fallback: an invalid primary solver fails over to the fallback solver
ot.ResourceMap.SetAsString('HiGHS-solver', 'nonsense')
nodesFb, weightsFb = factory1d.buildQuadrature(n1d)
ott.assert_almost_equal(sum(weightsFb), 1.0, 1e-3, 1e-3)
assert ot.ResourceMap.GetAsString('HiGHS-solver') == 'nonsense'
# An empty fallback disables the retry and surfaces the original error
ot.ResourceMap.SetAsString('GaussLPQuadrature-FallbackSolver', '')
with ott.assert_raises(TypeError):
    factory1d.buildQuadrature(n1d)
# Identical primary and fallback solvers: no pointless retry, error surfaces
ot.ResourceMap.SetAsString('GaussLPQuadrature-FallbackSolver', 'nonsense')
with ott.assert_raises(TypeError):
    factory1d.buildQuadrature(n1d)
ot.ResourceMap.SetAsString('HiGHS-solver', 'choose')
ot.ResourceMap.SetAsString('GaussLPQuadrature-FallbackSolver', 'ipm')
