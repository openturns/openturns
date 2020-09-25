"""
Create a linear model
=====================
"""
# %%
# In this example we are going to create a global approximation of a model response using linear model approximation.
#
# Here :math:`h(x,y) = [2 x + 0.05 * \sin(x) - y]`.
#

# %%
from __future__ import print_function
import openturns as ot
try:
    get_ipython()
except NameError:
    import matplotlib
    matplotlib.use('Agg')
from openturns.viewer import View
import numpy as np
import matplotlib.pyplot as plt
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# Hereafter we generate data using the previous model. We also add a noise: 

# %%
ot.RandomGenerator.SetSeed(0)
distribution = ot.Normal(2)
distribution.setDescription(["x","y"])
func = ot.SymbolicFunction(['x', 'y'], ['2 * x - y + 3 + 0.05 * sin(0.8*x)'])
input_sample = distribution.getSample(30)
epsilon = ot.Normal(0, 0.1).getSample(30)
output_sample = func(input_sample) + epsilon

# %%
# Let us run the linear model algorithm using the `LinearModelAlgorithm` class & get its associated result :

# %%
algo = ot.LinearModelAlgorithm(input_sample, output_sample)
result = ot.LinearModelResult(algo.getResult())

# %%
#  

# %%
# We get the result structure. As the underlying model is of type regression, it assumes a noise distribution associated to the residuals. Let us get it:

# %%
print(result.getNoiseDistribution())

# %%
# We can get also residuals:

# %%
print(result.getSampleResiduals())

# %%
# We can get also `standardized` residuals (also called `studentized residuals`). 

# %%
print(result.getStandardizedResiduals())

# %%
# Now we got the result, we can perform a postprocessing analysis. We use `LinearModelAnalysis` for that purpose: 

# %%
analysis = ot.LinearModelAnalysis(result)
print(analysis)

# %%
# It seems that the linear hypothesis could be accepted. Indeed, `R-Squared` value is nearly `1`. Also the adjusted value (taking into account the datasize & number of parameters) is similar to `R-Squared`. 
#
# Also, we notice that both `Fisher-Snedecor` and `Student` p-values detailled above are less than 1%. This ensure the quality of the linear model.

# %%
# Let us compare model and fitted values:

# %%
graph = analysis.drawModelVsFitted()
view = viewer.View(graph)

# %%
# Seems that the linearity hypothesis is accurate.

# %%
# We complete this analysis using some usefull graphs :

# %%
fig = plt.figure(figsize=(12,10))
for k, plot in enumerate(["drawResidualsVsFitted", "drawScaleLocation", "drawQQplot",
             "drawCookDistance", "drawResidualsVsLeverages", "drawCookVsLeverages"]):
    graph = getattr(analysis, plot)()
    ax = fig.add_subplot(3, 2, k + 1)
    v = View(graph, figure=fig, axes=[ax])
_ = v.getFigure().suptitle("Diagnostic graphs", fontsize=18)

# %%
# These graphics help asserting the linear model hypothesis. Indeed :
#  
#  - Quantile-to-quantile plot seems accurate
#  
#  - We notice heteroscedasticity within the noise
#  
#  - It seems that there is no outlier

# %%
# Finally we give the intervals for each estimated coefficient (95% confidence interval):

# %%
alpha = 0.95
interval = analysis.getCoefficientsConfidenceInterval(alpha)
print("confidence intervals with level=%1.2f : %s" % (alpha, interval))
if not viewer._noshow:
    plt.show()

