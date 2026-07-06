"""
Bandwidth sensitivity in kernel smoothing
=========================================
"""

# %%
#
# Introduction
# ------------
#
# When we have a sample, we may estimate the probability density function of the
# underlying distribution using kernel smoothing.
# One of the parameters of this method is the bandwidth, which can be either
# set by the user, or estimated from the data.
# This is especially true when the density is multimodal.
# In this example, we consider a bimodal distribution and see how the bandwidth
# can change the estimated probability density function.
#
# We consider the distribution:
#
# .. math::
#    f(x) = w_1 f_1(x) + w_2 f_2(x)
#
#
# for any :math:`x\in\mathbb{R}` where :math:`f_1` is the density of the Normal distribution
# :math:`\mathcal{N}(0,1)`, :math:`f_2` is the density of the Normal distribution
# :math:`\mathcal{N}(3/2,(1/3)^2)` and the weights are :math:`w_1 = \frac{3}{4}` and :math:`w_2 = \frac{1}{4}`.
#
# This is a mixture of two Normal distributions: when the sample size goes to infinity,
# 3/4th of the observations have the :math:`\mathcal{N}(0,1)`
# distribution and 1/4th of the observations have the :math:`\mathcal{N}(3/2,(1/3)^2)`
# distribution.
# Notice that this may not be the case when we consider a sample with finite size.
# This example is considered in (Wand, Jones, 1994), page 14, Figure 2.3.
#
# We consider a sample generated from independent realizations of :math:`f` and
# want to approximate the distribution from kernel smoothing.
# More precisely, we want to observe the sensitivity of the resulting density to the
# bandwidth.
#
# References
# ----------
#
# * "Kernel Smoothing", M.P.Wand, M.C.Jones. Chapman and Hall / CRC (1994).
#

# %%
import openturns as ot
import openturns.viewer as otv

# %%
# Simulation based on a mixture
# -----------------------------

# %%
# We choose a rather large sample size: :math:`\sampleSize=1000`.

# %%
sampleSize = 1000

# %%
# Then we define the two Normal distributions and their parameters.

# %%
w1 = 0.75
w2 = 1.0 - w1
distribution1 = ot.Normal(0.0, 1.0)
distribution2 = ot.Normal(1.5, 1.0 / 3.0)

# %%
# Since the distribution that we approximate is a mixture, we can create it from
# the :class:`~openturns.Mixture` class.
# The constructor takes as input argument a list of distributions and a list of weights.

# %%
distribution = ot.Mixture([distribution1, distribution2], [w1, w2])

# %%
# Then we generate a sample from it.

# %%
sample = distribution.getSample(sampleSize)

# %%
factory = ot.KernelSmoothing()
fit = factory.build(sample)

# %%
factory.getBandwidth()

# %%
# We see that the default bandwidth is close to 0.17.

# %%
graph = distribution.drawPDF()
curve = fit.drawPDF()
graph.add(curve)
graph.setLegends(["Mixture", "Kernel smoothing"])
graph.setLegendPosition("upper left")
view = otv.View(graph)

# %%
# Sensitivity to the bandwidth
# ----------------------------

# %%
# In this section, we observe the sensitivity of the kernel smoothing to the bandwidth.
# We consider the three following bandwidths: the small bandwidth 0.05, the large bandwidth 0.54 and 0.18 which is in-between.
# For each bandwidth, we use the second optional argument of the `build` method in order to select a specific bandwidth value.


# %%
def fitKernelSmoothingGivenBandwidth(distribution, sample, factory, bandwidth, label=None):
    # Fit the kernel smoothing and draw the PDF
    fit = factory.build(sample, [bandwidth])
    graph = fit.drawPDF()
    # Add the PDF of the distribution
    exact = distribution.drawPDF()
    curve = exact.getDrawable(0)
    curve.setLegend("Mixture")
    curve.setLineStyle("dashed")
    graph.add(curve)
    # Decorate the graph with a title and a X label
    graph.setXTitle("X")
    if label is None:
        graph.setTitle(f"h={bandwidth:.4f}")
    else:
        graph.setTitle(f"h={bandwidth:.4f}, {label}")
    graph.setLegends([""])
    return graph


# %%
hArray = [0.05, 0.54, 0.18]
nLen = len(hArray)
grid = ot.GridLayout(1, nLen)
for i in range(nLen):
    graph = fitKernelSmoothingGivenBandwidth(distribution, sample, factory, hArray[i])
    if i > 0:
        graph.setYTitle("")
    boundingBox = graph.getBoundingBox()
    upperBound = boundingBox.getUpperBound()
    upperBound[1] = 0.5  # Common y-range
    graph.setBoundingBox(boundingBox)
    grid.setGraph(0, i, graph)

view = otv.View(grid, figure_kw={"figsize": (10.0, 4.0)})
# %%
# We see that when the bandwidth is too small, the resulting kernel smoothing has many more modes than the distribution it is supposed to approximate.
# When the bandwidth is too large, the approximated distribution is too smooth and has only one mode instead of the expected two modes which are in the mixture distribution.
# When the bandwidth is equal to 0.18, the two modes are correctly represented.

# %%
# Sensitivity to the bandwidth rule
# ---------------------------------
#
# The library provides three different rules to compute the bandwidth. In this section, we compare the results that we can get with them.

# %%
h1 = factory.computeSilvermanBandwidth(sample)[0]
h1

# %%
h2 = factory.computePluginBandwidth(sample)[0]
h2

# %%
h3 = factory.computeMixedBandwidth(sample)[0]
h3

# %%
factory.getBandwidth()[0]

# %%
# We see that the default rule is the "Mixed" rule. This is because the sample is in dimension 1 and the sample size is quite large.
# For a small sample in 1 dimension, the "Plugin" rule would have been used.
#
# The following script compares the results produced by the three rules.

# %%
hArray = [h1, h2, h3]
legends = ["Silverman", "Plugin", "Mixed"]
nLen = len(hArray)
grid = ot.GridLayout(1, nLen)
for i in range(nLen):
    graph = fitKernelSmoothingGivenBandwidth(distribution, sample, factory, hArray[i], legends[i])
    if i > 0:
        graph.setYTitle("")
    boundingBox = graph.getBoundingBox()
    upperBound = boundingBox.getUpperBound()
    upperBound[1] = 0.5  # Common y-range
    graph.setBoundingBox(boundingBox)
    grid.setGraph(0, i, graph)

view = otv.View(grid, figure_kw={"figsize": (10.0, 4.0)})

otv.View.ShowAll()

# %%
# We see that the bandwidth produced by Silverman's rule is too large, leading to an oversmoothed distribution.
# The results produced by the Plugin and Mixed rules are comparable in this case.
