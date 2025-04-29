"""
Create a deconditioned random vector
====================================

# %%
# In this example we are going to build the random vector :math:`\inputRV` defined by the
# conditional distribution of:
#
# .. math::
#
#    \inputRV|\vect{\Theta}
#
# where :math:`\vect{\Theta}` is the output of the random variable :math:`\vect{Y}` through
# the link function :math:`f`:
#
# .. math::
#
#    \vect{\Theta} & = f(\vect{Y})\\
#    \vect{Y} & \sim \cL_{\vect{Y}}
#
# This example creates a :class:`~openturns.RandomVector` which can only be sampled.
#
# We consider the case where :math:`X` is of dimension 1 and follows a exponential distribution
# defined by:
#
# .. math::
#
#    X|(\Lambda, \Gamma) & \sim \cE(\Lambda, \Gamma) \\
#    \Lambda & \sim \cU(0, 0.1)\\
#    \Gamma & \sim \cU(1,2) \\
#
# with :math:`(\Lambda, \Gamma)` independent.

# %%
import openturns as ot

# %%
# Create the random vector :math:`\vect{\Theta} = (\Lambda, \Gamma)`:
gammaDist = ot.Uniform(1.0, 2.0)
lambdaDist = ot.Uniform(0.0, 0.1)
thetaDist = ot.JointDistribution([lambdaDist, gammaDist])
thetaRV = ot.RandomVector(thetaDist)

# %%
# Create the :math:`\inputRV|\vect{\Theta}` distribution: as the parameters have no importance, we
# create the default distribution.
XgivenThetaDist = ot.Exponential()

# %%
# Create the :math:`\vect X` distribution
XDist = ot.DeconditionedRandomVector(XgivenThetaDist, thetaRV)

# %%
# Draw a sample
XDist.getSample(5)
