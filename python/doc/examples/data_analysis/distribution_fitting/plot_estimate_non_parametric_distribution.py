"""
Non parametric distribution fitting
===================================
"""
# %%
# In this example we are going to estimate a non parametric distribution using the kernel smoothing method.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# Create data
ot.RandomGenerator.SetSeed(0)
distribution = ot.Gamma(6.0, 1.0)
sample = distribution.getSample(800)

# %%
# Estimate the Spearman correlation
kernel = ot.KernelSmoothing()
estimated = kernel.build(sample)

# %%
# Plot original distribution vs kernel smoothing
#graph = ot.Graph()
graph = distribution.drawPDF()
graph.setTitle('Kernel smoothing vs original')
graph.setGrid(True)



kernel_plot = estimated.drawPDF().getDrawable(0)
kernel_plot.setColor('blue')
#kernel_plot.setGrid(True)
#view = viewer.View(kernel_plot)
graph.add(kernel_plot)
graph.setLegends(['original', 'KS'])
graph.setLegendPosition('topright')
view = viewer.View(graph)

# %%
# access the bandwidth
kernel.getBandwidth()

# %%
# Compute bandwitdh with the Silverman rule
bandwidth = kernel.computeSilvermanBandwidth(sample)
bandwidth

# %%
# Regenerate with another bandwidth rule
estimated = kernel.build(sample, bandwidth)

# %%
# Plot original distribution vs kernel smoothing
graph = distribution.drawPDF()
graph.setTitle('Kernel smoothing vs original')
kernel_plot = estimated.drawPDF().getDrawable(0)
kernel_plot.setColor('blue')
graph.add(kernel_plot)
graph.setLegends(['original', 'KS'])
graph.setLegendPosition('topright')
view = viewer.View(graph)
plt.show()
