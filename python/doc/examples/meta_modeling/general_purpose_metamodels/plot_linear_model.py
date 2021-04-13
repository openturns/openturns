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
from __future__ import print_function
import openturns as ot
from openturns.viewer import View
import numpy as np
import matplotlib.pyplot as plt
import openturns.viewer as viewer

# %%
# Generation of the data set
# --------------------------
#
# We first generate the data and we add noise to the output observations: 

# %%
ot.RandomGenerator.SetSeed(0)
distribution = ot.Normal(2)
distribution.setDescription(['x','y'])
func = ot.SymbolicFunction(['x', 'y'], ['2 * x - y + 3 + 0.05 * sin(0.8*x)'])
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
# For clarity purposes, only the first 5 residual values are printed

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
# Seems that the linearity hypothesis is accurate.

# %%
# We complete this analysis using some usefull graphs :

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
graph = analysis.drawCookDistance()
view = viewer.View(graph)

# %%
graph = analysis.drawResidualsVsLeverages()
view = viewer.View(graph)

# %%
graph = analysis.drawCookVsLeverages()
view = viewer.View(graph)


# %%
# These graphics help asserting the linear model hypothesis. Indeed :
#  
#  - Quantile-to-quantile plot seems accurate
#  
#  - We notice homoscedasticity within the noise
#  
#  - No obvious outlier can be identified in the training data

# %%
# Finally, we give the intervals for each estimated coefficient (95% confidence interval):

# %%
alpha = 0.95
interval = analysis.getCoefficientsConfidenceInterval(alpha)
print("confidence intervals with level=%1.2f : " % (alpha))
print("%s" % (interval))


