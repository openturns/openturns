"""
Create a conditional random vector
==================================
"""

# %%
# In this example we are going to build a conditional random vector
#
# .. math::
#    \underline{X}|\underline{\Theta}
#

# %%
import openturns as ot

# %%
# create the random vector Theta (parameters of X)
gammaDist = ot.Uniform(1.0, 2.0)
alphaDist = ot.Uniform(0.0, 0.1)
thetaDist = ot.ComposedDistribution([gammaDist, alphaDist])
thetaRV = ot.RandomVector(thetaDist)

# %%
# create the XgivenTheta distribution
XgivenThetaDist = ot.Exponential()

# %%
# create the X distribution
XDist = ot.ConditionalRandomVector(XgivenThetaDist, thetaRV)

# %%
# draw a sample
XDist.getSample(5)
