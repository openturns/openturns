"""
Create a conditional distribution
=================================
"""

# %%

import openturns as ot
import openturns.viewer as otv


# %%
# In this example we are going to build the distribution of the random vector :math:`\vect{X}` conditioned by
# the random vector :math:`\vect{\Theta}`:
#
# .. math::
#    \vect{X}|\vect{\Theta}
#
# with :math:`\vect{\Theta}` obtained with the random variable :math:`Y` through a function :math:`f`
#
# .. math::
#    \vect{\Theta} = f(Y)
#

# %%
# We consider the following case: :math:`X|\vect{\Theta} \sim \cU(\vect{\Theta})`
# with :math:`\vect{\Theta} = (Y, 1 + Y^2)` and :math:`Y \sim \cU(-1,1)`.
# We first create the :math:`Y` distribution:
YDist = ot.Uniform(-1.0, 1.0)

# %%
# Then we create the link function `f: y \rightarrow (y, 1+y^2):
f = ot.SymbolicFunction(["y"], ["y", "1+y^2"])

# %%
# Then, we create the :math:`\vect{X}|\vect{\Theta}` distribution:
XgivenThetaDist = ot.Uniform()

# %%
# At last, we create the deconditioned distribution of:math:`X`:
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
