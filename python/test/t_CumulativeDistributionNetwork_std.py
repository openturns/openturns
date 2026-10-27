#! /usr/bin/env python

import io
import math
import os
import pickle
import tempfile

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(12345)

SQRT_2PI = math.sqrt(2.0 * math.pi)


def gaussian_partial(factor, local, positions):
    """Mixed partial derivative of a bivariate Gaussian CDF."""
    if len(positions) == 0:
        return factor.computeCDF(local)
    if len(positions) == 2:
        return factor.computePDF(local)
    first = positions[0]
    second = 1 - first
    marginal = factor.getMarginal(first)
    conditional = factor.getMarginal([first, second])
    return marginal.computePDF(local[first]) * conditional.computeConditionalCDF(
        local[second], [local[first]]
    )


def independent_partial(factor, local, positions):
    """Mixed partial derivative of a CDF with independent copula."""
    result = 1.0
    for j in range(factor.getDimension()):
        if j in positions:
            result *= factor.getMarginal(j).computePDF(local[j])
        else:
            result *= factor.getMarginal(j).computeCDF(local[j])
    return result


def mixed_difference(factors, graph, point, derivative):
    """Mixed backward difference of the product of the factor CDFs."""
    result = 0.0
    for subset in range(1 << len(derivative)):
        argument = list(point)
        sign = 1.0
        for k, variable in enumerate(derivative):
            if (subset >> k) & 1:
                argument[variable] -= 1.0
                sign = -sign
        value = 1.0
        for c, variables in enumerate(graph):
            value *= factors[c].computeCDF([argument[j] for j in variables])
        result += sign * value
    return result


def expected_density(factors, graph, point, partial):
    """Enumerate the derivative assignments and sum their products."""
    variable_factors = [[] for _ in point]
    for c, variables in enumerate(graph):
        for j in variables:
            variable_factors[j].append(c)
    assignment = [0] * len(point)
    result = [0.0]

    def visit(j):
        if j == len(point):
            term = 1.0
            for c, variables in enumerate(graph):
                positions = tuple(
                    k for k, variable in enumerate(variables) if assignment[variable] == c
                )
                local = tuple(point[variable] for variable in variables)
                term *= partial(factors[c], local, positions)
            result[0] += term
            return
        for c in variable_factors[j]:
            assignment[j] = c
            visit(j + 1)

    visit(0)
    return result[0]


def correlated_normal(rho):
    correlation = ot.CorrelationMatrix(2)
    correlation[0, 1] = rho
    return ot.Normal([0.0, 0.0], [1.0, 1.0], correlation)


# ---------------------------------------------------------------------------
# Product of two independent bivariate normal factors
# ---------------------------------------------------------------------------
graph = ot.BipartiteGraph([[0, 1], [0, 1]])
distribution = ot.CumulativeDistributionNetwork([ot.Normal(2)] * 2, graph)
assert distribution.getDimension() == 2
assert distribution.isContinuous()
assert not distribution.isDiscrete()
assert not distribution.isIntegral()
assert not distribution.isElliptical()
assert distribution.hasIndependentCopula()

x = ot.Point([0.7, -0.4])
phi_x = [math.exp(-0.5 * value * value) / SQRT_2PI for value in x]
Phi_x = [ot.Normal().computeCDF(value) for value in x]
expected_cdf = Phi_x[0] ** 2 * Phi_x[1] ** 2
expected_pdf = 4.0 * phi_x[0] * phi_x[1] * Phi_x[0] * Phi_x[1]
ott.assert_almost_equal(distribution.computeCDF(x), expected_cdf, 1.0e-14)
ott.assert_almost_equal(distribution.computePDF(x), expected_pdf, 1.0e-14)
ott.assert_almost_equal(
    distribution.computeLogPDF(x), math.log(expected_pdf), 1.0e-13
)

# Conditional laws: each marginal is the square of a normal CDF
y = 0.2
phi_y = math.exp(-0.5 * y * y) / SQRT_2PI
Phi_y = ot.Normal().computeCDF(y)
ott.assert_almost_equal(distribution.computeConditionalCDF(y, [x[0]]), Phi_y**2)
ott.assert_almost_equal(
    distribution.computeConditionalPDF(y, [x[0]]), 2.0 * phi_y * Phi_y
)
ott.assert_almost_equal(
    distribution.computeConditionalQuantile(0.9, [x[0]]),
    ot.Normal().computeScalarQuantile(math.sqrt(0.9)),
)

# Exact marginals, including a permutation of the coordinates
marginal = distribution.getMarginal(0)
ott.assert_almost_equal(marginal.computeCDF([x[0]]), Phi_x[0] ** 2, 1.0e-14)
ott.assert_almost_equal(
    marginal.computePDF([x[0]]), 2.0 * phi_x[0] * Phi_x[0], 1.0e-13
)
permuted = distribution.getMarginal([1, 0])
assert permuted.getDimension() == 2
ott.assert_almost_equal(
    permuted.computeCDF([x[1], x[0]]), expected_cdf, 1.0e-14
)
ott.assert_almost_equal(
    permuted.computePDF([x[1], x[0]]), expected_pdf, 1.0e-13
)

# Tail behaviour: the log-density must not underflow
tail = ot.Point([-15.0, -15.0])
expected_log_pdf = (
    math.log(4.0)
    + 2.0 * ot.Normal().computeLogPDF(tail[0])
    + 2.0 * math.log(ot.Normal().computeCDF(tail[0]))
)
ott.assert_almost_equal(distribution.computeLogPDF(tail), expected_log_pdf, 1.0e-13)
ott.assert_almost_equal(
    distribution.computePDF(tail), math.exp(expected_log_pdf), 1.0e-12
)
ott.assert_almost_equal(
    distribution.computeCDF(tail), ot.Normal().computeCDF(tail[0]) ** 4, 1.0e-13
)
deep_tail = ot.Point([-30.0, -30.0])
expected_log_pdf = (
    math.log(4.0)
    + 2.0 * ot.Normal().computeLogPDF(deep_tail[0])
    + 2.0 * math.log(ot.Normal().computeCDF(deep_tail[0]))
)
ott.assert_almost_equal(
    distribution.computeLogPDF(deep_tail), expected_log_pdf, 1.0e-13
)
assert math.isfinite(distribution.computeLogPDF(deep_tail))
assert distribution.computePDF(deep_tail) == 0.0
assert distribution.computeCDF(deep_tail) == 0.0
# A single Gaussian factor must also keep its log-density finite in the tails
tail_factor = correlated_normal(0.5)
single = ot.CumulativeDistributionNetwork(
    [tail_factor], ot.BipartiteGraph([[0, 1]])
)
ott.assert_almost_equal(
    single.computeLogPDF(deep_tail),
    tail_factor.computeLogPDF(deep_tail),
    1.0e-14,
)
assert math.isfinite(single.computeLogPDF(deep_tail))

# A double factor is not elliptical, a single factor is
assert ot.CumulativeDistributionNetwork(
    [ot.Normal(3)], ot.BipartiteGraph([[0, 1, 2]])
).isElliptical()

# ---------------------------------------------------------------------------
# Two correlated normal factors: exact density and conditional laws
# ---------------------------------------------------------------------------
rho = 0.5
factor = correlated_normal(rho)
distribution = ot.CumulativeDistributionNetwork(
    [factor] * 2, ot.BipartiteGraph([[0, 1], [0, 1]])
)
assert not distribution.hasIndependentCopula()
assert not distribution.isElliptical()
x = ot.Point([0.3, -0.2])
joint_cdf = factor.computeCDF(x)
joint_pdf = factor.computePDF(x)
ott.assert_almost_equal(distribution.computeCDF(x), joint_cdf**2, 1.0e-14)
ott.assert_almost_equal(
    distribution.computePDF(x),
    expected_density([factor] * 2, [[0, 1], [0, 1]], x, gaussian_partial),
    1.0e-14,
)

marginal = distribution.getMarginal(0)
ott.assert_almost_equal(
    marginal.computeCDF([x[0]]), ot.Normal().computeCDF(x[0]) ** 2, 1.0e-14
)
ott.assert_almost_equal(
    marginal.computePDF([x[0]]),
    2.0
    * ot.Normal().computePDF(x[0])
    * ot.Normal().computeCDF(x[0]),
    1.0e-13,
)

scale = math.sqrt(1.0 - rho * rho)
shifted = (x[1] - rho * x[0]) / scale
conditional_cdf = (
    joint_cdf * ot.Normal().computeCDF(shifted) / ot.Normal().computeCDF(x[0])
)
ott.assert_almost_equal(
    distribution.computeConditionalCDF(x[1], [x[0]]), conditional_cdf, 1.0e-13
)
partial_x = ot.Normal().computePDF(x[0]) * ot.Normal().computeCDF(shifted)
partial_y = ot.Normal().computePDF(x[1]) * ot.Normal().computeCDF(
    (x[0] - rho * x[1]) / scale
)
conditional_pdf = (partial_x * partial_y + joint_cdf * joint_pdf) / (
    ot.Normal().computeCDF(x[0]) * ot.Normal().computePDF(x[0])
)
ott.assert_almost_equal(
    distribution.computeConditionalPDF(x[1], [x[0]]), conditional_pdf, 1.0e-12
)

level = 0.4
quantile = distribution.computeConditionalQuantile(level, [x[0]])
quantile_shifted = (quantile - rho * x[0]) / scale
expected_level = (
    factor.computeCDF([x[0], quantile])
    * ot.Normal().computeCDF(quantile_shifted)
    / ot.Normal().computeCDF(x[0])
)
ott.assert_almost_equal(expected_level, level, 1.0e-9)

# ---------------------------------------------------------------------------
# Tree: two bivariate factors sharing one variable
# ---------------------------------------------------------------------------
factor_01 = correlated_normal(0.4)
factor_12 = correlated_normal(-0.3)
graph = [[0, 1], [1, 2]]
tree = ot.CumulativeDistributionNetwork([factor_01, factor_12], ot.BipartiteGraph(graph))
point = ot.Point([0.2, -0.5, 0.8])
point_01 = ot.Point([point[0], point[1]])
point_12 = ot.Point([point[1], point[2]])
assert not tree.isElliptical()
ott.assert_almost_equal(
    tree.computeCDF(point),
    factor_01.computeCDF(point_01) * factor_12.computeCDF(point_12),
    1.0e-14,
)
ott.assert_almost_equal(
    tree.computePDF(point),
    expected_density([factor_01, factor_12], graph, point, gaussian_partial),
    1.0e-13,
)
ott.assert_almost_equal(
    tree.computeLogPDF(point), math.log(tree.computePDF(point)), 1.0e-13
)

# X2 | X0, X1 on the tree, using the derivative assignment rule
f12_cdf = factor_12.computeCDF(point_12)
f12_pdf = factor_12.computePDF(point_12)
partial_01 = gaussian_partial(factor_01, point_01, (0,))
pdf_01 = factor_01.computePDF(point_01)
num = pdf_01 * f12_cdf + partial_01 * gaussian_partial(
    factor_12, point_12, (0,)
)
den = (
    pdf_01 * ot.Normal().computeCDF(point[1])
    + partial_01 * ot.Normal().computePDF(point[1])
)
ott.assert_almost_equal(
    tree.computeConditionalCDF(point[2], point_01), num / den, 1.0e-13
)
num = pdf_01 * gaussian_partial(factor_12, point_12, (1,)) + partial_01 * f12_pdf
ott.assert_almost_equal(
    tree.computeConditionalPDF(point[2], point_01), num / den, 1.0e-13
)

# ---------------------------------------------------------------------------
# Cyclic component: three factors on a triangle
# ---------------------------------------------------------------------------
factors = [correlated_normal(0.4), correlated_normal(-0.3), correlated_normal(0.2)]
graph = [[0, 1], [1, 2], [0, 2]]
cyclic = ot.CumulativeDistributionNetwork(factors, ot.BipartiteGraph(graph))
point = ot.Point([0.3, -0.4, 0.7])
expected = expected_density(factors, graph, point, gaussian_partial)
ott.assert_almost_equal(cyclic.computePDF(point), expected, 1.0e-12)
ott.assert_almost_equal(cyclic.computeLogPDF(point), math.log(expected), 1.0e-12)
ott.assert_almost_equal(
    cyclic.computeCDF(point),
    factors[0].computeCDF(point[:2])
    * factors[1].computeCDF(point[1:])
    * factors[2].computeCDF([point[0], point[2]]),
    1.0e-14,
)

# ---------------------------------------------------------------------------
# Non-Gaussian dependent factor: singleton complement and exact marginals
# ---------------------------------------------------------------------------
# For a Dirichlet vector, the conditional law of X_k given X_0..X_{k-1} is a
# scaled beta law with the remaining parameters:
#   X_k / (1 - sum_{i<k} x_i) ~ Beta(alpha_k, sum_{i>k} alpha_i).
parameters = [1.0, 2.0, 3.0, 4.0, 5.0]
dirichlet = ot.Dirichlet(parameters)
distribution = ot.CumulativeDistributionNetwork(
    [dirichlet], ot.BipartiteGraph([[0, 1, 2, 3]])
)
point = ot.Point([0.05, 0.1, 0.15, 0.2])
ott.assert_almost_equal(
    distribution.computePDF(point), dirichlet.computePDF(point), 1.0e-14
)

# Three conditioning coordinates: the local derivative has a singleton
# complement, evaluated exactly as a marginal density times a conditional CDF.
scale = 1.0 - sum(point[:3])
beta = ot.Beta(parameters[3], sum(parameters[4:]), 0.0, 1.0)
ott.assert_almost_equal(
    distribution.computeConditionalCDF(point[3], ot.Point(point[:3])),
    beta.computeCDF(point[3] / scale),
    1.0e-12,
)
ott.assert_almost_equal(
    distribution.computeConditionalPDF(point[3], ot.Point(point[:3])),
    beta.computePDF(point[3] / scale) / scale,
    1.0e-12,
)

# Two conditioning coordinates: the conditional CDF uses the singleton
# complement of the marginal over the first three coordinates, and the
# conditional PDF is the full derivative of a Dirichlet marginal.
scale = 1.0 - sum(point[:2])
beta = ot.Beta(parameters[2], sum(parameters[3:]), 0.0, 1.0)
ott.assert_almost_equal(
    distribution.computeConditionalCDF(point[2], ot.Point(point[:2])),
    beta.computeCDF(point[2] / scale),
    1.0e-8,
)
ott.assert_almost_equal(
    distribution.computeConditionalPDF(point[2], ot.Point(point[:2])),
    beta.computePDF(point[2] / scale) / scale,
    1.0e-8,
)

# ---------------------------------------------------------------------------
# Integral factors: backward differences and discrete conditionals
# ---------------------------------------------------------------------------
multinomial = ot.Multinomial(8, [0.2, 0.3, 0.5])
poisson_0 = ot.Poisson(2.0)
poisson_2 = ot.Poisson(2.5)
factors = [multinomial, poisson_0, poisson_2]
graph = [[0, 1, 2], [0], [2]]
discrete = ot.CumulativeDistributionNetwork(factors, ot.BipartiteGraph(graph))
assert discrete.isDiscrete()
assert discrete.isIntegral()
assert not discrete.isContinuous()
point = ot.Point([2.0, 2.0, 4.0])
expected = mixed_difference(factors, graph, point, [0, 1, 2])
ott.assert_almost_equal(discrete.computePDF(point), expected, 1.0e-12)
ott.assert_almost_equal(discrete.computeLogPDF(point), math.log(expected), 1.0e-12)

# A single factor reduces to its own mass function
ott.assert_almost_equal(
    mixed_difference([multinomial], [[0, 1, 2]], point, [0, 1, 2]),
    multinomial.computePDF(point),
    1.0e-12,
)

# Conditional laws by direct differentiation of the joint CDF
large = 1000.0
value = point[1]
num = discrete.computeCDF([point[0], value, large]) - discrete.computeCDF(
    [point[0] - 1.0, value, large]
)
den = discrete.computeCDF([point[0], large, large]) - discrete.computeCDF(
    [point[0] - 1.0, large, large]
)
ott.assert_almost_equal(
    discrete.computeConditionalCDF(value, [point[0]]), num / den, 1.0e-12
)
num = (
    discrete.computeCDF([point[0], value, large])
    - discrete.computeCDF([point[0] - 1.0, value, large])
    - discrete.computeCDF([point[0], value - 1.0, large])
    + discrete.computeCDF([point[0] - 1.0, value - 1.0, large])
)
ott.assert_almost_equal(
    discrete.computeConditionalPDF(value, [point[0]]), num / den, 1.0e-12
)
quantile = discrete.computeConditionalQuantile(0.6, [point[0]])
assert discrete.computeConditionalCDF(quantile, [point[0]]) >= 0.6
assert discrete.computeConditionalCDF(quantile - 1.0, [point[0]]) < 0.6
assert discrete.computeCDF([math.inf] * 3) == 1.0
assert discrete.computeCDF([-math.inf, point[1], point[2]]) == 0.0

# Infinite bounds are integrated out exactly, even for factors such as
# Multinomial(1, p) whose CDF becomes inaccurate at large finite arguments.
bernoulli_0 = ot.Multinomial(1, [0.3, 0.7])
bernoulli_1 = ot.Multinomial(1, [0.6, 0.4])
bernoulli_factors = [bernoulli_0, bernoulli_1, poisson_0, poisson_2]
bernoulli_graph = [[0, 1], [1, 2], [0], [2]]
bernoulli = ot.CumulativeDistributionNetwork(
    bernoulli_factors, ot.BipartiteGraph(bernoulli_graph)
)
ott.assert_almost_equal(bernoulli.computeConditionalCDF(1.0, [0.0]), 1.0)
ott.assert_almost_equal(bernoulli.computeConditionalPDF(1.0, [0.0]), 1.0)
assert bernoulli.computeCDF([math.inf] * 3) == 1.0

# ---------------------------------------------------------------------------
# Independent blocks are sampled directly
# ---------------------------------------------------------------------------
block = ot.CumulativeDistributionNetwork(
    [ot.Normal(), ot.Exponential()], ot.BipartiteGraph([[0], [1]])
)
assert block.hasIndependentCopula()
sample = block.getSample(20000)
ott.assert_almost_equal(sample.computeMean(), [0.0, 1.0], 1.0e-2, 5.0e-2)
covariance = sample.computeCovariance()
ott.assert_almost_equal(covariance[0, 1], 0.0, 1.0e-2, 3.0e-2)
realization = block.getRealization()
assert realization.getDimension() == 2
assert all(math.isfinite(value) for value in realization)

# ---------------------------------------------------------------------------
# Generic base services on a two-dimensional model
# ---------------------------------------------------------------------------
distribution = ot.CumulativeDistributionNetwork(
    [correlated_normal(rho)] * 2, ot.BipartiteGraph([[0, 1], [0, 1]])
)
validation = ott.DistributionValidation(distribution)
validation.skipMoments()
validation.skipCorrelation()
validation.skipConditional()
validation.skipTransformation()
validation.skipEntropy()
validation.skipGradient()
validation.skipGeneratingFunction()
validation.skipCharacteristicFunction()
validation.skipMinimumVolumeInterval()
validation.skipMinimumVolumeLevelSet()
validation.skipConfidenceInterval()
validation.skipProbability()
validation.setDomainSamplingSize(10000)
validation.run()

# ---------------------------------------------------------------------------
# Construction, accessors, equality, description and range
# ---------------------------------------------------------------------------
default = ot.CumulativeDistributionNetwork()
assert default.getDimension() == 1
assert default.getGraph().getBlackNodes().getSize() == 1
assert default.getGraph().getRedNodes().getSize() == 1
assert default.isContinuous()
assert not default.isDiscrete()
assert not default.isIntegral()
assert default.isElliptical()
assert default.hasIndependentCopula()
assert list(default.getRange().getLowerBound()) == [-1.0]
assert list(default.getRange().getUpperBound()) == [1.0]
ott.assert_almost_equal(default.computeCDF([0.5]), 0.75)
ott.assert_almost_equal(default.computePDF([0.5]), 0.5)

normal_2d = ot.Normal(2)
base = ot.CumulativeDistributionNetwork([normal_2d], ot.BipartiteGraph([[0, 1]]))
assert base.getDistributionCollection()[0] == normal_2d
assert base.getGraph().getBlackNodes().getSize() == 2
assert len(base.getParameterDescription()) == base.getParameterDimension()
assert "class=CumulativeDistributionNetwork" in repr(base)
assert "CumulativeDistributionNetwork([" in str(base)

assert ot.CumulativeDistributionNetwork(
    [ot.Normal(2)], ot.BipartiteGraph([[0, 1]])
) == base
assert not (
    ot.CumulativeDistributionNetwork([ot.Normal(2)], ot.BipartiteGraph([[1, 0]]))
    == base
)
assert not (base == ot.Normal())

with ott.assert_raises(Exception):
    ot.CumulativeDistributionNetwork(
        [ot.Normal(2)], ot.BipartiteGraph([[0, 1], [1, 2]])
    )
with ott.assert_raises(Exception):
    ot.CumulativeDistributionNetwork([ot.Normal()], ot.BipartiteGraph([[0, 1]]))
with ott.assert_raises(Exception):
    ot.CumulativeDistributionNetwork([ot.Normal(2)], ot.BipartiteGraph([[0, 2]]))
with ott.assert_raises(Exception):
    ot.CumulativeDistributionNetwork([ot.Normal(2)], ot.BipartiteGraph([[0, 0]]))

# The range is the intersection of the projected factor ranges.
range_network = ot.CumulativeDistributionNetwork(
    [ot.JointDistribution([ot.Uniform(0.0, 2.0)] * 2), ot.Uniform(1.0, 3.0)],
    ot.BipartiteGraph([[0, 1], [0]]),
)
range_interval = range_network.getRange()
ott.assert_almost_equal(range_interval.getLowerBound(), [1.0, 0.0])
ott.assert_almost_equal(range_interval.getUpperBound(), [2.0, 2.0])
assert list(range_interval.getFiniteLowerBound()) == [True, True]
assert list(range_interval.getFiniteUpperBound()) == [True, True]

# Accessors rebuild the incidence caches and the range.
swap = ot.CumulativeDistributionNetwork(
    [ot.Uniform(0.0, 1.0), ot.Exponential()], ot.BipartiteGraph([[0], [1]])
)
ott.assert_almost_equal(
    swap.computeCDF([0.5, 1.0]),
    ot.Uniform(0.0, 1.0).computeCDF(0.5) * ot.Exponential().computeCDF(1.0),
)
swap.setGraph(ot.BipartiteGraph([[1], [0]]))
ott.assert_almost_equal(
    swap.computeCDF([1.0, 0.5]),
    ot.Uniform(0.0, 1.0).computeCDF(0.5) * ot.Exponential().computeCDF(1.0),
)
ott.assert_almost_equal(
    swap.computePDF([1.0, 0.5]),
    ot.Uniform(0.0, 1.0).computePDF(0.5) * ot.Exponential().computePDF(1.0),
)
swap.setDistributionCollection([ot.Uniform(0.0, 2.0), ot.Exponential()])
ott.assert_almost_equal(
    swap.computeCDF([1.0, 1.0]),
    ot.Uniform(0.0, 2.0).computeCDF(1.0) * ot.Exponential().computeCDF(1.0),
)
with ott.assert_raises(Exception):
    swap.setDistributionCollection([ot.Normal()])
with ott.assert_raises(Exception):
    swap.setDistributionCollection([ot.Normal(), ot.Normal(), ot.Normal()])
with ott.assert_raises(Exception):
    swap.setGraph(ot.BipartiteGraph([[0]]))
with ott.assert_raises(Exception):
    swap.setGraph(ot.BipartiteGraph([[0, 1, 2], [1]]))

# Exact marginals: full, permuted, single-factor and multi-factor cases.
described = ot.CumulativeDistributionNetwork(
    [ot.Normal(2)], ot.BipartiteGraph([[0, 1]])
)
described.setDescription(["a", "b"])
full_marginal = described.getMarginal([0, 1])
assert full_marginal == described
assert full_marginal.getDimension() == 2
permuted_marginal = described.getMarginal([1, 0])
assert permuted_marginal.getDescription() == ["b", "a"]
assert permuted_marginal.getDimension() == 2
single_marginal = described.getMarginal(1)
assert single_marginal.getDimension() == 1
assert single_marginal.getDescription() == ["b"]
ott.assert_almost_equal(
    single_marginal.computeCDF([0.3]), ot.Normal().computeCDF(0.3)
)
ott.assert_almost_equal(
    single_marginal.computePDF([0.3]), ot.Normal().computePDF(0.3)
)
with ott.assert_raises(Exception):
    described.getMarginal(2)
with ott.assert_raises(Exception):
    described.getMarginal([])
with ott.assert_raises(Exception):
    described.getMarginal([0, 0])
with ott.assert_raises(Exception):
    described.getMarginal([0, 2])

tree_marginal = tree.getMarginal([0, 2])
tree_point = ot.Point([0.2, -0.5, 0.8])
ott.assert_almost_equal(
    tree_marginal.computeCDF([tree_point[0], tree_point[2]]),
    ot.Normal().computeCDF(tree_point[0]) * ot.Normal().computeCDF(tree_point[2]),
    1.0e-14,
)
ott.assert_almost_equal(
    tree_marginal.computePDF([tree_point[0], tree_point[2]]),
    ot.Normal().computePDF(tree_point[0]) * ot.Normal().computePDF(tree_point[2]),
    1.0e-14,
)

# Parameters are concatenated, described and staged before being committed.
parameter_network = ot.CumulativeDistributionNetwork(
    [ot.Normal(), ot.Uniform(0.0, 1.0)], ot.BipartiteGraph([[0], [1]])
)
parameter = parameter_network.getParameter()
parameter_description = parameter_network.getParameterDescription()
assert len(parameter) == 4
assert list(parameter_description) == [
    "mu_0_distribution_0",
    "sigma_0_distribution_0",
    "a_distribution_1",
    "b_distribution_1",
]
with ott.assert_raises(Exception):
    parameter_network.setParameter([0.0])
assert parameter_network.getParameter() == parameter
with ott.assert_raises(Exception):
    parameter_network.setParameter([0.0, 1.0, 2.0, 1.0])
assert parameter_network.getParameter() == parameter
parameter_network.setParameter([0.5, 2.0, 1.0, 3.0])
ott.assert_almost_equal(parameter_network.getParameter(), [0.5, 2.0, 1.0, 3.0])
ott.assert_almost_equal(
    parameter_network.computeCDF([1.0, 2.0]),
    ot.Normal(0.5, 2.0).computeCDF(1.0) * ot.Uniform(1.0, 3.0).computeCDF(2.0),
    1.0e-14,
)

# Persistence through pickle and the Study mechanism.
buffer = io.BytesIO()
pickle.dump(base, buffer)
buffer.seek(0)
assert pickle.load(buffer) == base
if ot.PlatformInfo.HasFeature("libxml2"):
    study_path = tempfile.mktemp(suffix=".xml")
    try:
        study = ot.Study()
        study.setStorageManager(ot.XMLStorageManager(study_path))
        study.add("distribution", base)
        study.save()
        study = ot.Study()
        study.setStorageManager(ot.XMLStorageManager(study_path))
        study.load()
        loaded = ot.CumulativeDistributionNetwork()
        study.fillObject("distribution", loaded)
        assert loaded == base
        ott.assert_almost_equal(
            loaded.computePDF([0.3, -0.2]), base.computePDF([0.3, -0.2]), 1.0e-14
        )
    finally:
        if os.path.exists(study_path):
            os.remove(study_path)

# ---------------------------------------------------------------------------
# Infinite coordinates and independent-factor partial derivatives
# ---------------------------------------------------------------------------
independent = ot.CumulativeDistributionNetwork(
    [ot.Normal(2)], ot.BipartiteGraph([[0, 1]])
)
assert independent.hasIndependentCopula()
ott.assert_almost_equal(
    independent.computeCDF([0.3, 0.4]),
    ot.Normal().computeCDF(0.3) * ot.Normal().computeCDF(0.4),
    1.0e-14,
)
ott.assert_almost_equal(
    independent.computeCDF([0.3, math.inf]), ot.Normal().computeCDF(0.3), 1.0e-14
)
ott.assert_almost_equal(
    independent.computePDF([0.3, -0.4]),
    ot.Normal().computePDF(0.3) * ot.Normal().computePDF(-0.4),
    1.0e-14,
)
assert independent.computeCDF([math.inf, math.inf]) == 1.0
assert independent.computeCDF([-math.inf, 0.3]) == 0.0
assert independent.computePDF([math.inf, 0.0]) == 0.0
assert independent.computePDF([-math.inf, 0.0]) == 0.0

# A mixed network whose independent factor is combined with a dependent factor
# forces the derivative engine through the cached and uncached paths of the
# univariate product, rather than through the top-level independent shortcut.
independent_factor = ot.Normal(3)
dependent_factor = ot.Normal(
    [0.0, 0.0], [1.0, 1.0], correlated_normal(0.5).getCorrelation()
)
mixed_factors = ot.CumulativeDistributionNetwork(
    [independent_factor, dependent_factor],
    ot.BipartiteGraph([[0, 1, 2], [1, 2]]),
)
assert not mixed_factors.hasIndependentCopula()
mixed_point = [0.2, -0.4, 0.7]


def mixed_cdf(point):
    return mixed_factors.computeCDF(point)


step = 1.0e-4
numerator = (
    mixed_cdf([mixed_point[0] + step, math.inf, mixed_point[2]])
    - mixed_cdf([mixed_point[0] - step, math.inf, mixed_point[2]])
) / (2.0 * step)
denominator = (
    mixed_cdf([mixed_point[0] + step, math.inf, math.inf])
    - mixed_cdf([mixed_point[0] - step, math.inf, math.inf])
) / (2.0 * step)
assert (
    abs(
        mixed_factors.computeConditionalCDF(mixed_point[2], [mixed_point[0]])
        - numerator / denominator
    )
    < 1.0e-7
)
numerator = (
    mixed_cdf([mixed_point[0] + step, mixed_point[1] + step, mixed_point[2]])
    - mixed_cdf([mixed_point[0] + step, mixed_point[1] - step, mixed_point[2]])
    - mixed_cdf([mixed_point[0] - step, mixed_point[1] + step, mixed_point[2]])
    + mixed_cdf([mixed_point[0] - step, mixed_point[1] - step, mixed_point[2]])
) / (4.0 * step * step)
denominator = (
    mixed_cdf([mixed_point[0] + step, mixed_point[1] + step, math.inf])
    - mixed_cdf([mixed_point[0] + step, mixed_point[1] - step, math.inf])
    - mixed_cdf([mixed_point[0] - step, mixed_point[1] + step, math.inf])
    + mixed_cdf([mixed_point[0] - step, mixed_point[1] - step, math.inf])
) / (4.0 * step * step)
assert (
    abs(
        mixed_factors.computeConditionalCDF(
            mixed_point[2], [mixed_point[0], mixed_point[1]]
        )
        - numerator / denominator
    )
    < 1.0e-7
)
step = 1.0e-5
reference = (
    mixed_factors.computeConditionalCDF(
        mixed_point[2] + step, [mixed_point[0], mixed_point[1]]
    )
    - mixed_factors.computeConditionalCDF(
        mixed_point[2] - step, [mixed_point[0], mixed_point[1]]
    )
) / (2.0 * step)
assert (
    abs(
        mixed_factors.computeConditionalPDF(
            mixed_point[2], [mixed_point[0], mixed_point[1]]
        )
        - reference
    )
    < 1.0e-7
)
reference = (
    mixed_factors.computeConditionalCDF(mixed_point[2] + step, [mixed_point[0]])
    - mixed_factors.computeConditionalCDF(mixed_point[2] - step, [mixed_point[0]])
) / (2.0 * step)
assert (
    abs(
        mixed_factors.computeConditionalPDF(mixed_point[2], [mixed_point[0]])
        - reference
    )
    < 1.0e-7
)

# ---------------------------------------------------------------------------
# Exact local derivatives: Gaussian Schur complement and singleton complement
# ---------------------------------------------------------------------------
correlation_3d = ot.CorrelationMatrix(3)
correlation_3d[0, 1] = 0.4
correlation_3d[0, 2] = -0.2
correlation_3d[1, 2] = 0.3
gaussian_3d = ot.Normal(
    [0.1, -0.2, 0.3], [1.0, 1.5, 0.8], correlation_3d
)
gaussian_network = ot.CumulativeDistributionNetwork(
    [gaussian_3d], ot.BipartiteGraph([[0, 1, 2]])
)
ott.assert_almost_equal(
    gaussian_network.computeCDF([0.4, 0.1, -0.3]),
    gaussian_3d.computeCDF([0.4, 0.1, -0.3]),
    1.0e-13,
)
ott.assert_almost_equal(
    gaussian_network.computePDF([0.4, 0.1, -0.3]),
    gaussian_3d.computePDF([0.4, 0.1, -0.3]),
    1.0e-13,
)
for target, conditioning in [(1, [0.2]), (2, [0.2, 0.3])]:
    value = 0.4
    conditioning_point = ot.Point(conditioning)
    ott.assert_almost_equal(
        gaussian_network.computeConditionalCDF(value, conditioning_point),
        gaussian_3d.computeConditionalCDF(value, conditioning_point),
        1.0e-12,
    )
    ott.assert_almost_equal(
        gaussian_network.computeConditionalPDF(value, conditioning_point),
        gaussian_3d.computeConditionalPDF(value, conditioning_point),
        1.0e-12,
    )
assert gaussian_network.computeConditionalCDF(-40.0, ot.Point([0.0, 0.0])) == 0.0
assert gaussian_network.computeConditionalPDF(-40.0, ot.Point([0.0, 0.0])) == 0.0
for level in [0.1, 0.5, 0.9]:
    ott.assert_almost_equal(
        gaussian_network.computeConditionalQuantile(level, ot.Point([0.2, 0.3])),
        gaussian_3d.computeConditionalQuantile(level, ot.Point([0.2, 0.3])),
        1.0e-12,
    )
with ott.assert_raises(Exception):
    gaussian_network.computeCDF([0.4, 0.1])
with ott.assert_raises(Exception):
    gaussian_network.computePDF([0.4, 0.1])
with ott.assert_raises(Exception):
    gaussian_network.computeLogPDF([0.4, 0.1])
with ott.assert_raises(Exception):
    gaussian_network.computeConditionalCDF(0.0, [0.0, 0.0, 0.0])
assert gaussian_network.isElliptical()

# A non-Gaussian bivariate dependent factor uses the singleton-complement
# formula with an exact factor conditional CDF.
student_correlation = ot.CorrelationMatrix(2)
student_correlation[0, 1] = 0.4
student = ot.Student(4.0, [0.0, 0.0], [1.0, 1.0], student_correlation)
student_network = ot.CumulativeDistributionNetwork(
    [student], ot.BipartiteGraph([[0, 1]])
)
student_point = ot.Point([0.2, 0.3])
ott.assert_almost_equal(
    student_network.computeCDF(student_point),
    student.computeCDF(student_point),
    1.0e-13,
)
ott.assert_almost_equal(
    student_network.computePDF(student_point),
    student.computePDF(student_point),
    1.0e-13,
)
ott.assert_almost_equal(
    student_network.computeConditionalCDF(0.3, [0.2]),
    student.computeConditionalCDF(0.3, [0.2]),
    1.0e-13,
)
ott.assert_almost_equal(
    student_network.computeConditionalPDF(0.3, [0.2]),
    student.computeConditionalPDF(0.3, [0.2]),
    1.0e-13,
)
assert student_network.isElliptical()

# Dirichlet: every conditional is exact. The CDF uses the singleton-complement
# formula after marginalising the factor; the PDF is the full derivative of a
# Dirichlet marginal.
dirichlet_parameters = [1.0, 2.0, 3.0, 4.0, 5.0]
dirichlet = ot.Dirichlet(dirichlet_parameters)
dirichlet_network = ot.CumulativeDistributionNetwork(
    [dirichlet], ot.BipartiteGraph([[0, 1, 2, 3]])
)
dirichlet_point = ot.Point([0.05, 0.1, 0.15, 0.2])
ott.assert_almost_equal(
    dirichlet_network.computePDF(dirichlet_point),
    dirichlet.computePDF(dirichlet_point),
    1.0e-14,
)
assert not dirichlet_network.isElliptical()
for k in [1, 2, 3]:
    scale = 1.0 - sum(dirichlet_point[:k])
    beta = ot.Beta(dirichlet_parameters[k], sum(dirichlet_parameters[k + 1 :]), 0.0, 1.0)
    conditioning = ot.Point(dirichlet_point[:k])
    ott.assert_almost_equal(
        dirichlet_network.computeConditionalCDF(dirichlet_point[k], conditioning),
        beta.computeCDF(dirichlet_point[k] / scale),
        1.0e-12,
    )
    ott.assert_almost_equal(
        dirichlet_network.computeConditionalPDF(dirichlet_point[k], conditioning),
        beta.computePDF(dirichlet_point[k] / scale) / scale,
        1.0e-12,
    )

# A small MaximumTerms must not affect these exact paths: there is no
# finite-difference fallback in the conditional services.
maximum_terms_key = "CumulativeDistributionNetwork-MaximumTerms"
original_maximum_terms = ot.ResourceMap.GetAsUnsignedInteger(maximum_terms_key)
try:
    ot.ResourceMap.SetAsUnsignedInteger(maximum_terms_key, 1)
    scale = 1.0 - sum(dirichlet_point[:2])
    beta = ot.Beta(
        dirichlet_parameters[2], sum(dirichlet_parameters[3:]), 0.0, 1.0
    )
    ott.assert_almost_equal(
        dirichlet_network.computeConditionalCDF(
            dirichlet_point[2], ot.Point(dirichlet_point[:2])
        ),
        beta.computeCDF(dirichlet_point[2] / scale),
        1.0e-12,
    )
    scale = 1.0 - dirichlet_point[0]
    beta = ot.Beta(
        dirichlet_parameters[1], sum(dirichlet_parameters[2:]), 0.0, 1.0
    )
    ott.assert_almost_equal(
        dirichlet_network.computeConditionalPDF(
            dirichlet_point[1], ot.Point(dirichlet_point[:1])
        ),
        beta.computePDF(dirichlet_point[1] / scale) / scale,
        1.0e-12,
    )
finally:
    ot.ResourceMap.SetAsUnsignedInteger(maximum_terms_key, original_maximum_terms)

# ---------------------------------------------------------------------------
# Conditional services: independent shortcut, boundaries and invalid input
# ---------------------------------------------------------------------------
independent_conditional = ot.CumulativeDistributionNetwork(
    [ot.Normal(2)] * 2, ot.BipartiteGraph([[0, 1], [0, 1]])
)
ott.assert_almost_equal(
    independent_conditional.computeConditionalPDF(0.2, [0.7]),
    2.0 * ot.Normal().computePDF(0.2) * ot.Normal().computeCDF(0.2),
    1.0e-14,
)
ott.assert_almost_equal(
    independent_conditional.computeConditionalCDF(math.inf, [0.7]), 1.0
)
ott.assert_almost_equal(
    independent_conditional.computeConditionalCDF(-math.inf, [0.7]), 0.0
)
assert independent_conditional.computeConditionalPDF(math.inf, [0.7]) == 0.0
assert independent_conditional.computeConditionalPDF(-math.inf, [0.7]) == 0.0
ott.assert_almost_equal(
    independent_conditional.computeConditionalCDF(0.3, []),
    independent_conditional.getMarginal(0).computeCDF([0.3]),
    1.0e-14,
)
ott.assert_almost_equal(
    independent_conditional.computeConditionalPDF(0.3, []),
    independent_conditional.getMarginal(0).computePDF([0.3]),
    1.0e-14,
)
ott.assert_almost_equal(
    independent_conditional.computeConditionalQuantile(0.3, []),
    independent_conditional.getMarginal(0).computeScalarQuantile(0.3),
    1.0e-14,
)
ott.assert_almost_equal(
    independent_conditional.computeConditionalQuantile(0.0, [0.7]),
    independent_conditional.getMarginal(1).computeScalarQuantile(0.0),
)
ott.assert_almost_equal(
    independent_conditional.computeConditionalQuantile(1.0, [0.7]),
    independent_conditional.getMarginal(1).computeScalarQuantile(1.0),
)
with ott.assert_raises(Exception):
    independent_conditional.computeConditionalCDF(0.5, [0.0, 0.0])
with ott.assert_raises(Exception):
    independent_conditional.computeConditionalPDF(0.5, [0.0, 0.0])
with ott.assert_raises(Exception):
    independent_conditional.computeConditionalQuantile(0.5, [0.0, 0.0])
with ott.assert_raises(Exception):
    independent_conditional.computeConditionalQuantile(-0.1, [0.7])
with ott.assert_raises(Exception):
    independent_conditional.computeConditionalQuantile(1.1, [0.7])

# ---------------------------------------------------------------------------
# Mixed continuous/discrete and non-integral discrete networks
# ---------------------------------------------------------------------------
mixed = ot.CumulativeDistributionNetwork(
    [correlated_normal(0.5), ot.Poisson()], ot.BipartiteGraph([[0, 1], [1]])
)
assert not mixed.isContinuous()
assert not mixed.isDiscrete()
assert not mixed.isIntegral()
assert not mixed.hasIndependentCopula()
ott.assert_almost_equal(
    mixed.computeCDF([0.2, 1.0]),
    correlated_normal(0.5).computeCDF([0.2, 1.0]) * ot.Poisson().computeCDF(1.0),
    1.0e-14,
)
with ott.assert_raises(RuntimeError):
    mixed.computePDF([0.2, 1.0])
with ott.assert_raises(RuntimeError):
    mixed.computeLogPDF([0.2, 1.0])
with ott.assert_raises(RuntimeError):
    mixed.computeConditionalCDF(1.0, [0.2])
with ott.assert_raises(RuntimeError):
    mixed.computeConditionalPDF(1.0, [0.2])
with ott.assert_raises(RuntimeError):
    mixed.computeConditionalQuantile(0.5, [0.2])

# An independent mixed network can still use the independent conditional
# shortcut, which only needs the marginal of the target variable.
independent_mixed = ot.CumulativeDistributionNetwork(
    [ot.Normal(), ot.Poisson()], ot.BipartiteGraph([[0], [1]])
)
assert independent_mixed.hasIndependentCopula()
ott.assert_almost_equal(
    independent_mixed.computeConditionalCDF(1.0, [0.0]),
    ot.Poisson().computeCDF(1.0),
    1.0e-14,
)
ott.assert_almost_equal(
    independent_mixed.computeConditionalPDF(1.0, [0.0]),
    ot.Poisson().computePDF(1.0),
    1.0e-14,
)

finite_discrete = ot.FiniteDiscreteDistribution(
    ot.Sample([[0.5], [1.5]]), [0.5, 0.5]
)
finite_discrete_network = ot.CumulativeDistributionNetwork(
    [finite_discrete], ot.BipartiteGraph([[0]])
)
assert finite_discrete_network.isDiscrete()
assert not finite_discrete_network.isIntegral()
assert not finite_discrete_network.isContinuous()
ott.assert_almost_equal(finite_discrete_network.computeCDF([0.5]), 0.5)
with ott.assert_raises(RuntimeError):
    finite_discrete_network.computePDF([0.5])

# ---------------------------------------------------------------------------
# Integral networks: simple factor graph, conditionals, quantiles and guards
# ---------------------------------------------------------------------------
bernoulli_factor_0 = ot.Multinomial(1, [0.4, 0.6])
bernoulli_factor_1 = ot.Multinomial(1, [0.3, 0.7])
discrete_chain = ot.CumulativeDistributionNetwork(
    [bernoulli_factor_0, bernoulli_factor_1], ot.BipartiteGraph([[0, 1], [1, 2]])
)
assert discrete_chain.isDiscrete()
assert discrete_chain.isIntegral()
assert not discrete_chain.isContinuous()
assert not discrete_chain.hasIndependentCopula()
for value_0 in [0.0, 1.0]:
    for value_1 in [0.0, 1.0]:
        for value_2 in [0.0, 1.0]:
            chain_point = ot.Point([value_0, value_1, value_2])
            expected = mixed_difference(
                [bernoulli_factor_0, bernoulli_factor_1],
                [[0, 1], [1, 2]],
                chain_point,
                [0, 1, 2],
            )
            ott.assert_almost_equal(
                discrete_chain.computePDF(chain_point), expected, 1.0e-14
            )
            ott.assert_almost_equal(
                discrete_chain.computeCDF(chain_point),
                bernoulli_factor_0.computeCDF([value_0, value_1])
                * bernoulli_factor_1.computeCDF([value_1, value_2]),
                1.0e-14,
            )
chain_point = ot.Point([0.0, 1.0])
denominator = discrete_chain.computeCDF(
    [chain_point[0], math.inf, math.inf]
) - discrete_chain.computeCDF([chain_point[0] - 1.0, math.inf, math.inf])
numerator = discrete_chain.computeCDF(
    [chain_point[0], chain_point[1], math.inf]
) - discrete_chain.computeCDF([chain_point[0] - 1.0, chain_point[1], math.inf])
ott.assert_almost_equal(
    discrete_chain.computeConditionalCDF(chain_point[1], [chain_point[0]]),
    numerator / denominator,
    1.0e-14,
)
numerator = (
    discrete_chain.computeCDF([chain_point[0], chain_point[1], math.inf])
    - discrete_chain.computeCDF([chain_point[0] - 1.0, chain_point[1], math.inf])
    - discrete_chain.computeCDF([chain_point[0], chain_point[1] - 1.0, math.inf])
    + discrete_chain.computeCDF(
        [chain_point[0] - 1.0, chain_point[1] - 1.0, math.inf]
    )
)
ott.assert_almost_equal(
    discrete_chain.computeConditionalPDF(chain_point[1], [chain_point[0]]),
    numerator / denominator,
    1.0e-14,
)
for level in [0.001, 0.5, 0.999]:
    quantile = discrete_chain.computeConditionalQuantile(level, [chain_point[0]])
    assert discrete_chain.computeConditionalCDF(quantile, [chain_point[0]]) >= level
    assert (
        discrete_chain.computeConditionalCDF(quantile - 1.0, [chain_point[0]]) < level
    )
ott.assert_almost_equal(
    discrete_chain.computeConditionalCDF(10.0, [chain_point[0]]), 1.0
)
assert discrete_chain.computeConditionalPDF(10.0, [chain_point[0]]) == 0.0
ott.assert_almost_equal(
    discrete_chain.computeConditionalCDF(chain_point[1], []),
    discrete_chain.getMarginal(0).computeCDF([chain_point[1]]),
    1.0e-14,
)
ott.assert_almost_equal(
    discrete_chain.computeConditionalCDF(chain_point[1], [math.inf]), 0.0
)
ott.assert_almost_equal(
    discrete_chain.computeConditionalQuantile(0.0, [chain_point[0]]),
    discrete_chain.getMarginal(1).computeScalarQuantile(0.0),
)
ott.assert_almost_equal(
    discrete_chain.computeConditionalQuantile(1.0, [chain_point[0]]),
    discrete_chain.getMarginal(1).computeScalarQuantile(1.0),
)
assert discrete_chain.computePDF([0.5, 0.0, 0.0]) == 0.0
assert discrete_chain.computePDF([0.0, 0.5, 0.0]) == 0.0
assert discrete_chain.computePDF([0.0, 1.5, 0.0]) == 0.0

# Independent discrete conditionals use the marginal shortcut.
independent_discrete = ot.CumulativeDistributionNetwork(
    [ot.Poisson(2.0), ot.Poisson(3.0)], ot.BipartiteGraph([[0], [1]])
)
assert independent_discrete.hasIndependentCopula()
ott.assert_almost_equal(
    independent_discrete.computeConditionalCDF(2.0, [1.0]),
    ot.Poisson(3.0).computeCDF(2.0),
    1.0e-14,
)
ott.assert_almost_equal(
    independent_discrete.computeConditionalPDF(2.0, [1.0]),
    ot.Poisson(3.0).computePDF(2.0),
    1.0e-14,
)
ott.assert_almost_equal(
    independent_discrete.computeConditionalQuantile(0.5, [1.0]),
    ot.Poisson(3.0).computeScalarQuantile(0.5),
)

# A discrete triangle exercises the variable-elimination table construction.
discrete_triangle_factors = [ot.Multinomial(1, [0.5, 0.5])] * 3
discrete_triangle = ot.CumulativeDistributionNetwork(
    discrete_triangle_factors,
    ot.BipartiteGraph([[0, 1], [1, 2], [0, 2]]),
)
for triangle_point in [[0.0, 0.0, 0.0], [1.0, 1.0, 1.0], [0.0, 1.0, 1.0]]:
    expected = mixed_difference(
        discrete_triangle_factors,
        [[0, 1], [1, 2], [0, 2]],
        triangle_point,
        [0, 1, 2],
    )
    ott.assert_almost_equal(
        discrete_triangle.computePDF(triangle_point), expected, 1.0e-14
    )

# ---------------------------------------------------------------------------
# MaximumTerms guards
# ---------------------------------------------------------------------------
try:
    ot.ResourceMap.SetAsUnsignedInteger(maximum_terms_key, 4)
    with ott.assert_raises(RuntimeError):
        cyclic.computePDF([0.1, 0.2, 0.3])
    with ott.assert_raises(RuntimeError):
        discrete.computePDF([2.0, 2.0, 4.0])
    with ott.assert_raises(RuntimeError):
        discrete_triangle.computePDF([0.0, 1.0, 1.0])
finally:
    ot.ResourceMap.SetAsUnsignedInteger(maximum_terms_key, original_maximum_terms)

# A tree only needs one free-variable entry per factor, so the guard is
# reached with a unit limit rather than a small positive one.
try:
    ot.ResourceMap.SetAsUnsignedInteger(maximum_terms_key, 1)
    with ott.assert_raises(RuntimeError):
        tree.computePDF([0.2, -0.5, 0.8])
finally:
    ot.ResourceMap.SetAsUnsignedInteger(maximum_terms_key, original_maximum_terms)

# ---------------------------------------------------------------------------
# Dependent sampling uses inversion and reproduces the marginal moments
# ---------------------------------------------------------------------------
sampled = ot.CumulativeDistributionNetwork(
    [correlated_normal(0.5)] * 2, ot.BipartiteGraph([[0, 1], [0, 1]])
)
sampled.setDescription(["x", "y"])
dependent_sample = sampled.getSample(2000)
assert dependent_sample.getSize() == 2000
assert dependent_sample.getDimension() == 2
assert dependent_sample.getDescription() == ["x", "y"]
expected_mean = 1.0 / math.sqrt(math.pi)
expected_variance = 1.0 - 1.0 / math.pi
dependent_mean = dependent_sample.computeMean()
dependent_covariance = dependent_sample.computeCovariance()
assert abs(dependent_mean[0] - expected_mean) < 0.05
assert abs(dependent_mean[1] - expected_mean) < 0.05
assert abs(dependent_covariance[0, 0] - expected_variance) < 0.06
assert abs(dependent_covariance[1, 1] - expected_variance) < 0.06
assert dependent_covariance[0, 1] > 0.2
for _ in range(10):
    dependent_realization = sampled.getRealization()
    assert dependent_realization.getDimension() == 2
    assert all(math.isfinite(value) for value in dependent_realization)

block_sample = ot.CumulativeDistributionNetwork(
    [ot.Normal(), ot.Exponential()], ot.BipartiteGraph([[0], [1]])
)
block_sample.setDescription(["normal", "exponential"])
block_realizations = block_sample.getSample(100)
assert block_realizations.getDescription() == ["normal", "exponential"]
assert block_realizations.getSize() == 100
