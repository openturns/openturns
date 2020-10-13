"""
Fit a distribution by maximum likelihood
========================================
"""
# %%
# In this example we are going to estimate the parameters of a parametric by generic numerical optimization of the likelihood.
#
# The likelihood of a sample :math:`X` according to a parametric density function :math:`p_{\underline{\theta}}` is:
#
# .. math::
#    likelihood(\underline{x}_1, \dots, \underline{x}_n,\underline{\theta}) = \prod_{i=1}^n p_{\underline{\theta}}(\underline{x}_i)
#

# %%
from __future__ import print_function
import openturns as ot
import math as m
ot.Log.Show(ot.Log.NONE)

# %%
# Create data from a gaussian pdf with mu=4, sigma=1.5
sample = ot.Normal(4.0, 1.5).getSample(200)

# %%
# Create the search interval of (mu, sigma): the constraint is sigma>0
lowerBound = [-1.0, 1.0e-4]
upperBound = [-1.0, -1.0]
finiteLowerBound = [False, True]
finiteUpperBound = [False, False]
bounds = ot.Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound)

# %%
# Create the starting point of the research
# For mu : the first point
# For sigma : a value evaluated from the two first points
mu0 = sample[0][0]
sigma0 = m.sqrt((sample[1][0] - sample[0][0]) * (sample[1][0] - sample[0][0]))
startingPoint = [mu0, sigma0]
ot.Point(startingPoint)

# %%
# Create the estimator from a parametric pdf
pdf = ot.Normal()
factory = ot.MaximumLikelihoodFactory(pdf)
factory.setOptimizationBounds(bounds)

# %%
# Set the starting point via the solver
solver = factory.getOptimizationAlgorithm()
solver.setStartingPoint(startingPoint)
factory.setOptimizationAlgorithm(solver)

# %%
# Estimate the parametric model
distribution = factory.build(sample)
str(distribution)

# %%
# Retrieve the estimated parameters
distribution.getParameter()
