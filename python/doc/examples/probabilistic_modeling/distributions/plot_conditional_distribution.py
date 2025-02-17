"""
Create a conditional distribution
=================================
"""

# %%

import openturns as ot
import openturns.viewer as otv


# %%
# In this example we are going to build the distribution of the random vector :math:`\vect{X}` conditioned by the random vector :math:`\vect{\Theta}`
#
# .. math::
#    \vect{X}|\vect{\Theta}
#
# with :math:`\vect{\Theta}` obtained with the random variable :math:`Y` through a function :math:`f`
#
# .. math::
#    \vect{\Theta}=f(Y)
#

# %%
# Create the :math:`Y` distribution
YDist = ot.Uniform(-1.0, 1.0)

# %%
# Create :math:`\vect{\Theta}=f(Y)`
f = ot.SymbolicFunction(["y"], ["y", "1+y^2"])

# %%
# Create the :math:`\vect{X}|\vect{\Theta}` distribution
XgivenThetaDist = ot.Uniform()

# %%
# create the distribution
XDist = ot.DeconditionedDistribution(XgivenThetaDist, YDist, f)
XDist.setDescription(["X|Theta=f(y)"])
XDist

# %%
# Get a sample
XDist.getSample(5)

# %%
# Draw PDF
graph = XDist.drawPDF()
view = otv.View(graph)


# %%
view.ShowAll()
