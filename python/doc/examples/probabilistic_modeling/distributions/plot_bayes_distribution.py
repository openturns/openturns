r"""
Create a Joint by Conditioning  distribution
============================================
"""

# %%
# In this example we are going to build the distribution of the random vector:
#
# .. math::
#    (Y, \vect{X}|\vect{\Theta})
#
# with :math:`\vect{X}` conditioned by the random vector :math:`\vect{\Theta}` obtained with the random variable :math:`Y` through a function :math:`f`:
#
# .. math::
#    \vect{\Theta} = f(\vect{Y})
#

# %%
import openturns as ot
import openturns.viewer as viewer


# %%
# We consider the following case: :math:`X|\vect{\Theta} \sim \cN(\vect{\Theta})`
# with :math:`\vect{\Theta} = (Y, 0.1 + Y^2)` and :math:`Y \sim \cN(0,1)`.
#
# We first create the :math:`Y` distribution:
YDist = ot.Normal(0.0, 1.0)

# %%
# Then we create the link function :math:`f: y \rightarrow (y, 0.1+y^2)`:
f = ot.SymbolicFunction(["y"], ["y", "0.1 + y^2"])

# %%
# Then, we create the :math:`\vect{X}|\vect{\Theta}` distribution:
XgivenThetaDist = ot.Normal()

# %%
# At last, we create the distribution of :math:`(Y,X)`:
XDist = ot.JointByConditioningDistribution(XgivenThetaDist, YDist, f)
XDist.setDescription(["Y", r"$X|\mathbf{\boldsymbol{\Theta}} = f(Y)$"])
XDist

# %%
# Get a sample:
sample = XDist.getSample(100)

# %%
# Draw the PDF:
ot.ResourceMap.SetAsString("Contour-DefaultColorMapNorm", "rank")
graph = XDist.drawPDF(sample.getMin(), sample.getMax(), [256] * 2)
graph.setTitle(r"$(Y,X)$ iso-PDF")
cloud = ot.Cloud(sample)
cloud.setColor("red")
cloud.setLegend("sample")
graph.add(cloud)
view = viewer.View(graph)

# %%
view.ShowAll()
