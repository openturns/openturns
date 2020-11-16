"""
Fit an extreme value distribution
=================================
"""


# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# %%
# Set the random generator seed
ot.RandomGenerator.SetSeed(0)


# %%
# The generalized extreme value distribution (GEV)
# ------------------------------------------------

# %%
# The :class:`~openturns.GeneralizedExtremeValue` distribution is a family of continuous probability distributions that combine the :class:`~openturns.Gumbel`, :class:`~openturns.Frechet` and :class:`~openturns.WeibullMax` distribution, all extreme value distribution.
#
# In this example we use the associated :class:`~openturns.GeneralizedExtremeValueFactory` to fit sample with extreme values. This factory returns the best model among the Frechet, Gumbel and Weibull estimates according to the BIC criterion.
#


# %%
# For experiment purpose we draw a sample from a Gumbel of parameters :math:`\beta = 1.0` and :math:`\gamma = 3.0` and another one from a Frechet with parameters :math:`\beta=1.0, \alpha = 1.0` and :math:`\gamma = 0.0` .
# We consider both samples as a unique sample from an unknown extreme distribution to be fitted.
#

# %%
# The distributions used :
myGumbel = ot.Gumbel(1.0, 3.0)
myFrechet = ot.Frechet(1.0, 1.0, 0.0)


# %%
# We build our experiment sample of size 2000.
sample = ot.Sample()
sampleFrechet = myFrechet.getSample(1000)
sampleGumbel = myGumbel.getSample(1000)
sample.add(sampleFrechet)
sample.add(sampleGumbel)


# %%
# We fit the sample thanks to the `GeneralizedExtremeValueFactory` :
myDistribution = ot.GeneralizedExtremeValueFactory().buildAsGeneralizedExtremeValue(
    sample
)

# %%
# We can display the parameters of the fitted distribution `myDistribution` :
print(myDistribution)

# %%
# We can also get the actual distribution (Weibull, Frechet or Gumbel) with the `getActualDistribution` method :
print(myDistribution.getActualDistribution())

# %%
# The given sample is then best described by a WeibullMax distribution.

# %%
# We draw the fitted distribution and an histogram of the data.
graph = myDistribution.drawPDF()
graph.add(ot.HistogramFactory().build(sample).drawPDF())
graph.setColors(["black", "red"])
graph.setLegends(["GEV fitting", "histogram"])
graph.setLegendPosition("topright")

view = viewer.View(graph)
axes = view.getAxes()
_ = axes[0].set_xlim(-20.0, 20.0)


# %%
# For educational purpose we compare different fitting strategies for this sample :
#
# - we use the histogram from the data (in red)
# - the GEV fitted distribution (in black)
# - the pure Frechet fitted distribution (in green)
# - the pure Gumbel fitted distribution (in blue)
# - the pure WeibullMax fitted distribution (in dashed orange)
#
graph = myDistribution.drawPDF()
graph.add(ot.HistogramFactory().build(sample).drawPDF())

distFrechet = ot.FrechetFactory().buildAsFrechet(sample)
graph.add(distFrechet.drawPDF())

distGumbel = ot.GumbelFactory().buildAsGumbel(sample)
graph.add(distGumbel.drawPDF())

# We change the line style of the WeibullMax because it is expected to be the same 
# as the GEV one.
distWeibullMax = ot.WeibullMaxFactory().buildAsWeibullMax(sample)
curveWeibullMax = distWeibullMax.drawPDF().getDrawable(0)
curveWeibullMax.setLineStyle("dashed")
graph.add(curveWeibullMax)

graph.setColors(["black", "red", "green", "blue", "orange"])
graph.setLegends(
    [
        "GEV fitting",
        "histogram",
        "Frechet fitting",
        "Gumbel fitting",
        "WeibullMax fitting",
    ]
)
graph.setLegendPosition("topright")
view = viewer.View(graph)
axes = view.getAxes()  # axes is a matplotlib object
_ = axes[0].set_xlim(-20.0, 20.0)


# %%
# As returned by the `getActualDistribution` method the GEV distribution is a WeibullMax.
# The :class:`~openturns.GeneralizedExtremeValueFactory` class is a convenient class to fit extreme valued samples without an a priori knowledge of the underlying (at least the closest) extreme distribution.


# %%
# The Generalized Pareto Distribution (GPD)
# -----------------------------------------
#
# In this paragraph we turn to the fitting of a :class:`~openturns.GeneralizedPareto` distribution.
# Various estimators are defined in OpenTurns for the GPD factory. Please refer to the :class:`~openturns.GeneralizedParetoFactory` class documentation for more information.
# The selection is based on the sample size and compared to the `GeneralizedParetoFactory-SmallSize` key of the :class:`~openturns.ResourceMap` :
#

# %%
print(ot.ResourceMap.GetAsUnsignedInteger("GeneralizedParetoFactory-SmallSize"))


# %%
# The small sample case
# ^^^^^^^^^^^^^^^^^^^^^
#
# In this case the default estimator is based on a probability weighted method of moments, with a fallback on the exponential regression method.

# %%
myDist = ot.GeneralizedPareto(1.0, 0.0, 0.0)
N = 17
sample = myDist.getSample(N)

# %%
# We build our experiment sample of size N
myFittedDist = ot.GeneralizedParetoFactory().buildAsGeneralizedPareto(sample)
print(myFittedDist)

# %%
# We draw the fitted distribution as well as an histogram to visualize the fitting :
graph = myFittedDist.drawPDF()
graph.add(ot.HistogramFactory().build(sample).drawPDF())
graph.setTitle("Generalized Pareto distribution fitting on a sample")
graph.setColors(["black", "red"])
graph.setLegends(["GPD fitting", "histogram"])
graph.setLegendPosition("topright")

view = viewer.View(graph)
axes = view.getAxes()
_ = axes[0].set_xlim(-1.0, 10.0)


# %%
# Large sample
# ^^^^^^^^^^^^
#
# For a larger sample the estimator is based on an exponential regression method
# with a fallback on the probability weighted moments estimator.


# %%
N = 1000
sample = myDist.getSample(N)

# %%
# We build our experiment sample of size N
myFittedDist = ot.GeneralizedParetoFactory().buildAsGeneralizedPareto(sample)
print(myFittedDist)

# %%
# We draw the fitted distribution as well as an histogram to visualize the fitting :
graph = myFittedDist.drawPDF()
graph.add(ot.HistogramFactory().build(sample).drawPDF())
graph.setTitle("Generalized Pareto distribution fitting on a sample")
graph.setColors(["black", "red"])
graph.setLegends(["GPD fitting", "histogram"])
graph.setLegendPosition("topright")

view = viewer.View(graph)
axes = view.getAxes()
_ = axes[0].set_xlim(-1.0, 10.0)


plt.show()
