#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.ResourceMap.SetAsUnsignedInteger(
    "LinearCombinationDistribution-DefaultMaxSize", 4000000
)


def checkMixture(distribution, atoms, weights, constant, classNames):
    """Check a simplification result against the expected atom structure and
    the exact moments of the requested linear combination."""
    coll = distribution.getDistributionCollection()
    assert len(coll) == len(classNames), "wrong number of atoms"
    for i in range(len(coll)):
        assert (
            coll[i].getImplementation().getClassName() == classNames[i]
        ), "wrong atom class at position %d" % i
    # The moments of the combination are exact whatever the simplifications
    mean = constant
    variance = 0.0
    for i in range(len(atoms)):
        mean += weights[i] * atoms[i].getMean()[0]
        variance += weights[i] ** 2 * atoms[i].getCovariance()[0, 0]
    ott.assert_almost_equal(distribution.getMean(), [mean], 1e-9, 1e-9, "mean")
    ott.assert_almost_equal(
        distribution.getCovariance()[0, 0], variance, 1e-9, 1e-9, "covariance"
    )


def checkWeights(distribution, weights):
    """Check that the weight matrix of the distribution matches the expected values."""
    got = distribution.getWeights()
    assert got.getNbRows() == 1, "wrong weight matrix shape"
    assert got.getNbColumns() == len(weights), "wrong number of weights"
    for i in range(len(weights)):
        ott.assert_almost_equal(got[0, i], weights[i], 1e-12, 1e-12, "weight")


# Test fusion of Dirac with no other atoms: should be a unique Dirac(5.8)
atoms = [ot.Dirac(1.0), ot.Dirac(2.0), ot.Dirac(3.0)]
d = ot.LinearCombinationDistribution(atoms, [0.5, 0.6, 0.7], 2.0)
checkMixture(d, atoms, [0.5, 0.6, 0.7], 2.0, ["Dirac"])
ott.assert_almost_equal(d.getDistributionCollection()[0].getParameter()[0], 5.8)

# Test fusion of Dirac with other atoms: the Dirac should be merged into
# the constant: 5 + Exponential(lambda=1,gamma=0)
atoms = [ot.Dirac(1.0), ot.Dirac(2.0), ot.Exponential()]
d = ot.LinearCombinationDistribution(atoms, 2.0)
checkMixture(d, atoms, [1.0, 1.0, 1.0], 2.0, ["Exponential"])
ott.assert_almost_equal(d.getConstant(), [5.0])

# Test flatten LinearCombinationDistribution atoms: the LinearCombinationDistribution should have 4 atoms.
atoms = [
    ot.Gumbel(),
    ot.LinearCombinationDistribution([ot.Logistic(), ot.WeibullMin()], [0.5, 1.5], 3.0),
    ot.Frechet(1.0, 4.0),
]
weights = [2.0, 3.0, 4.0]
d = ot.LinearCombinationDistribution(atoms, weights, 2.0)
checkMixture(
    d,
    atoms,
    weights,
    2.0,
    ["Gumbel", "Logistic", "WeibullMin", "Frechet"],
)
ott.assert_almost_equal(d.getConstant(), [11.0])
checkWeights(d, [2.0, 1.5, 4.5, 4.0])

# Test merge of Normal atoms:
atoms = [ot.Normal(1.0, 8.0), ot.Logistic(), ot.Normal(2.0, 1.0)]
d = ot.LinearCombinationDistribution(atoms, [0.5, 2.5, 3.0], 2.0)
checkMixture(d, atoms, [0.5, 2.5, 3.0], 2.0, ["Logistic", "Normal"])
# The Normal atom absorbs the constant and the aggregated normal part
coll = d.getDistributionCollection()
ott.assert_almost_equal(
    coll[1].getParameter()[0], 8.5, 1e-10, 1e-12, "wrong merged Normal mean"
)
ott.assert_almost_equal(
    coll[1].getParameter()[1], 5.0, 1e-10, 1e-12, "wrong merged Normal sigma"
)
checkWeights(d, [2.5, 1.0])

# Test merge of Exponential, Gamma and ChiSquare atoms
atoms = [
    ot.Exponential(1.0, 1.0),
    ot.Exponential(1.5, -1.0),
    ot.Exponential(2.0, 1.0),
    ot.Gamma(4.0, 2.0, -1.0),
    ot.Gamma(3.0, 1.0, 3.0),
    ot.ChiSquare(4.0),
]
weights = [1.0, 1.5, 1.0, 2.0, 2.0, 0.5]
d = ot.LinearCombinationDistribution(atoms, weights, 2.0)
checkMixture(d, atoms, weights, 2.0, ["Gamma", "Gamma", "Exponential"])

# Test merge of Uniform atoms
atoms = [
    ot.Uniform(0.0, 1.0),
    ot.Uniform(0.0, 1.0),
    ot.Uniform(1.0, 3.0),
    ot.Uniform(-1.0, 4.0),
    ot.Uniform(2.0, 3.0),
]
weights = [1.0, 1.0, 2.0, 3.0, 4.0]
d = ot.LinearCombinationDistribution(atoms, weights, 2.0)
checkMixture(
    d, atoms, weights, 2.0, ["Triangular", "Trapezoidal", "Uniform"]
)

# Test merge of Bernoulli and Binomial atoms
# Deactivate the fusion of discrete atoms
ot.ResourceMap.SetAsUnsignedInteger("LinearCombinationDistribution-MaximumSupportSize", 0)
atoms = [
    ot.Bernoulli(0.5),
    ot.Bernoulli(0.5),
    ot.Bernoulli(0.1),
    ot.Binomial(4, 0.5),
    ot.Binomial(6, 0.5),
    ot.Binomial(3, 0.1),
]
weights = [1.0, 1.5, 2.0, 1.0, 4.0, 2.0]
d = ot.LinearCombinationDistribution(atoms, weights, 2.0)
checkMixture(
    d,
    atoms,
    weights,
    2.0,
    ["Binomial", "Binomial", "Bernoulli", "Binomial"],
)

# Test merge of Poisson atoms
# Deactivate the fusion of discrete atoms
ot.ResourceMap.SetAsUnsignedInteger("LinearCombinationDistribution-MaximumSupportSize", 0)
atoms = [ot.Poisson(3.0), ot.Poisson(2.0), ot.Poisson(6.0), ot.Poisson(10.0), ot.Poisson(4.0)]
weights = [1.0, 2.0, 3.0, 2.0, 1.0]
d = ot.LinearCombinationDistribution(atoms, weights, 2.0)
checkMixture(
    d, atoms, weights, 2.0, ["Poisson", "Poisson", "Poisson"]
)

# Test fusion of opposite-weight Poisson atoms into a Skellam atom weighted
# by the absolute value of their weights
ot.ResourceMap.SetAsUnsignedInteger("LinearCombinationDistribution-MaximumSupportSize", 10000)
# identical parameters
atoms = [ot.Poisson(2.0), ot.Poisson(2.0)]
d = ot.LinearCombinationDistribution(atoms, [2.5, -2.5], 2.0)
checkMixture(d, atoms, [2.5, -2.5], 2.0, ["Skellam"])
checkWeights(d, [2.5])
ott.assert_almost_equal(
    d.getDistributionCollection()[0].getParameter(), [2.0, 2.0], 0.0, 0.0
)
# different parameters
atoms = [ot.Poisson(2.0), ot.Poisson(3.0)]
d = ot.LinearCombinationDistribution(atoms, [1.5, -1.5], 2.0)
checkMixture(d, atoms, [1.5, -1.5], 2.0, ["Skellam"])
checkWeights(d, [1.5])
ott.assert_almost_equal(
    d.getDistributionCollection()[0].getParameter(), [2.0, 3.0], 0.0, 0.0
)

# Test that discrete and continuous atoms are kept as they are: the
# continuous/discrete pairing into a Mixture has been removed, and the
# generic evaluation on the raw atoms gives the same values.
# Note that a leftover Uniform atom absorbs its weight into its bounds,
# hence the unit weights below.
ot.ResourceMap.SetAsUnsignedInteger("LinearCombinationDistribution-MaximumSupportSize", 10000)
# more continuous atoms
atoms = [ot.Logistic(), ot.Binomial(2, 0.5), ot.Uniform()]
d = ot.LinearCombinationDistribution(atoms, [1.0, 2.0, 3.0], 2.0)
checkMixture(d, atoms, [1.0, 2.0, 3.0], 2.0, ["Logistic", "Uniform", "Binomial"])
checkWeights(d, [1.0, 1.0, 2.0])
# the leftover uniform absorbs its weight and the constant into its bounds:
# U(a, b) * w + c -> U(c + w * a, c + w * b)
uni = atoms[2]
lo = uni.getRange().getLowerBound()[0]
hi = uni.getRange().getUpperBound()[0]
rng = d.getDistributionCollection()[1].getRange()
ott.assert_almost_equal(
    rng.getLowerBound()[0],
    2.0 + 3.0 * lo,
    0.0,
    0.0,
    "uniform lower bound",
)
ott.assert_almost_equal(
    rng.getUpperBound()[0],
    2.0 + 3.0 * hi,
    0.0,
    0.0,
    "uniform upper bound",
)
# more discrete atoms
atoms = [ot.Bernoulli(0.1), ot.Binomial(2, 0.5), ot.Uniform()]
d = ot.LinearCombinationDistribution(atoms, [1.0, 2.0, 3.0], 2.0)
checkMixture(d, atoms, [1.0, 2.0, 3.0], 2.0, ["Uniform", "FiniteDiscreteDistribution"])
checkWeights(d, [1.0, 1.0])
# same number of continuous and discrete atoms
atoms = [ot.Logistic(), ot.Bernoulli(0.1), ot.Binomial(2, 0.5), ot.Uniform()]
d = ot.LinearCombinationDistribution(atoms, [1.0, 2.0, 3.0, 4.0], 2.0)
checkMixture(
    d, atoms, [1.0, 2.0, 3.0, 4.0], 2.0,
    ["Logistic", "Uniform", "FiniteDiscreteDistribution"],
)
checkWeights(d, [1.0, 1.0, 1.0])
# The simplification does not change the distribution: check the agreement
# of the evaluations with the unfused collection
ot.ResourceMap.SetAsBool("LinearCombinationDistribution-SimplifyAtoms", False)
raw = ot.LinearCombinationDistribution(atoms, [1.0, 2.0, 3.0, 4.0], 2.0)
ot.ResourceMap.SetAsBool("LinearCombinationDistribution-SimplifyAtoms", True)
x = d.getMean()[0]
for offset in [-1.5, -0.5, 0.0, 0.5, 1.5]:
    xx = x + offset * d.getStandardDeviation()[0]
    ott.assert_almost_equal(d.computePDF(xx), raw.computePDF(xx), 1e-8, 1e-12, "pdf")
    ott.assert_almost_equal(d.computeCDF(xx), raw.computeCDF(xx), 1e-10, 1e-12, "cdf")

# Test the fusion of discrete atoms
# All the atoms have a too large support
ot.ResourceMap.SetAsUnsignedInteger("LinearCombinationDistribution-MaximumSupportSize", 1)
atoms = [ot.Binomial(2, 0.1), ot.Binomial(3, 0.5), ot.Poisson(), ot.Geometric()]
weights = [1.0, 2.0, 3.0, 4.0]
d = ot.LinearCombinationDistribution(atoms, weights, 2.0)
checkMixture(
    d,
    atoms,
    weights,
    2.0,
    ["Geometric", "Poisson", "Binomial", "Binomial"],
)

# Some atoms have a too large support, no pending aggregated discrete
ot.ResourceMap.SetAsUnsignedInteger("LinearCombinationDistribution-MaximumSupportSize", 15)
atoms = [ot.Binomial(2, 0.1), ot.Binomial(3, 0.5), ot.Poisson(), ot.Geometric()]
weights = [1.0, 2.0, 3.0, 4.0]
d = ot.LinearCombinationDistribution(atoms, weights, 2.0)
checkMixture(
    d,
    atoms,
    weights,
    2.0,
    ["Geometric", "Poisson", "FiniteDiscreteDistribution"],
)

# Some atoms have a too large support, a pending aggregated discrete
ot.ResourceMap.SetAsUnsignedInteger("LinearCombinationDistribution-MaximumSupportSize", 15)
atoms = [
    ot.Binomial(2, 0.1),
    ot.Binomial(3, 0.5),
    ot.Poisson(),
    ot.Binomial(2, 0.1),
    ot.Binomial(3, 0.5),
]
weights = [1.0, 2.0, 3.0, 4.0, 5.0]
d = ot.LinearCombinationDistribution(atoms, weights, 2.0)
checkMixture(
    d,
    atoms,
    weights,
    2.0,
    ["Poisson", "FiniteDiscreteDistribution", "Binomial", "Binomial"],
)

# All the atoms can be merged
ot.ResourceMap.SetAsUnsignedInteger("LinearCombinationDistribution-MaximumSupportSize", 100)
atoms = [ot.Bernoulli(0.1), ot.Bernoulli(0.2), ot.Bernoulli(0.3), ot.Bernoulli(0.4)]
weights = [1.0, 2.0, 3.0, 4.0]
d = ot.LinearCombinationDistribution(atoms, weights, 2.0)
checkMixture(d, atoms, weights, 2.0, ["FiniteDiscreteDistribution"])
