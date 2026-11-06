#! /usr/bin/env python

import math
import os

import openturns as ot
import openturns.testing as ott
import openturns.experimental as otexp

ot.TESTPREAMBLE()


def makeBasis(functions, measure):
    """Expose a collection of functions and its measure as an OrthogonalBasis."""
    return ot.OrthogonalBasis(
        otexp.FiniteOrthogonalFunctionFactory(functions, measure)
    )


# The production defaults target high accuracy. The bases used here are of low
# degree, so 32 nodes per marginal and a 32-node moment rule are enough while
# keeping the test fast; the previous values are restored at the end.
discretizationKey = "BasisOrthonormalizationAlgorithm-DefaultDiscretization"
ngaussKey = "GaussLPQuadrature-Ngauss"
savedDiscretization = ot.ResourceMap.GetAsUnsignedInteger(discretizationKey)
savedNgauss = ot.ResourceMap.GetAsUnsignedInteger(ngaussKey)
ot.ResourceMap.SetAsUnsignedInteger(discretizationKey, 32)
ot.ResourceMap.SetAsUnsignedInteger(ngaussKey, 32)

# This test requires the HiGHS LP solver (registered under highs_FOUND in CMakeLists)
dim = 2
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * dim)

# Build initial basis: tensorized Hermite polynomials
refBasis = ot.OrthogonalProductPolynomialFactory([ot.HermiteFactory()] * dim)
initialBasis = [refBasis.build(i) for i in range(6)]

orthogonalBasis = makeBasis(initialBasis, distribution)
factory = otexp.BasisOrthonormalizationFactory(orthogonalBasis, len(initialBasis))
functions = [factory.build(k) for k in range(len(initialBasis))]

# The quadrature order must be positive and must not exceed the orthonormal set
with ott.assert_raises(TypeError):
    factory.buildQuadratureRule(0)
with ott.assert_raises(TypeError):
    factory.buildQuadratureRule(len(initialBasis) + 1)
with ott.assert_raises(TypeError):
    factory.buildQuadrature(0)
with ott.assert_raises(TypeError):
    factory.buildQuadrature(len(initialBasis) + 1)

# The rule integrates the first n orthonormal functions exactly. The number of
# nodes is an output of the algorithm: it is at most n and is not asserted to
# be exactly n.
n = 3
nodes, weights = factory.buildQuadrature(n)
assert 0 < len(nodes) <= n
assert len(weights) == len(nodes)
assert all(value >= 0.0 for value in weights)
ott.assert_almost_equal(sum(weights), 1.0, 1e-3, 1e-3)

for k in range(n):
    integral = sum(weights[i] * functions[k](nodes[i])[0] for i in range(len(nodes)))
    if k == 0:
        ott.assert_almost_equal(integral, 1.0, 1e-3, 1e-3)
    else:
        ott.assert_almost_equal(integral, 0.0, 1e-3, 1e-3)

# Same check through the GaussLPQuadrature builder directly: the functions
# whose moments are reproduced and the integration measure.
quad = otexp.GaussLPQuadrature(functions, distribution)
assert quad.getSize() == len(functions)
assert len(quad.getFunctionsCollection()) == len(functions)
assert quad.getMeasure().getDimension() == dim
assert quad.getAnchors().getSize() == 0
nodes2, weights2 = quad.build(n)
assert 0 < len(nodes2) <= n
assert len(weights2) == len(nodes2)
assert all(value >= 0.0 for value in weights2)
ott.assert_almost_equal(sum(weights2), 1.0, 1e-3, 1e-3)

for k in range(n):
    integral = sum(weights2[i] * functions[k](nodes2[i])[0] for i in range(len(nodes2)))
    if k == 0:
        ott.assert_almost_equal(integral, 1.0, 1e-3, 1e-3)
    else:
        ott.assert_almost_equal(integral, 0.0, 1e-3, 1e-3)

# The functions do not need to be orthogonal with respect to the measure:
# reproduce the raw moments of the monomials 1, x, x^2 against a uniform
# measure.
rawMeasure = ot.Uniform(2.0, 4.0)
rawFunctions = [
    ot.SymbolicFunction("x", "1"),
    ot.SymbolicFunction("x", "x"),
    ot.SymbolicFunction("x", "x^2"),
]
rawQuad = otexp.GaussLPQuadrature(rawFunctions, rawMeasure)
rawNodes, rawWeights = rawQuad.build(3)
assert 0 < len(rawNodes) <= 3
assert len(rawWeights) == len(rawNodes)
assert all(value >= 0.0 for value in rawWeights)
ott.assert_almost_equal(sum(rawWeights), 1.0, 1e-3, 1e-3)
for k, expected in enumerate([1.0, 3.0, 28.0 / 3.0]):
    integral = sum(
        rawWeights[i] * rawNodes[i][0] ** k for i in range(len(rawNodes))
    )
    ott.assert_almost_equal(integral, expected, 1e-3, 1e-3)


def checkMoments(ruleNodes, ruleWeights, tolerance=1e-3):
    """Check the first three monomial moments on the uniform measure [2, 4]."""
    ott.assert_almost_equal(sum(ruleWeights), 1.0, tolerance, tolerance)
    for k, expected in enumerate([1.0, 3.0, 28.0 / 3.0]):
        integral = sum(
            ruleWeights[i] * ruleNodes[i][0] ** k for i in range(len(ruleNodes))
        )
        ott.assert_almost_equal(integral, expected, tolerance, tolerance)


# Anchors: the three-point Gauss-Legendre rule of the measure is a set of
# anchors reproducing the moments on its own, so no node is added and the
# returned nodes are a subset of the anchors.
gaussAnchors = ot.Sample(
    [
        [3.0 - math.sqrt(3.0 / 5.0)],
        [3.0],
        [3.0 + math.sqrt(3.0 / 5.0)],
    ]
)
anchorOnlyQuad = otexp.GaussLPQuadrature(rawFunctions, rawMeasure, gaussAnchors)
assert anchorOnlyQuad.getAnchors().getSize() == 3
anchorOnlyNodes, anchorOnlyWeights = anchorOnlyQuad.build(3)
assert 0 < len(anchorOnlyNodes) <= 3
assert len(anchorOnlyWeights) == len(anchorOnlyNodes)
assert all(value > 0.0 for value in anchorOnlyWeights)
for anchorOnlyNode in anchorOnlyNodes:
    distance = min(
        (anchorOnlyNode - gaussAnchors[i]).norm()
        for i in range(gaussAnchors.getSize())
    )
    assert distance < 1e-15
checkMoments(anchorOnlyNodes, anchorOnlyWeights, 1e-9)

# More anchors than needed: the minimum-weight program selects the subset of
# anchors reproducing the moments, again without adding nodes.
extraAnchors = ot.Sample([[2.2], [2.8], [3.2], [3.8]])
extraQuad = otexp.GaussLPQuadrature(rawFunctions, rawMeasure, extraAnchors)
extraNodes, extraWeights = extraQuad.build(3)
assert 0 < len(extraNodes) <= len(extraAnchors)
assert len(extraWeights) == len(extraNodes)
assert all(value > 0.0 for value in extraWeights)
for extraNode in extraNodes:
    distance = min(
        (extraNode - extraAnchors[i]).norm()
        for i in range(extraAnchors.getSize())
    )
    assert distance < 1e-15
checkMoments(extraNodes, extraWeights)

# When the anchors cannot reproduce the moments on their own, they are all
# forced into the rule and candidates complete it.
failedAnchors = ot.Sample([[2.2], [2.4], [2.6]])
failedQuad = otexp.GaussLPQuadrature(rawFunctions, rawMeasure, failedAnchors)
failedNodes, failedWeights = failedQuad.build(3)
assert len(failedNodes) > len(failedAnchors)
assert len(failedWeights) == len(failedNodes)
assert all(value > 0.0 for value in failedWeights)
ott.assert_almost_equal(failedNodes[: len(failedAnchors)], failedAnchors)
checkMoments(failedNodes, failedWeights)

# Fewer anchors than functions: all the anchors are forced and the rule is
# completed with added nodes.
forcedAnchors = ot.Sample([[2.2], [3.8]])
forcedQuad = otexp.GaussLPQuadrature(rawFunctions, rawMeasure, forcedAnchors)
forcedNodes, forcedWeights = forcedQuad.build(3)
assert 0 < len(forcedNodes) <= 3
assert len(forcedWeights) == len(forcedNodes)
assert all(value > 0.0 for value in forcedWeights)
ott.assert_almost_equal(forcedNodes[: len(forcedAnchors)], forcedAnchors)
checkMoments(forcedNodes, forcedWeights)

# An empty anchor sample is accepted and leaves the builder unchanged.
emptyAnchorQuad = otexp.GaussLPQuadrature(
    rawFunctions, rawMeasure, ot.Sample()
)
assert emptyAnchorQuad.getAnchors().getSize() == 0

# Duplicate anchors are removed, keeping a single copy.
duplicateQuad = otexp.GaussLPQuadrature(
    rawFunctions, rawMeasure, ot.Sample([[2.2], [2.2], [3.8]])
)
assert duplicateQuad.getAnchors().getSize() == 2
duplicateNodes, duplicateWeights = duplicateQuad.build(3)
ott.assert_almost_equal(duplicateNodes[:2], ot.Sample([[2.2], [3.8]]))
checkMoments(duplicateNodes, duplicateWeights)

# Anchors must be finite, dimensioned like the measure and have a positive
# target PDF.
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadrature(rawFunctions, rawMeasure, ot.Sample([[1.0]]))
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadrature(rawFunctions, rawMeasure, ot.Sample([[4.5]]))
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadrature(rawFunctions, rawMeasure, ot.Sample([[2.2, 1.0]]))
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadrature(
        rawFunctions, rawMeasure, ot.Sample([[float("nan")]])
    )

# The anchor accessor validates immediately when the measure is set, and a
# rejected update leaves the anchors unchanged.
accessorQuad = otexp.GaussLPQuadrature(
    rawFunctions, rawMeasure, ot.Sample([[2.2], [3.8]])
)
accessorQuad.setAnchors(ot.Sample([[2.5], [3.5]]))
ott.assert_almost_equal(accessorQuad.getAnchors(), ot.Sample([[2.5], [3.5]]))
with ott.assert_raises(TypeError):
    accessorQuad.setAnchors(ot.Sample([[1.0]]))
assert accessorQuad.getAnchors().getSize() == 2
accessorQuad.setAnchors(ot.Sample())
assert accessorQuad.getAnchors().getSize() == 0

# The builder can be configured in any order: anchors set before the measure
# are validated when the measure is set.
deferredQuad = otexp.GaussLPQuadrature()
deferredQuad.setAnchors(ot.Sample([[2.2], [3.8]]))
with ott.assert_raises(TypeError):
    deferredQuad.build(3)
deferredQuad.setMeasureAndFunctions(rawMeasure, rawFunctions)
assert deferredQuad.getAnchors().getSize() == 2
deferredNodes, deferredWeights = deferredQuad.build(3)
ott.assert_almost_equal(deferredNodes[:2], ot.Sample([[2.2], [3.8]]))
checkMoments(deferredNodes, deferredWeights)

# The builder returns an immutable weighted experiment
rule = quad.buildRule(n)
assert isinstance(rule, otexp.GaussLPQuadratureRule)
assert isinstance(rule, ot.WeightedExperimentImplementation)
assert 0 < rule.getSize() <= n
assert rule.getDistribution().getDimension() == dim
ott.assert_almost_equal(rule.getDistribution().getMean(), distribution.getMean())
assert not rule.hasUniformWeights()
assert not rule.isRandom()
ruleNodes, ruleWeights = rule.generateWithWeights()
assert len(ruleNodes) == rule.getSize()
assert len(ruleWeights) == rule.getSize()
ott.assert_almost_equal(sum(ruleWeights), 1.0, 1e-3, 1e-3)
# The rule is deterministic once built.
ruleNodesAgain, ruleWeightsAgain = rule.generateWithWeights()
ott.assert_almost_equal(ruleNodesAgain, ruleNodes)
ott.assert_almost_equal(ruleWeightsAgain, ruleWeights)
assert len(rule.generate()) == rule.getSize()

# The rule is immutable
with ott.assert_raises(TypeError):
    rule.setSize(rule.getSize() + 1)
with ott.assert_raises(TypeError):
    rule.setDistribution(ot.Normal(dim))
# Setting the current size is a no-op, not a change.
rule.setSize(rule.getSize())
assert rule.getSize() == len(ruleNodes)
assert all(value >= 0.0 for value in ruleWeights)
assert "GaussLPQuadratureRule" in repr(rule)
assert "GaussLPQuadrature" in repr(quad)

# The rule constructor validates its arguments and exposes uniform weights.
uniformRule = otexp.GaussLPQuadratureRule(
    ot.Sample([[0.25], [0.75]]),
    ot.Point([0.5, 0.5]),
    ot.Uniform(0.0, 1.0),
)
assert uniformRule.getSize() == 2
assert uniformRule.hasUniformWeights()
assert not uniformRule.isRandom()
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadratureRule(ot.Sample(), ot.Point(), ot.Uniform(0.0, 1.0))
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadratureRule(
        ot.Sample([[0.5], [0.25]]), ot.Point([1.0]), ot.Uniform(0.0, 1.0)
    )
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadratureRule(
        ot.Sample([[0.5], [0.25]]), ot.Point([0.5, 0.5]), ot.Normal(2)
    )
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadratureRule(
        ot.Sample([[float("nan")]]), ot.Point([1.0]), ot.Uniform(0.0, 1.0)
    )
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadratureRule(
        ot.Sample([[0.5]]), ot.Point([-0.1]), ot.Uniform(0.0, 1.0)
    )
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadratureRule(
        ot.Sample([[0.5]]), ot.Point([float("inf")]), ot.Uniform(0.0, 1.0)
    )

# The tensor rule used for the moments is capped so that a high-dimensional
# measure does not materialize a huge tensor product.
ot.ResourceMap.SetAsUnsignedInteger("GaussLPQuadrature-MaximumTensorNodes", 100)
try:
    cappedNodes, cappedWeights = factory.buildQuadrature(n)
    assert 0 < len(cappedNodes) <= n
    assert len(cappedWeights) == len(cappedNodes)
    ott.assert_almost_equal(sum(cappedWeights), 1.0, 1e-3, 1e-3)
    for k in range(n):
        integral = sum(
            cappedWeights[i] * functions[k](cappedNodes[i])[0]
            for i in range(len(cappedNodes))
        )
        expected = 1.0 if k == 0 else 0.0
        ott.assert_almost_equal(integral, expected, 1e-3, 1e-3)
    # A null tensor budget is rejected.
    ot.ResourceMap.SetAsUnsignedInteger("GaussLPQuadrature-MaximumTensorNodes", 0)
    with ott.assert_raises(TypeError):
        factory.buildQuadrature(n)
    # A budget that cannot even hold the order-2 tensor rule is rejected
    # instead of materializing 2^dimension nodes.
    ot.ResourceMap.SetAsUnsignedInteger("GaussLPQuadrature-MaximumTensorNodes", 1)
    with ott.assert_raises(TypeError):
        factory.buildQuadrature(n)
finally:
    ot.ResourceMap.SetAsUnsignedInteger("GaussLPQuadrature-MaximumTensorNodes", 1000000)

# Every documented ResourceMap key is validated before use.
assert ot.ResourceMap.GetAsUnsignedInteger("GaussLPQuadrature-Margin") == 10
for key in [
    "GaussLPQuadrature-AlphaS",
    "GaussLPQuadrature-Margin",
    "GaussLPQuadrature-Ngauss",
]:
    previous = ot.ResourceMap.GetAsUnsignedInteger(key)
    try:
        ot.ResourceMap.SetAsUnsignedInteger(key, 0)
        with ott.assert_raises(TypeError):
            factory.buildQuadrature(n)
    finally:
        ot.ResourceMap.SetAsUnsignedInteger(key, previous)
previous = ot.ResourceMap.GetAsScalar("GaussLPQuadrature-Epsilon")
try:
    ot.ResourceMap.SetAsScalar("GaussLPQuadrature-Epsilon", -1.0)
    with ott.assert_raises(TypeError):
        factory.buildQuadrature(n)
finally:
    ot.ResourceMap.SetAsScalar("GaussLPQuadrature-Epsilon", previous)
previous = ot.ResourceMap.GetAsString("GaussLPQuadrature-Solver")
try:
    ot.ResourceMap.SetAsString("GaussLPQuadrature-Solver", "")
    with ott.assert_raises(TypeError):
        factory.buildQuadrature(n)
finally:
    ot.ResourceMap.SetAsString("GaussLPQuadrature-Solver", previous)

# The standard normal rule is transported only when the standard space is the
# standard normal distribution: a non-Gaussian elliptical measure with a
# dependency has no usable default rule and must be rejected.
correlation = ot.CorrelationMatrix(dim)
correlation[0, 1] = 0.5
student = ot.Student(3, [0.0] * dim, [1.0] * dim, correlation)
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadrature(functions, student).build(n)

# Non-unit support volume: moments must include the Jacobian (volume) factor
dist1d = ot.Uniform(2.0, 4.0)
refBasis1d = ot.OrthogonalProductPolynomialFactory([ot.LegendreFactory()])
initialBasis1d = [refBasis1d.build(i) for i in range(4)]
factory1d = otexp.BasisOrthonormalizationFactory(
    makeBasis(initialBasis1d, dist1d), len(initialBasis1d)
)
functions1d = [factory1d.build(k) for k in range(len(initialBasis1d))]
n1d = 2
nodes1d, weights1d = factory1d.buildQuadrature(n1d)
assert 0 < len(nodes1d) <= n1d
assert len(weights1d) == len(nodes1d)
ott.assert_almost_equal(sum(weights1d), 1.0, 1e-3, 1e-3)
for k in range(n1d):
    integral = sum(weights1d[i] * functions1d[k](nodes1d[i])[0] for i in range(len(nodes1d)))
    if k == 0:
        ott.assert_almost_equal(integral, 1.0, 1e-3, 1e-3)
    else:
        ott.assert_almost_equal(integral, 0.0, 1e-3, 1e-3)

# LP solver fallback: an invalid primary solver fails over to the fallback
# solver. Anchors are supplied so that a solver failure cannot be hidden by
# the measure-adapted fallback rule.
quad1d = otexp.GaussLPQuadrature(
    functions1d, dist1d, ot.Sample([[2.2], [3.8]])
)
ot.ResourceMap.SetAsString('GaussLPQuadrature-Solver', 'nonsense')
nodesFb, weightsFb = quad1d.build(n1d)
ott.assert_almost_equal(sum(weightsFb), 1.0, 1e-3, 1e-3)
# The ResourceMap is never modified to switch solvers
assert ot.ResourceMap.GetAsString('GaussLPQuadrature-Solver') == 'nonsense'
assert ot.ResourceMap.GetAsString('GaussLPQuadrature-FallbackSolver') == 'ipm'
# An empty fallback disables the retry and surfaces the original error
ot.ResourceMap.SetAsString('GaussLPQuadrature-FallbackSolver', '')
with ott.assert_raises(TypeError):
    quad1d.build(n1d)
# Identical primary and fallback solvers: no pointless retry, error surfaces
ot.ResourceMap.SetAsString('GaussLPQuadrature-FallbackSolver', 'nonsense')
with ott.assert_raises(TypeError):
    quad1d.build(n1d)
ot.ResourceMap.SetAsString('GaussLPQuadrature-Solver', 'choose')
ot.ResourceMap.SetAsString('GaussLPQuadrature-FallbackSolver', 'ipm')

# Highly correlated 2D normal distribution: the LP equality system is
# ill-conditioned. Check the default solver chain, then an explicit interior
# point solver without fallback. Both must return a valid rule.
correlation = ot.CorrelationMatrix(2)
correlation[0, 1] = 0.99
correlatedDistribution = ot.Normal([0.0, 0.0], [1.0, 1.0], correlation)
correlatedFactory = otexp.BasisOrthonormalizationFactory(
    makeBasis(initialBasis, correlatedDistribution), len(initialBasis)
)
correlatedFunctions = [correlatedFactory.build(k) for k in range(len(initialBasis))]
correlatedQuad = otexp.GaussLPQuadrature(correlatedFunctions, correlatedDistribution)

for solver, fallback in [('choose', 'ipm'), ('ipm', '')]:
    ot.ResourceMap.SetAsString('GaussLPQuadrature-Solver', solver)
    ot.ResourceMap.SetAsString('GaussLPQuadrature-FallbackSolver', fallback)
    correlatedNodes, correlatedWeights = correlatedQuad.build(4)
    assert 0 < len(correlatedNodes) <= 4
    assert len(correlatedWeights) == len(correlatedNodes)
    assert all(value >= 0.0 for value in correlatedWeights)
    ott.assert_almost_equal(sum(correlatedWeights), 1.0, 1e-3, 1e-3)
    for k in range(4):
        integral = sum(
            correlatedWeights[i] * correlatedFunctions[k](correlatedNodes[i])[0]
            for i in range(len(correlatedNodes))
        )
        expected = 1.0 if k == 0 else 0.0
        ott.assert_almost_equal(integral, expected, 1e-3, 1e-3)

# Restore the default solver configuration.
ot.ResourceMap.SetAsString('GaussLPQuadrature-Solver', 'choose')
ot.ResourceMap.SetAsString('GaussLPQuadrature-FallbackSolver', 'ipm')

# Heavy-tailed measures: the moment and candidate kernels must be adapted to
# the measure instead of using a Gauss-Legendre rule on the numerical range.
heavyBasis = [
    ot.SymbolicFunction("x", "1"),
    ot.SymbolicFunction("x", "x"),
    ot.SymbolicFunction("x", "x^2"),
]
for heavyDistribution in [ot.LogNormal(), ot.Student(3)]:
    heavyFactory = otexp.BasisOrthonormalizationFactory(
        makeBasis(heavyBasis, heavyDistribution), len(heavyBasis)
    )
    heavyFunctions = [heavyFactory.build(k) for k in range(len(heavyBasis))]
    heavyQuad = otexp.GaussLPQuadrature(heavyFunctions, heavyDistribution)
    heavyNodes, heavyWeights = heavyQuad.build(3)
    assert 0 < len(heavyNodes) <= 3
    assert len(heavyWeights) == len(heavyNodes)
    assert all(math.isfinite(value) for value in heavyWeights)
    assert all(value >= 0.0 for value in heavyWeights)
    ott.assert_almost_equal(sum(heavyWeights), 1.0, 1e-3, 1e-3)
    for k in range(3):
        integral = sum(
            heavyWeights[i] * heavyFunctions[k](heavyNodes[i])[0]
            for i in range(len(heavyNodes))
        )
        expected = 1.0 if k == 0 else 0.0
        ott.assert_almost_equal(integral, expected, 1e-3, 1e-3)

# The factory exposes the rule as a weighted experiment without unwrapping it.
heavyRule = otexp.BasisOrthonormalizationFactory(
    makeBasis(heavyBasis, ot.LogNormal()), len(heavyBasis)
).buildQuadratureRule(3)
assert isinstance(heavyRule, ot.WeightedExperiment)
assert 0 < heavyRule.getSize() <= 3
assert len(heavyRule.generate()) == heavyRule.getSize()
heavyRuleNodes, heavyRuleWeights = heavyRule.generateWithWeights()
assert len(heavyRuleNodes) == heavyRule.getSize()
assert len(heavyRuleWeights) == heavyRule.getSize()
ott.assert_almost_equal(sum(heavyRuleWeights), 1.0, 1e-3, 1e-3)

# Save/load round trip for the builder: the functions, the measure and the
# anchors are persisted.
savedQuad = otexp.GaussLPQuadrature(
    rawFunctions, rawMeasure, ot.Sample([[2.2], [3.8]])
)
studyFile = "gauss_lp_quadrature.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(studyFile))
study.add("quad", savedQuad)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager(studyFile))
loadedStudy.load()
loadedQuad = otexp.GaussLPQuadrature()
loadedStudy.fillObject("quad", loadedQuad)
assert loadedQuad.getSize() == len(rawFunctions)
assert len(loadedQuad.getFunctionsCollection()) == len(rawFunctions)
ott.assert_almost_equal(loadedQuad.getMeasure().getParameter(), ot.Point([2.0, 4.0]))
assert loadedQuad.getAnchors().getSize() == 2
ott.assert_almost_equal(loadedQuad.getAnchors(), ot.Sample([[2.2], [3.8]]))
loadedNodes, loadedWeights = loadedQuad.build(3)
ott.assert_almost_equal(loadedNodes[:2], ot.Sample([[2.2], [3.8]]))
checkMoments(loadedNodes, loadedWeights)
os.remove(studyFile)

# Save/load round trip for the immutable rule
studyFile = "gauss_lp_quadrature_rule.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(studyFile))
study.add("rule", rule)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager(studyFile))
loadedStudy.load()
loadedRule = otexp.GaussLPQuadratureRule()
loadedStudy.fillObject("rule", loadedRule)
assert loadedRule.getSize() == rule.getSize()
loadedRuleNodes, loadedRuleWeights = loadedRule.generateWithWeights()
ott.assert_almost_equal(loadedRuleNodes, ruleNodes)
ott.assert_almost_equal(loadedRuleWeights, ruleWeights)
os.remove(studyFile)

# Public accessors: setMeasureAndFunctions preserves and revalidates the
# anchors, and a rejected update leaves the builder unchanged.
preservedQuad = otexp.GaussLPQuadrature(
    rawFunctions, rawMeasure, ot.Sample([[2.6], [3.8]])
)
preservedQuad.setMeasureAndFunctions(ot.Uniform(2.5, 4.5), rawFunctions)
assert preservedQuad.getSize() == len(rawFunctions)
assert len(preservedQuad.getFunctionsCollection()) == len(rawFunctions)
assert preservedQuad.getAnchors().getSize() == 2
ott.assert_almost_equal(
    preservedQuad.getMeasure().getParameter(), ot.Point([2.5, 4.5])
)
with ott.assert_raises(TypeError):
    preservedQuad.setMeasureAndFunctions(ot.Uniform(5.0, 6.0), rawFunctions)
# A rejected setter leaves the builder unchanged.
ott.assert_almost_equal(
    preservedQuad.getMeasure().getParameter(), ot.Point([2.5, 4.5])
)
assert preservedQuad.getAnchors().getSize() == 2
with ott.assert_raises(TypeError):
    preservedQuad.setMeasureAndFunctions(rawMeasure, [])
with ott.assert_raises(TypeError):
    preservedQuad.setMeasureAndFunctions(ot.Normal(3), rawFunctions)
with ott.assert_raises(TypeError):
    preservedQuad.setMeasureAndFunctions(
        rawMeasure, [ot.SymbolicFunction(["x", "y"], ["1"])]
    )
with ott.assert_raises(TypeError):
    preservedQuad.setMeasureAndFunctions(
        rawMeasure, [ot.SymbolicFunction(["x"], ["1", "1"])]
    )

# Guard: the constructors validate their arguments.
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadrature([], distribution)
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadrature(functions, ot.Distribution())
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadrature([ot.SymbolicFunction(["x"], ["1"])], distribution)
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadrature(
        [ot.SymbolicFunction(["x", "y"], ["x", "y"])], distribution
    )

# The local refinement uses dedicated ResourceMap tolerances, tighter than the
# generic optimization defaults and independent from them.
toleranceKeys = [
    "GaussLPQuadrature-MaximumAbsoluteError",
    "GaussLPQuadrature-MaximumRelativeError",
    "GaussLPQuadrature-MaximumObjectiveError",
    "GaussLPQuadrature-MaximumConstraintError",
]
savedTolerances = {key: ot.ResourceMap.GetAsScalar(key) for key in toleranceKeys}
for key in toleranceKeys:
    ott.assert_almost_equal(ot.ResourceMap.GetAsScalar(key), 1.0e-12, 1.0e-15, 1.0e-15)
# The clustering path is enabled by default: the builder returns the smallest
# rule on the path that meets the residual tolerance.
assert ot.ResourceMap.GetAsBool("GaussLPQuadrature-ClusterPath")


def buildMomentError(quad, basisFunctions, order):
    ruleNodes, ruleWeights = quad.build(order)
    error = max(
        abs(
            sum(
                ruleWeights[i] * basisFunctions[k](ruleNodes[i])[0]
                for i in range(len(ruleNodes))
            )
            - (1.0 if k == 0 else 0.0)
        )
        for k in range(order)
    )
    return ruleNodes, ruleWeights, error


toleranceBasis = ot.OrthogonalBasis(
    ot.OrthogonalProductPolynomialFactory(
        [ot.LegendreFactory()] * 2, ot.NormInfEnumerateFunction(2)
    )
)
toleranceFunctions = [toleranceBasis.build(k) for k in range(16)]
toleranceMeasure = toleranceBasis.getMeasure()
savedEpsilon = ot.ResourceMap.GetAsScalar("GaussLPQuadrature-Epsilon")
try:
    ot.ResourceMap.SetAsScalar("GaussLPQuadrature-Epsilon", 1.0e-5)
    # Loose dedicated tolerances stop the refinement early: the moments are
    # only reproduced to a few 1e-4.
    for key in toleranceKeys:
        ot.ResourceMap.SetAsScalar(key, 1.0e-3)
    ot.RandomGenerator.SetSeed(0)
    looseQuad = otexp.GaussLPQuadrature(toleranceFunctions, toleranceMeasure)
    _, _, looseError = buildMomentError(
        looseQuad, toleranceFunctions, len(toleranceFunctions)
    )
    assert looseError > 1.0e-8

    # With the tight dedicated tolerances, the clustering path recovers the
    # 2 x 2 tensor Gauss-Legendre rule instead of refining the full rule.
    for key in toleranceKeys:
        ot.ResourceMap.SetAsScalar(key, 1.0e-12)
    ot.RandomGenerator.SetSeed(0)
    tightQuad = otexp.GaussLPQuadrature(toleranceFunctions, toleranceMeasure)
    tightNodes, tightWeights, tightError = buildMomentError(
        tightQuad, toleranceFunctions, len(toleranceFunctions)
    )
    assert len(tightNodes) == 4
    reference = ot.GaussProductExperiment(toleranceBasis.getMeasure(), [2] * 2)
    referenceNodes, referenceWeights = reference.generateWithWeights()
    for referenceNode in referenceNodes:
        distance = min(
            (tightNodes[i] - referenceNode).norm() for i in range(len(tightNodes))
        )
        assert distance < 1.0e-6
    weightDifference = max(
        abs(tightWeight - referenceWeight)
        for tightWeight, referenceWeight in zip(
            sorted(tightWeights), sorted(referenceWeights)
        )
    )
    assert weightDifference < 1.0e-6
    assert tightError < 1.0e-6
    assert tightError < looseError

    # Without the clustering path, only the largest rule is refined: it uses
    # all the requested functions and reaches a much smaller residual.
    ot.ResourceMap.SetAsBool("GaussLPQuadrature-ClusterPath", False)
    ot.RandomGenerator.SetSeed(0)
    fullQuad = otexp.GaussLPQuadrature(toleranceFunctions, toleranceMeasure)
    fullNodes, _, fullError = buildMomentError(
        fullQuad, toleranceFunctions, len(toleranceFunctions)
    )
    assert len(fullNodes) == len(toleranceFunctions)
    assert fullError < 1.0e-8
finally:
    for key, value in savedTolerances.items():
        ot.ResourceMap.SetAsScalar(key, value)
    ot.ResourceMap.SetAsScalar("GaussLPQuadrature-Epsilon", savedEpsilon)
    ot.ResourceMap.SetAsBool("GaussLPQuadrature-ClusterPath", True)

# Guard: the requested number of functions must be positive and must not
# exceed the number of functions available to the builder.
guardQuad = otexp.GaussLPQuadrature(functions, distribution)
with ott.assert_raises(TypeError):
    guardQuad.build(0)
with ott.assert_raises(TypeError):
    guardQuad.build(len(initialBasis) + 1)
with ott.assert_raises(TypeError):
    guardQuad.buildRule(0)
with ott.assert_raises(TypeError):
    guardQuad.buildRule(len(initialBasis) + 1)

# Guard: functions whose dimension does not match the measure, and non-scalar
# functions, are rejected.
badInputDimension = [ot.SymbolicFunction(ot.Description(["x", "y", "z"]), ot.Description(["1"]))] + functions[1:]
with ott.assert_raises(TypeError):
    otexp.FiniteOrthogonalFunctionFactory(badInputDimension, distribution)
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadrature(badInputDimension, distribution)
badOutputDimension = [ot.SymbolicFunction(ot.Description(["x"]), ot.Description(["1", "1"]))] + functions[1:]
with ott.assert_raises(TypeError):
    otexp.FiniteOrthogonalFunctionFactory(badOutputDimension, distribution)
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadrature(badOutputDimension, distribution)

# Guard: an empty rule cannot be built.
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadrature().build(1)
with ott.assert_raises(TypeError):
    otexp.GaussLPQuadrature().buildRule(1)

# Restore the production settings.
ot.ResourceMap.SetAsUnsignedInteger(discretizationKey, savedDiscretization)
ot.ResourceMap.SetAsUnsignedInteger(ngaussKey, savedNgauss)
