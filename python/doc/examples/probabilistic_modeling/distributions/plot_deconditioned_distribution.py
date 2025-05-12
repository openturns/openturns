"""
Create a deconditioned distribution
===================================
"""

# %%
# In this example we are going to build the distribution of the random vector :math:`\inputRV`
# defined by the conditional distribution of:
#
# .. math::
#
#    \inputRV|\vect{\Theta}
#
# where :math:`\vect{\Theta}` is the output of the random variable :math:`\vect{Y}` through the link
# function :math:`f`:
#
# .. math::
#
#    \vect{\Theta} & = f(\vect{Y})\\
#    \vect{Y} & \sim \cL_{\vect{Y}}
#
# This example creates a :class:`~openturns.DeconditionedDistribution` which offers all the methods
# attached to the distributions.
#
# We consider the case where :math:`X` is of dimension 1 and follows a uniform distribution defined
# by:
#
# ===============  =========================================================  ===============================
# Variable         Distribution                                               Parameter
# ===============  =========================================================  ===============================
# :math:`X`        :class:`~openturns.Uniform` (:math:`a, b`)                 :math:`(a,b) = (Y, 1+Y^2)`
# :math:`Y`        :class:`~openturns.Uniform` (:math:`c, d`)                 :math:`(c,d) = (-1, 1)`
# ===============  =========================================================  ===============================
#

# %%
import openturns as ot
import openturns.viewer as otv

# %%
# Create the :math:`Y` distribution.
YDist = ot.Uniform(-1.0, 1.0)

# %%
# Create the link function :math:`f: y \rightarrow (y, 1+y^2)`.
f = ot.SymbolicFunction(["y"], ["y", "1+y^2"])

# %%
# Create the conditional distribution of :math:`\vect{X}|\vect{\Theta}`: as the parameters have no
# importance, we use the default distribution.
XgivenThetaDist = ot.Uniform()

# %%
# Create the deconditioned distribution of :math:`X`.
XDist = ot.DeconditionedDistribution(XgivenThetaDist, YDist, f)
XDist.setDescription([r"$X|\mathbf{\boldsymbol{\Theta}} = f(Y)$"])
XDist

# %%
# Get a sample:
XDist.getSample(5)

# %%
# Draw the PDF.
graph = XDist.drawPDF()
view = otv.View(graph)


# %%
view.ShowAll()
