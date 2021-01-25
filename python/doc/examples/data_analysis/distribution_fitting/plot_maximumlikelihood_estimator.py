"""
Fit a distribution by maximum likelihood
========================================
"""
# %%
# In this example we are going to estimate the parameters of a parametric by generic numerical optimization of the likelihood.
#
# The likelihood of a sample :math:`(\vect{x}_1, \dots, \vect{x}_n)` according to a parametric density function :math:`p_{\vect{\theta}}` is:
#
# .. math::
#    \ell(\vect{x}_1, \dots, \vect{x}_n,\vect{\theta}) = \prod_{i=1}^n p_{\vect{\theta}}(\vect{x}_i)
#

# %%
from __future__ import print_function
import openturns as ot
import math as m
ot.Log.Show(ot.Log.NONE)

# %%
# Create data from a normal PDF with :math:`\mu=4`, :math:`\sigma=1.5`.
sample = ot.Normal(4.0, 1.5).getSample(200)

# %%
# Create the search interval of (:math:`\mu`, :math:`\sigma`) : the constraint is :math:`\sigma>0`.
lowerBound = [-1.0, 1.0e-4]
upperBound = [-1.0, -1.0]
finiteLowerBound = [False, True]
finiteUpperBound = [False, False]
bounds = ot.Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound)

# %%
# Create the starting point of the research:
#
# - for :math:`\mu` : the first point,
# - for :math:`\sigma` : a value evaluated from the two first points.
mu0 = sample[0][0]
sigma0 = m.sqrt((sample[1][0] - sample[0][0]) * (sample[1][0] - sample[0][0]))
startingPoint = [mu0, sigma0]

# %%
# Create the estimator from a parametric PDF.
pdf = ot.Normal()
factory = ot.MaximumLikelihoodFactory(pdf)
factory.setOptimizationBounds(bounds)

# %%
# Set the starting point via the solver.
solver = factory.getOptimizationAlgorithm()
solver.setStartingPoint(startingPoint)
factory.setOptimizationAlgorithm(solver)

# %%
# Estimate the parametric model.
distribution = factory.build(sample)
str(distribution)

# %%
# Retrieve the estimated parameters.
print(distribution.getParameter())
