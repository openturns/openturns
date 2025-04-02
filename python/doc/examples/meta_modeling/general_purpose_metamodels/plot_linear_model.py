"""
Create a linear model
=====================
In this example we create a metamodel model using a linear model approximation
with the :class:`~openturns.LinearModelAlgorithm` class.
We show how the :class:`~openturns.LinearModelAnalysis` class
can be used to produce the statistical analysis of the least squares
regression model.
"""

# %%
# Introduction
# ~~~~~~~~~~~~
#
# The following 2-d function is used in this example:
#
# .. math::
#
#     \model(x,y) = 3 + 2x - y
#
# for any :math:`x, y \in \Rset`.
#
# Notice that this model is linear:
#
# .. math::
#
#     \model(x,y) = \beta_1 + \beta_2 x + \beta_3 y
#
# where :math:`\beta_1 = 3`, :math:`\beta_2 = 2` and :math:`\beta_3 = -1`.
#
# We consider noisy observations of the output:
#
# .. math::
#
#     Y = \model(x,y) + \epsilon
#
# where :math:`\epsilon \sim \cN(0, \sigma^2)` with :math:`\sigma > 0`
# is the standard deviation.
# In our example, we use :math:`\sigma = 0.1`.
#
# Finally, we use :math:`n = 1000` independent observations of the model.
#

# %%
import openturns as ot
import openturns.viewer as otv

# %%
# Simulate the data set
# ~~~~~~~~~~~~~~~~~~~~~
#
# We first generate the data and we add noise to the output observations:

# %%
ot.RandomGenerator.SetSeed(0)
distribution = ot.Normal(2)
distribution.setDescription(["x", "y"])
sampleSize = 1000
func = ot.SymbolicFunction(["x", "y"], ["3 + 2 * x - y"])
input_sample = distribution.getSample(sampleSize)
epsilon = ot.Normal(0, 0.1).getSample(sampleSize)
output_sample = func(input_sample) + epsilon

# %%
# Linear regression
# ~~~~~~~~~~~~~~~~~
#
# Let us run the linear model algorithm using the :class:`~openturns.LinearModelAlgorithm`
# class and get the associated results:

# %%
algo = ot.LinearModelAlgorithm(input_sample, output_sample)
result = algo.getResult()

# %%
# Residuals analysis
# ~~~~~~~~~~~~~~~~~~
#
# We can now analyze the residuals of the regression on the training data.
# For clarity purposes, only the first 5 residual values are printed.

# %%
residuals = result.getSampleResiduals()
residuals[:5]

# %%
# Alternatively, the standardized residuals can be used:

# %%
stdresiduals = result.getStandardizedResiduals()
stdresiduals[:5]

# %%
# We can also get the noise distribution wich is assumed to be gaussian:

# %%
result.getNoiseDistribution()


# %%
# Analysis of the results
# ~~~~~~~~~~~~~~~~~~~~~~~
#
# In order to post-process the linear regression results, the :class:`~openturns.LinearModelAnalysis`
# class can be used:

# %%
analysis = ot.LinearModelAnalysis(result)
analysis

# %%
# The results seem to indicate that the linear model is satisfactory.
#
# - The basis uses the three functions :math:`1` (which is called the intercept),
#   :math:`x` and :math:`y`.
# - Each row of the table of coefficients tests if one single coefficient is zero.
#   The probability of observing a large value of the T statistics is close to
#   zero for all coefficients.
#   Hence, we can reject the hypothesis that any coefficient is zero.
#   In other words, all the coefficients are significantly nonzero.
# - The :math:`R^2` score is close to 1.
#   Furthermore, the adjusted :math:`R^2` value, which takes into account the size of
#   the data set and the number of hyperparameters, is similar to the
#   unadjusted :math:`R^2` score.
#   Most of the variance is explained by the linear model.
# - The F-test tests if all coefficients are simultaneously zero.
#   The `Fisher-Snedecor` p-value is lower than 1%.
#   Hence, there is at least one nonzero coefficient.
# - The normality test checks that the residuals have a normal distribution.
#   The normality assumption can be rejected if the p-value is close to zero.
#   The p-values are larger than 0.05: the normality assumption of the
#   residuals is not rejected.
#

# %%
# Graphical analyses
# ~~~~~~~~~~~~~~~~~~
#
# Let us compare model and fitted values:

# %%
graph = analysis.drawModelVsFitted()
view = otv.View(graph)

# %%
# The previous figure seems to indicate that the linearity hypothesis
# is accurate.

# %%
# Residuals can be plotted against the fitted values.

# %%
graph = analysis.drawResidualsVsFitted()
view = otv.View(graph)

# %%
graph = analysis.drawScaleLocation()
view = otv.View(graph)

# %%
graph = analysis.drawQQplot()
view = otv.View(graph)

# %%
# In this case, the two distributions are very close: there is no obvious
# outlier.
#
# Cook's distance measures the impact of every individual data point on the
# linear regression, and can be plotted as follows:

# %%
graph = analysis.drawCookDistance()
view = otv.View(graph)

# %%
# This graph shows us the index of the points with disproportionate influence.
#
# One of the components of the computation of Cook's distance at a given
# point is that point's *leverage*.
# High-leverage points are far from their closest neighbors, so the fitted
# linear regression model must pass close to them.

# sphinx_gallery_thumbnail_number = 6
graph = analysis.drawResidualsVsLeverages()
view = otv.View(graph)

# %%
# In this case, there seems to be no obvious influential outlier characterized
# by large leverage and residual values.
#
# Similarly, we can also plot Cook's distances as a function of the sample
# leverages:

# %%
graph = analysis.drawCookVsLeverages()
view = otv.View(graph)

# %%
# Finally, we give the intervals for each estimated coefficient (95% confidence
# interval):

# %%
alpha = 0.95
interval = analysis.getCoefficientsConfidenceInterval(alpha)
print("confidence intervals with level=%1.2f: " % (alpha))
print("%s" % (interval))


# %%
otv.View.ShowAll()
