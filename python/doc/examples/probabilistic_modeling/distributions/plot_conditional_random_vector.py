r"""
Create a conditional random vector
==================================
"""

# %%
# In this example we are going to build a conditional random vector
#
# .. math::
#    \vect X|\vect\Theta
#

# %%
import openturns as ot

# %%
# Create the random vector :math:`\vect\Theta` (parameters of :math:`\vect X`)
gammaDist = ot.Uniform(1.0, 2.0)
alphaDist = ot.Uniform(0.0, 0.1)
thetaDist = ot.JointDistribution([gammaDist, alphaDist])
thetaRV = ot.RandomVector(thetaDist)

# %%
# Create the :math:`\vect X|\vect\Theta` distribution
XgivenThetaDist = ot.Exponential()

# %%
# Create the :math:`\vect X` distribution
XDist = ot.ConditionalRandomVector(XgivenThetaDist, thetaRV)

# %%
# Draw a sample
XDist.getSample(5)
