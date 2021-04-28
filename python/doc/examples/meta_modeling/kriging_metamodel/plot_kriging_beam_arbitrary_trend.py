"""
Configuring an arbitrary trend in Kriging
=========================================
"""
# %%
# The goal of this example is to show how to configure an arbitrary trend in a Kriging metamodel. 
#
# In general, any collection of multivariate functions can be used as the `basis` argument of a `KrigingAlgorithm`. In practice, it might not be convenient to create a multivariate basis and this is why we sometimes create it by tensorization of univariate functions. In this example, we first use Legendre polynomials as our univariate functions, then we create an orthogonal polynomial basis corresponding to the input marginals.
#
# For this purpose, we use the :ref:`cantilever beam <use-case-cantilever-beam>` example.

# %%
# Definition of the model
# -----------------------

# %%
import openturns as ot
ot.RandomGenerator.SetSeed(0)
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# We load the cantilever beam use case
from openturns.usecases import cantilever_beam as cantilever_beam
cb = cantilever_beam.CantileverBeam()

# %%
# We load the function (model) which evaluates the output Y depending on the inputs.
model = cb.model

# %%
# Then we define the distribution of the input random vector. 
dimension = cb.dim # number of inputs
myDistribution = cb.distribution

# %%
# Create the design of experiments
# --------------------------------

# %%
# We consider a simple Monte-Carlo sampling as a design of experiments. This is why we generate an input sample using the `getSample` method of the distribution. Then we evaluate the output using the `model` function.

# %%
sampleSize_train = 20
X_train = myDistribution.getSample(sampleSize_train)
Y_train = model(X_train)

# %%
# Create the Legendre basis
# -------------------------
#
# We first create a Legendre basis of univariate polynomials. In order to convert then into multivariate polynomials, we use a linear enumerate function.
#
# The `LegendreFactory` class creates Legendre polynomials. 

# %%
univariateFactory = ot.LegendreFactory()

# %%
# This factory corresponds to the `Uniform` distribution in the [-1,1] interval. 

# %%
univariateFactory.getMeasure()

# %%
# This interval does not correspond to the interval on which the input marginals are defined (we will come back to this topic later), but this will, anyway, create a consistent trend for the kriging.

# %%
polyColl = [univariateFactory]*dimension

# %%
enumerateFunction = ot.LinearEnumerateFunction(dimension)
productBasis = ot.OrthogonalProductPolynomialFactory(polyColl, enumerateFunction)

# %%
functions = []
numberOfTrendCoefficients = 12
for i in range(numberOfTrendCoefficients):
    multivariatepolynomial = productBasis.build(i)
    print(multivariatepolynomial)
    functions.append(multivariatepolynomial)

# %%
basis = ot.Basis(functions)

# %%
# Create the metamodel
# --------------------

# %%
# In order to create the kriging metamodel, we first select a constant trend with the `ConstantBasisFactory` class. Then we use a squared exponential covariance model. Finally, we use the `KrigingAlgorithm` class to create the kriging metamodel, taking the training sample, the covariance model and the trend basis as input arguments. 

# %%
covarianceModel = ot.SquaredExponential(X_train.getMax(), [1.0])

# %%
# We need to manually define sensible optimization bounds.
# Note that since the amplitude parameter is computed analytically (this is possible when the output dimension is 1), we only need to set bounds on the scale parameter.

# %%
scaleOptimizationBounds = ot.Interval([1.0, 1.0, 1.0, 1.0e-10], [1.0e11, 1.0e3, 1.0e1, 1.0e-5])

# %%
algo = ot.KrigingAlgorithm(X_train, Y_train, covarianceModel, basis)
algo.setOptimizationBounds(scaleOptimizationBounds)
algo.run()
result = algo.getResult()
krigingWithConstantTrend = result.getMetaModel()

# %%
# The `getTrendCoefficients` method returns the coefficients of the trend.

# %%
result.getTrendCoefficients()

# %%
# We see that the number of coefficients in the trend corresponds to the number of functions in the basis. 

# %%
result.getCovarianceModel()

# %%
# The `SquaredExponential` model has one amplitude coefficient and 4 scale coefficients. This is because this covariance model is anisotropic : each of the 4 input variables is associated with its own scale coefficient. 

# %%
# Create an orthogonal multivariate polynomial factory
# ----------------------------------------------------

# %%
# In order to create a Legendre basis which better corresponds to the input marginals, we could consider the orthogonal basis which would be associated to uniform marginals. To compute the bounds of these uniform distributions, we may consider the 1% and 99% quantiles of each marginal.
#
# There is, however, a simpler way to proceed. We can simply orthogonalize the input marginals and create the orthogonal polynomial basis corresponding to the inputs. This corresponds to the method we would use in the polynomial chaos. 
#
# We first create the polynomial basis which corresponds to the inputs. 

# %%
multivariateBasis = ot.OrthogonalProductPolynomialFactory([cb.E, cb.F, cb.L, cb.I])

# %%
# Then we create the multivariate basis which has maximum degree equal to 2.

# %%
totalDegree = 2
enumerateFunction = multivariateBasis.getEnumerateFunction()
numberOfTrendCoefficients = enumerateFunction.getStrataCumulatedCardinal(totalDegree)
numberOfTrendCoefficients

# %%
functions = []
for i in range(numberOfTrendCoefficients):
    multivariatepolynomial = productBasis.build(i)
    print(multivariatepolynomial)
    functions.append(multivariatepolynomial)

# %%
basis = ot.Basis(functions)

# %%
algo = ot.KrigingAlgorithm(X_train, Y_train, covarianceModel, basis)
algo.setOptimizationBounds(scaleOptimizationBounds)
algo.run()
result = algo.getResult()
krigingWithConstantTrend = result.getMetaModel()

# %%
# The `getTrendCoefficients` method returns the coefficients of the trend.

# %%
result.getTrendCoefficients()

# %%
# Conclusion
# ----------
#
# The trend that we have configured corresponds to the basis that we would have used in a full polynomial chaos computed with least squares. 
#
# Other extensions of this work would be:
#
# * to use a Fourier basis with `FourierSeriesFactory`,
# * wavelets with `HaarWaveletFactory`,
#
# or any other univariate factory. 
