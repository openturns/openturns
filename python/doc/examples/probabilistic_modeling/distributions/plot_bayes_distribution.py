"""
Create a Bayes distribution
===========================
"""
# %%
# In this example we are going to build the distribution of the random vector
#
# .. math::
#    (\underline{Y}, \underline{X}|\underline{\Theta})
#
# with X conditioned by the random variable Theta obtained with the random variable Y through a function f
#
# .. math::
#    \underline{\Theta}=f(\underline{Y})
#

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# %%
# create the Y distribution
YDist = ot.Normal(0.0, 1.0)

# %%
# create Theta=f(y)
f = ot.SymbolicFunction(["y"], ["y", "0.1 + y^2"])

# %%
# create the X|Theta distribution
XgivenThetaDist = ot.Normal()

# %%
# create the distribution
XDist = ot.BayesDistribution(XgivenThetaDist, YDist, f)
XDist.setDescription(["X|Theta=f(y)", "y"])
XDist

# %%
# Get a sample
sample = XDist.getSample(100)

# %%
# draw PDF
graph = XDist.drawPDF()
cloud = ot.Cloud(sample)
cloud.setColor("red")
cloud.setLegend("sample")
graph.add(cloud)
view = viewer.View(graph)
plt.show()
