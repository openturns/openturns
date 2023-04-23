"""
Advanced polynomial chaos construction
======================================
"""
# %%
# In this example we are going to expose advanced elements in the construction of a polynomial chaos algorithm:
#
# - construction of the multivariate orthonormal basis,
# - truncature strategy of the multivariate orthonormal basis,
# - evaluation strategy of the approximation coefficients.

# %%
# In this example, we consider the following function :math:`\mathbb{R}^4 \rightarrow \mathbb{R}`:
#
# .. math::
#    g(\mathbf{x}) = 1+x_1 x_2 + 2 x_3^2+x_4^4
#
#
# for any :math:`x_1,x_2,x_3,x_4\in\mathbb{R}`.
#
# We assume that the inputs have normal, uniform, gamma and beta distributions :
#
# .. math::
#    X_1 \sim \mathcal{N}(0,1), \qquad X_2 \sim \mathcal{U}(-1,1), \qquad X_3 \sim \mathcal{G}(2.75,1), \qquad X_4 \sim \mathcal{B}(2.5,1,-1,2),
#
#
# and :math:`X_1`, :math:`X_2`, :math:`X_3` and :math:`X_4` are independent.

# %%
# Define the model and the input distribution
# -------------------------------------------

# %%
import openturns as ot

ot.Log.Show(ot.Log.NONE)

# %%
model = ot.SymbolicFunction(["x1", "x2", "x3", "x4"], ["1+x1*x2 + 2*x3^2+x4^4"])

# %%
# Create a distribution of dimension 4.

# %%
distribution = ot.ComposedDistribution(
    [ot.Normal(), ot.Uniform(), ot.Gamma(2.75, 1.0), ot.Beta(2.5, 1.0, -1.0, 2.0)]
)

# %%
inputDimension = distribution.getDimension()
inputDimension

# %%
# STEP 1: Construction of the multivariate orthonormal basis
# ----------------------------------------------------------

# %%
# Create the univariate polynomial family collection which regroups the polynomial families for each direction.

# %%
polyColl = ot.PolynomialFamilyCollection(inputDimension)

# %%
# We could use the Krawtchouk and Charlier families (for discrete distributions).

# %%
polyColl[0] = ot.KrawtchoukFactory()
polyColl[1] = ot.CharlierFactory()

# %%
# We could also use the automatic selection of the polynomial which corresponds to the distribution: this is done with the `StandardDistributionPolynomialFactory` class.

# %%
for i in range(inputDimension):
    marginal = distribution.getMarginal(i)
    polyColl[i] = ot.StandardDistributionPolynomialFactory(marginal)

# %%
# In our specific case, we use specific polynomial factories.

# %%
polyColl[0] = ot.HermiteFactory()
polyColl[1] = ot.LegendreFactory()
polyColl[2] = ot.LaguerreFactory(2.75)
# Parameter for the Jacobi factory : 'Probabilty' encoded with 1
polyColl[3] = ot.JacobiFactory(2.5, 3.5, 1)

# %%
# Create the enumeration function.

# %%
# The first possibility is to use the `LinearEnumerateFunction`.

# %%
enumerateFunction = ot.LinearEnumerateFunction(inputDimension)
multivariateBasis = ot.OrthogonalProductPolynomialFactory(polyColl, enumerateFunction)
multivariateBasis

# %%
# Another possibility is to use the `HyperbolicAnisotropicEnumerateFunction`, which gives less weight to interactions.

# %%
q = 0.4
enumerateFunction = ot.HyperbolicAnisotropicEnumerateFunction(inputDimension, q)

# %%
# Create the multivariate orthonormal basis which is the cartesian product of the univariate basis.

# %%
multivariateBasis = ot.OrthogonalProductPolynomialFactory(polyColl, enumerateFunction)
multivariateBasis

# %%
# Ask how many basis terms there are in the 6-th strata.
# In the special case of the Linear enumerate function this is also the strata with all the multi-indices of total degree 5.
k = 5
enumerateFunction.getStrataCardinal(k)

# %%
# Ask how many basis multi-indices have total degrees lower or equal to k=5.
enumerateFunction.getBasisSizeFromTotalDegree(k)

# %%
# Give the k-th term of the multivariate basis. To calculate its degree, add the integers.
k = 5
enumerateFunction(k)

# %%
# Build a term of the basis as a Function. Generally, we do not need to construct manually any term, all terms are constructed automatically by a strategy of construction of the basis.

# %%
i = 5
Psi_i = multivariateBasis.build(i)
Psi_i

# %%
# Get the measure mu associated to the multivariate basis.

# %%
distributionStandard = multivariateBasis.getMeasure()
distributionStandard

# %%
# STEP 2: Truncature strategy of the multivariate orthonormal basis
# -----------------------------------------------------------------

# %%
# FixedStrategy : all the polynomials af degree lower or equal to 2 which corresponds to the 15 first ones.

# %%
p = 15
truncatureBasisStrategy = ot.FixedStrategy(multivariateBasis, p)

# %%
# CleaningStrategy : among the maximumConsideredTerms = 500 first polynomials, those which have the mostSignificant = 50 most significant contributions with significance criterion significanceFactor equal to :math:`10^{-4}`
# The `True` boolean indicates if we are interested in the online monitoring of the current basis update (removed or added coefficients).

# %%
maximumConsideredTerms = 500
mostSignificant = 50
significanceFactor = 1.0e-4
truncatureBasisStrategy_2 = ot.CleaningStrategy(
    multivariateBasis, maximumConsideredTerms, mostSignificant, significanceFactor, True
)

# %%
# STEP 3: Evaluation strategy of the approximation coefficients
# -------------------------------------------------------------

# %%
# The technique illustrated is the Least Squares technique where the points come from an design of experiments. Here : the Monte Carlo sampling technique.

# %%
sampleSize = 100
evaluationCoeffStrategy = ot.LeastSquaresStrategy()
experiment = ot.MonteCarloExperiment(distribution, sampleSize)

# %%
# You can specify the approximation algorithm. This is the algorithm that generates a sequence of basis using Least Angle Regression.

# %%
basisSequenceFactory = ot.LARS()

# %%
# This algorithm estimates the empirical error on each sub-basis using Leave One Out strategy.

# %%
fittingAlgorithm = ot.CorrectedLeaveOneOut()
# Finally the metamodel selection algorithm embbeded in LeastSquaresStrategy
approximationAlgorithm = ot.LeastSquaresMetaModelSelectionFactory(
    basisSequenceFactory, fittingAlgorithm
)
evaluationCoeffStrategy_2 = ot.LeastSquaresStrategy(approximationAlgorithm)
experiment_2 = experiment

# %%
# Try integration.
marginalSizes = [2] * inputDimension
evaluationCoeffStrategy_3 = ot.IntegrationStrategy()
experiment_3 = ot.GaussProductExperiment(distribution, marginalSizes)

# %%
# Evaluate design of experiments.
# For the Gauss product we need to specify the non-uniform weights.
X, W = experiment.generateWithWeights()
Y = model(X)

# %%
# STEP 4: Creation of the Functional Chaos Algorithm
# --------------------------------------------------
#
# The `FunctionalChaosAlgorithm` class combines
#
# * the model : `model`
# * the distribution of the input random vector : `distribution`
# * the truncature strategy of the multivariate basis
# * and the evaluation strategy of the coefficients

# %%
polynomialChaosAlgorithm = ot.FunctionalChaosAlgorithm(
    X, W, Y, distribution, truncatureBasisStrategy, evaluationCoeffStrategy
)
