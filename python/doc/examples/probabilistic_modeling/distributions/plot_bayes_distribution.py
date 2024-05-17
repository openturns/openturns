"""
Create a Bayes distribution
===========================
"""
# %%
# In this example we are going to build the distribution of the random vector
#
# .. math::
#    (\vect{Y}, \vect{X}|\vect{\Theta})
#
# with :math:`\vect{X}` conditioned by the random vector :math:`\vect{\Theta}` obtained with the random variable :math:`Y` through a function :math:`f`
#
# .. math::
#    \vect{\Theta}=f(Y)
#

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# %%
# Create the math:`Y` distribution
YDist = ot.Normal(0.0, 1.0)

# %%
# Create :math:`\vect{\Theta}=f(Y)`
f = ot.SymbolicFunction(["y"], ["y", "0.1 + y^2"])

# %%
# Create the :math:`\vect{X}|\vect{\Theta}` distribution
XgivenThetaDist = ot.Normal()

# %%
# Create the distribution
XDist = ot.BayesDistribution(XgivenThetaDist, YDist, f)
XDist.setDescription(["X|Theta=f(y)", "y"])
XDist

# %%
# Get a sample
sample = XDist.getSample(100)

# %%
# Draw PDF
graph = XDist.drawPDF()
cloud = ot.Cloud(sample)
cloud.setColor("red")
cloud.setLegend("sample")
graph.add(cloud)
view = viewer.View(graph)
plt.show()
