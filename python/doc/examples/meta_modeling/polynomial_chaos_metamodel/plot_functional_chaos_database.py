"""
Create a full or sparse polynomial chaos expansion
==================================================
"""
# %%
# In this example we create a global approximation of a model using
# polynomial chaos expansion based on a design of experiment.
# The goal of this example is to show how we can create a full or
# sparse polynomial chaos expansion depending on our needs
# and depending on the number of observations we have.
# In general, we should have more observations than parameters to estimate.
# This is why a sparse polynomial chaos may be interesting:
# by carefully selecting the coefficients we estimate,
# we may reduce overfitting and increase the predictions of the
# metamodel.

# %%
import openturns as ot

ot.Log.Show(ot.Log.NONE)

# %%
# Define the model
# ~~~~~~~~~~~~~~~~

# %%
# Create the function.
myModel = ot.SymbolicFunction(
    ["x1", "x2", "x3", "x4"], ["1 + x1 * x2 + 2 * x3^2 + x4^4"]
)

# %%
# Create a multivariate distribution.
distribution = ot.JointDistribution(
    [ot.Normal(), ot.Uniform(), ot.Gamma(2.75, 1.0), ot.Beta(2.5, 1.0, -1.0, 2.0)]
)

# %%
# In order to create the PCE, we can specify the distribution of the
# input parameters.
# If not known, statistical inference can be used to select a possible
# candidate, and fitting tests can validate such an hypothesis.
# Please read :doc:`Fit a distribution from an input sample </auto_meta_modeling/polynomial_chaos_metamodel/plot_chaos_build_distribution>`
# for an example of this method.

# %%
# Create a training sample
# ~~~~~~~~~~~~~~~~~~~~~~~~

# %%
# Create a pair of input and output samples.
sampleSize = 250
inputSample = distribution.getSample(sampleSize)
outputSample = myModel(inputSample)

# %%
# Build the orthogonal basis
# ~~~~~~~~~~~~~~~~~~~~~~~~~~

# %%
# In the next cell, we create the univariate orthogonal polynomial basis
# for each marginal.
inputDimension = inputSample.getDimension()
coll = [
    ot.StandardDistributionPolynomialFactory(distribution.getMarginal(i))
    for i in range(inputDimension)
]
enumerateFunction = ot.LinearEnumerateFunction(inputDimension)
productBasis = ot.OrthogonalProductPolynomialFactory(coll, enumerateFunction)

# %%
# We can achieve the same result using :class:`~OrthogonalProductPolynomialFactory`.
marginalDistributionCollection = [
    distribution.getMarginal(i) for i in range(inputDimension)
]
multivariateBasis = ot.OrthogonalProductPolynomialFactory(
    marginalDistributionCollection
)
multivariateBasis

# %%
# Create a full PCE
# ~~~~~~~~~~~~~~~~~

# %%
# Create the algorithm.
# We compute the basis size from the total degree.
# The next lines use the :class:`~openturns.LeastSquaresStrategy` class
# with default parameters (the default is the
# :class:`~openturns.PenalizedLeastSquaresAlgorithmFactory` class).
# This creates a full polynomial chaos expansion, i.e.
# we keep all the candidate coefficients produced by the enumeration
# rule.
# In order to create a sparse polynomial chaos expansion, we
# must use the :class:`~openturns.LeastSquaresMetaModelSelectionFactory`
# class instead.
#
totalDegree = 3
candidateBasisSize = enumerateFunction.getBasisSizeFromTotalDegree(totalDegree)
print("Candidate basis size = ", candidateBasisSize)
adaptiveStrategy = ot.FixedStrategy(productBasis, candidateBasisSize)
projectionStrategy = ot.LeastSquaresStrategy()
algo = ot.FunctionalChaosAlgorithm(
    inputSample, outputSample, distribution, adaptiveStrategy, projectionStrategy
)
algo.run()
result = algo.getResult()
result

# %%
# Get the number of coefficients in the PCE.
selectedBasisSizeFull = result.getIndices().getSize()
print("Selected basis size = ", selectedBasisSizeFull)

# %%
# We see that the number of coefficients in the selected basis is
# equal to the number of coefficients in the candidate basis.
# This is, indeed, a *full* PCE.

# %%
# Use the PCE
# ~~~~~~~~~~~

# %%
# Get the metamodel function.
metamodel = result.getMetaModel()

# %%
# In order to evaluate the metamodel on a single point, we just
# use it as any other :class:`openturns.Function`.
xPoint = distribution.getMean()
yPoint = metamodel(xPoint)
print("Value at ", xPoint, " is ", yPoint)

# %%
# Print residuals.
result.getResiduals()

# %%
# Based on these results, we may want to validate our metamodel.
# More details on this topic are presented in
# :doc:`Validate a polynomial chaos </auto_meta_modeling/polynomial_chaos_metamodel/plot_chaos_draw_validation>`.

# %%
# Create a sparse PCE
# ~~~~~~~~~~~~~~~~~~~

# %%
# In order to create a sparse polynomial chaos expansion, we
# use the :class:`~openturns.LeastSquaresMetaModelSelectionFactory`
# class instead.
#
totalDegree = 6
candidateBasisSize = enumerateFunction.getBasisSizeFromTotalDegree(totalDegree)
print("Candidate basis size = ", candidateBasisSize)
adaptiveStrategy = ot.FixedStrategy(productBasis, candidateBasisSize)
selectionAlgorithm = ot.LeastSquaresMetaModelSelectionFactory()
projectionStrategy = ot.LeastSquaresStrategy(selectionAlgorithm)
algo = ot.FunctionalChaosAlgorithm(
    inputSample, outputSample, distribution, adaptiveStrategy, projectionStrategy
)
algo.run()
result = algo.getResult()
result

# %%
# Get the number of coefficients in the PCE.
selectedBasisSizeSparse = result.getIndices().getSize()
print("Selected basis size = ", selectedBasisSizeSparse)

# %%
# We see that the number of selected coefficients is lower than
# the number of candidate coefficients.
# This may reduce overfitting and can produce a PCE with more
# accurate predictions.
