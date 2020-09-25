"""
Create a conditional distribution
=================================
"""
# %% 

# %%
# In this example we are going to build the distribution of the random vector X conditioned by the random variable Theta
#
# .. math:: 
#    \underline{X}|\underline{\Theta}
#   
# with Theta obtained with the random variable Y through a function f
#
# .. math:: 
#    \underline{\Theta}=f(\underline{Y})
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# create the Y distribution
YDist = ot.Uniform(-1.0, 1.0)

# %%
# create Theta=f(y)
f = ot.SymbolicFunction(['y'], ['y', '1+y^2'])

# %%
# create the X|Theta distribution
XgivenThetaDist = ot.Uniform()

# %%
# create the distribution
XDist = ot.ConditionalDistribution(XgivenThetaDist, YDist, f)
XDist.setDescription(['X|Theta=f(y)'])
XDist

# %%
# Get a sample
XDist.getSample(5)

# %%
# draw PDF
graph = XDist.drawPDF()
view = viewer.View(graph)
if not viewer._noshow:
    plt.show()
