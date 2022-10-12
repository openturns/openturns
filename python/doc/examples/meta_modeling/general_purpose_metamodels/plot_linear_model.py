"""
Create a linear model
=====================
In this example we create a surrogate model using linear model approximation.
"""
# %%
# The following 2-dimensional function is used in this example
# :math:`h(x,y) = 2x - y + 3 + 0.05 \sin(0.8x)`.
#

# %%
import openturns as ot
import openturns.viewer as viewer

# %%
# Generation of the data set
# --------------------------
#
# We first generate the data and we add noise to the output observations:

# %%
ot.RandomGenerator.SetSeed(0)
distribution = ot.Normal(2)
distribution.setDescription(["x", "y"])
func = ot.SymbolicFunction(["x", "y"], ["2 * x - y + 3 + 0.05 * sin(0.8*x)"])
input_sample = distribution.getSample(30)
epsilon = ot.Normal(0, 0.1).getSample(30)
output_sample = func(input_sample) + epsilon

# %%
# Linear regression
# -----------------
#
# Let us run the linear model algorithm using the `LinearModelAlgorithm` class and get the associated results :

# %%
algo = ot.LinearModelAlgorithm(input_sample, output_sample)
result = algo.getResult()

# %%
# Residuals analysis
# ------------------
#
# We can now analyse the residuals of the regression on the training data.
# For clarity purposes, only the first 5 residual values are printed.

# %%
residuals = result.getSampleResiduals()
print(residuals[:5])

# %%
# Alternatively, the `standardized` or `studentized` residuals can be used:

# %%
stdresiduals = result.getStandardizedResiduals()
print(stdresiduals[:5])

# %%
# Similarly, we can also obtain the underyling distribution characterizing the residuals:

# %%
print(result.getNoiseDistribution())


# %%
# ANOVA table
# -----------
#
# In order to post-process the linear regression results, the `LinearModelAnalysis` class can be used:

# %%
analysis = ot.LinearModelAnalysis(result)
print(analysis)

# %%
# The results seem to indicate that the linear hypothesis can be accepted. Indeed, the `R-Squared` value is nearly `1`. Furthermore, the adjusted value, which takes into account the data set size and the number of hyperparameters, is similar to `R-Squared`.
#
# We can also notice that the `Fisher-Snedecor` and `Student` p-values detailed above are lower than 1%. This ensures an acceptable quality of the linear model.

# %%
# Graphical analyses
# ------------------
#
# Let us compare model and fitted values:

# %%
graph = analysis.drawModelVsFitted()
view = viewer.View(graph)

# %%
# The previous figure seems to indicate that the linearity hypothesis is accurate.

# %%
# Residuals can be plotted against the fitted values.

# %%
graph = analysis.drawResidualsVsFitted()
view = viewer.View(graph)

# %%
graph = analysis.drawScaleLocation()
view = viewer.View(graph)

# %%
graph = analysis.drawQQplot()
view = viewer.View(graph)

# %%
# In this case, the two distributions are very close: there is no obvious outlier.
#
# Cook's distance measures the impact of every invidual data point on the linear regression, and can be plotted as follows:

# %%
graph = analysis.drawCookDistance()
view = viewer.View(graph)

# %%
# This graph shows us the index of the points with disproportionate influence.
#
# One of the components of the computation of Cook's distance at a given point is that point's *leverage*.
# High-leverage points are far from their closest neighbors, so the fitted linear regression model must pass close to them.

# %%
graph = analysis.drawResidualsVsLeverages()
view = viewer.View(graph)

# %%
# In this case, there seem to be no obvious influential outlier characterized by large leverage and residual values, as is also shown in the figure below:
#
# Similarly, we can also plot Cook's distances as a function of the sample leverages:

# %%
graph = analysis.drawCookVsLeverages()
view = viewer.View(graph)

# %%
# Finally, we give the intervals for each estimated coefficient (95% confidence interval):

# %%
alpha = 0.95
interval = analysis.getCoefficientsConfidenceInterval(alpha)
print("confidence intervals with level=%1.2f : " % (alpha))
print("%s" % (interval))
