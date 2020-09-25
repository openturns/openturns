"""
Create a 2-d gaussian distribution
==================================
"""
# %%
# In this example we are going to create a bidimensional gaussian distribution with parameters :math:`\mu = [0.0, 2.2], \sigma = [1.0, 0.6]`.
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# create the distribution, no correlation
distribution = ot.Normal([0.0, 2.0], [1.0, 0.6], ot.CorrelationMatrix(2))
print(distribution)

# %%
# draw a sample
sample = distribution.getSample(10)
print(sample)

# %%
# draw PDF
graph = distribution.drawPDF()
view = viewer.View(graph)
if not viewer._noshow:
    plt.show()
