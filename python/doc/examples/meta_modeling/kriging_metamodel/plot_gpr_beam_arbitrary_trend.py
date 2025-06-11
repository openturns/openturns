"""
Gaussian Process Regression: choose an arbitrary trend
======================================================
"""

# %%
# The goal of this example is to show how to configure an arbitrary trend in a Kriging metamodel.
# In the :doc:`/auto_meta_modeling/kriging_metamodel/plot_gpr_choose_trend`
# and :doc:`/auto_meta_modeling/kriging_metamodel/plot_gpr_beam_trend` examples,
# we show how to configure a polynomial trend.
#
# In general, any collection of multivariate functions can be used as the
# `basis` argument of a :class:`~openturns.experimental.GaussianProcessFitter` or a
# :class:`~openturns.experimental.GaussianProcessRegression`.
# In practice, it might not be convenient to create a multivariate basis and
# this is why we sometimes create it by tensorization of univariate functions.
# In this example, we first use Legendre polynomials as our univariate functions,
# then we create an orthogonal polynomial basis corresponding to the input marginals.
#
# For this purpose, we use the :ref:`cantilever beam <use-case-cantilever-beam>` example.
from openturns.usecases import cantilever_beam
import openturns as ot
import openturns.experimental as otexp


# %%
# Definition of the model
# -----------------------
#
# We load the cantilever beam use case
cb = cantilever_beam.CantileverBeam()

# %%
# We load the function (model) which evaluates the output `Y` depending on the inputs.
model = cb.model

# %%
# Then we define the distribution of the input random vector.
dimension = cb.dim  # number of inputs
input_dist = cb.distribution

# %%
# Create the design of experiments
# --------------------------------

# %%
# We consider a simple Monte-Carlo sampling as a design of experiments.
# This is why we generate an input sample using the `getSample` method of the distribution.
# Then we evaluate the output using the `model` function.
sampleSize_train = 20
X_train = input_dist.getSample(sampleSize_train)
Y_train = model(X_train)

# %%
# Create the Legendre basis
# -------------------------
#
# We first create a Legendre basis of univariate polynomials.
#
# The :class:`~openturns.LegendreFactory` class creates Legendre polynomials.
univariateFactory = ot.LegendreFactory()

# %%
# These polynomials are orthogonal with respect to the `Uniform` distribution on :math:`[-1, 1]`, as we can see.
univariateFactory.getMeasure()

# %%
# Even if the measure  `Uniform([-1, 1])` does not correspond to the input marginal distributions,
# these polynomials will, anyway, create a consistent trend for the Gaussian process regression metamodel.
#
# We use the same polynomial family for each input. The multivariate polynomials are created by tensorization of the univariate polynomials.
# The linear enumerate function specifies the order followed to create the multivariate polynomials.
polyColl = [univariateFactory] * dimension
enumerateFunction = ot.LinearEnumerateFunction(dimension)
productBasis = ot.OrthogonalProductPolynomialFactory(polyColl, enumerateFunction)

# %%
# The basis contains the first multivariate polynomials ordered according to the linear enumerate function.
functions = []
numberOfTrendCoefficients = 12
for i in range(numberOfTrendCoefficients):
    multivariatepolynomial = productBasis.build(i)
    print(multivariatepolynomial)
    functions.append(multivariatepolynomial)

# %%
basis = ot.Basis(functions)

# %%
# Create the Gaussian Process Regression metamodel
# ------------------------------------------------
#
# In order to create the  Gaussian process regression  metamodel, we choose the function basis created previously
# and a squared exponential covariance model.
covariance_model = ot.SquaredExponential([1.0] * dimension, [1.0])

# %%
# First, we estimate a Gaussian process approximating the model with the class
# :class:`~openturns.experimental.GaussianProcessFitter`.
algo_fit = otexp.GaussianProcessFitter(X_train, Y_train, covariance_model, basis)
print('First run: algo GPFitter = ', algo_fit.getOptimizationAlgorithm())
algo_fit.setOptimizationAlgorithm(ot.TNC())
algo_fit.run()
fitter_result = algo_fit.getResult()

# %%
# Then, we condition the estimated gaussian process to
# make it interpolate the data set using the class
# :class:`~openturns.experimental.GaussianProcessRegression`.
gpr_algo = otexp.GaussianProcessRegression(fitter_result)
gpr_algo.run()
gpr_result = gpr_algo.getResult()

# %%
# We get the Gaussian process regression metamodel.
gpr_metamodel = gpr_result.getMetaModel()

# %%
# The method :meth:`~openturns.experimental.GaussianProcessRegressionResult.getTrendCoefficients` returns the
# coefficients of the trend. We see that the number of coefficients in the trend corresponds to the number of
# functions in the basis.
print(gpr_result.getTrendCoefficients())

# %%
# We get the updated covariance model. The :class:`~openturns.SquaredExponential` model has one amplitude coefficient
# and 4 scale coefficients. This is because this covariance model is anisotropic : each of the 4 input variables
# is associated with its own scale coefficient.
print(gpr_result.getCovarianceModel())

# %%
# Create an orthogonal multivariate polynomial factory
# ----------------------------------------------------
#
# We suggest here to create a multivariate polynomial basis where each marginal polynomial family corresponds to the polynomial family orthogonal
# to the marginal input distribution. We use the class :class:`~openturns.OrthogonalProductPolynomialFactory` created from the input marginal distributions.
# This corresponds to the basis we usually use in the polynomial chaos expansion.
#
# We first create the mutlivariate polynomial basis as a tensorized product of the univariate polynomial basis orthogonal to the marginal input distributions.
multivariateBasis = ot.OrthogonalProductPolynomialFactory([cb.E, cb.F, cb.L, cb.II])

# %%
# Then we create the multivariate basis which has maximum degree equal to 2. There are 15 functions contained in the basis.
totalDegree = 2
enumerateFunction = multivariateBasis.getEnumerateFunction()
numberOfTrendCoefficients = enumerateFunction.getStrataCumulatedCardinal(totalDegree)
print("Number of functions in the basis: ", numberOfTrendCoefficients)

# %%
functions = []
for i in range(numberOfTrendCoefficients):
    multivariatepolynomial = productBasis.build(i)
    print(multivariatepolynomial)
    functions.append(multivariatepolynomial)

# %%
basis = ot.Basis(functions)

# %%
algo_fit = otexp.GaussianProcessFitter(X_train, Y_train, covariance_model, basis)
print('Second run: algo GPFitter = ', algo_fit.getOptimizationAlgorithm())
algo_fit.setOptimizationAlgorithm(ot.TNC())
algo_fit.run()
fitter_result = algo_fit.getResult()
gpr_algo = otexp.GaussianProcessRegression(fitter_result)
gpr_algo.run()
gpr_result = gpr_algo.getResult()
print(gpr_result.getTrendCoefficients())

# %%
# Conclusion
# ----------
#
# The trend that we have configured corresponds to the basis that we would have used in a full polynomial chaos expansioncomputed with least squares.
#
# Other extensions of this work would be:
#
# * to use a Fourier basis with :class:`~openturns.FourierSeriesFactory`,
# * wavelets with :class:`~openturns.HaarWaveletFactory`,
#
# or any other univariate factory.
