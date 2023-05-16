"""
Sample manipulation
===================
"""
# %%
# This example will describe the main statistical functionalities on data through the Sample object. The Sample is an output variable of interest.

# %%
import openturns as ot

ot.Log.Show(ot.Log.NONE)

# %%
# A typical example
# -----------------

# %%
# A recurring issue in uncertainty quantification is to perform analysis on an output variable of interest Y obtained through a model `f` and input parameters `X`.
# Here we shall consider the input parameters as two independent standard normal distributions :math:`X=(X_1, X_2)`.
# We therefore use an `IndependentCopula` to describe the link between the two marginals.
#

# input parameters
inputDist = ot.ComposedDistribution([ot.Normal()] * 2, ot.IndependentCopula(2))
inputDist.setDescription(["X1", "X2"])

# %%
# We create a vector from the 2D-distribution created before :

# %%
inputVector = ot.RandomVector(inputDist)


# %%
# Suppose our model `f` is known and reads as :
#
# .. math::
#    f(X) = \begin{pmatrix}
#             x_1^2 + x_2 \\
#             x_1   + x_2^2
#           \end{pmatrix}
#
# We define our model `f` with a `SymbolicFunction`

# %%
f = ot.SymbolicFunction(["x1", "x2"], ["x1^2+x2", "x2^2+x1"])

# %%
# Our output vector is Y=f(X), the image of the inputVector by the model
outputVector = ot.CompositeRandomVector(f, inputVector)

# %%
# We can now get a sample out of Y, that is realizations (here 1000) of the random outputVector
size = 1000
sample = outputVector.getSample(size)

# %%
# The `sample` may be seen as a matrix of size :math:`1000 \times 2`. We print the 5 first samples (out of 1000) :

# %%
sample[:5]


# %%
# Basic operations on samples
# ---------------------------
# We have access to basic information about a sample such as
#
# - minimum and maximum per component
#
sample.getMin(), sample.getMax()

# %%
# - the range per component (max-min)
#
sample.computeRange()

# %%
# More elaborate functionalities are also available :
#

# %%
# - get the median per component
#
sample.computeMedian()

# %%
# - compute the covariance
#
sample.computeCovariance()

# %%
# - get the empirical 0.95 quantile per component
#
sample.computeQuantilePerComponent(0.95)

# %%
# - get the value of the empirical CDF at a point
#
point = [1.1, 2.2]
sample.computeEmpiricalCDF(point)


# %%
# Estimate the statistical moments
# --------------------------------

# %%
# Oftentimes, we need to estimate the first moments of the output data. We can then estimate statistical moments from the output sample :

# %%
# - estimate the moment of order 1 : mean
#
sample.computeMean()

# %%
# - estimate the standard deviation for each component
#
sample.computeStandardDeviation()

# %%
# - estimate the moment of order 2 : variance
#
sample.computeVariance()

# %%
# - estimate the moment of order 3 : skewness
#
sample.computeSkewness()

# %%
# - estimate the moment of order 4 : kurtosis
#
sample.computeKurtosis()

# %%
# Test the correlation
# --------------------

# %%
# Some statistical test for correlation are available :
#
# - get the sample Pearson correlation matrix :
#
sample.computePearsonCorrelation()

# %%
# - get  the sample Kendall correlation matrix :
#
sample.computeKendallTau()

# %%
#  - get  the sample Spearman correlation matrix :
#
sample.computeSpearmanCorrelation()
