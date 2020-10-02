"""
Estimate a probability with Latin Hypercube Sampling
====================================================
"""
# %%
# In this example we show how to use the `LHS` algorithm to estimate the probability of an event.

# %%
# Introduction
# ------------
#
# We consider a simple beam stressed by a traction load F at both sides.
#
# ![Axial stressed beam](axial-stressed-beam.png)
#
# The geometry is supposed to be deterministic; the diameter D is equal to:
#
# .. math::
#    D=0.02 \textrm{ (m)}.
# 
#
# By definition, the yield stress is the load divided by the surface. Since the surface is :math:`\pi D^2/4`, the stress is:
#
# .. math::
#    S = \frac{F}{\pi D^2/4}.
# 
#
# Failure occurs when the beam plastifies, i.e. when the axial stress gets larger than the yield stress:
#
# .. math::
#    R - \frac{F}{\pi D^2/4} \leq 0
# 
#
# where :math:`R` is the strength.
#  
# Therefore, the limit state function :math:`G` is: 
#
# .. math::
#    G(R,F) = R - \frac{F}{\pi D^2/4},
# 
#
# for any :math:`R,F\in\mathbb{R}`.
#
# The value of the parameter :math:`D` is such that:
#
# .. math::
#    D^2/4 = 10^{-4},
# 
#
# which leads to the equation:
#
# .. math::
#    G(R,F) = R - \frac{F}{10^{-4} \pi}.
# 
#
# We consider the following distribution functions.
#
# ========  ===========================================================================
# Variable  Distribution 
# ========  ===========================================================================
#  R         LogNormal(:math:`\mu_R=3\times 10^6`, :math:`\sigma_R=3\times 10^5`) [Pa] 
#  F         Normal(:math:`\mu_F=750`, :math:`\sigma_F=50`) [N] 
# ========  ===========================================================================
#
# where :math:`\mu_R=E(R)` and :math:`\sigma_R^2=V(R)` are the mean and the variance of :math:`R`.
#
# The failure probability is: 
#
# .. math::
#    P_f = \text{Prob}(G(R,F) \leq 0).
# 
#
# The exact :math:`P_f` is 
#
# .. math::
#    P_f = 0.02920.
# 

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# Create the joint distribution of the parameters.

# %%
distribution_R = ot.LogNormalMuSigma(300.0, 30.0, 0.0).getDistribution()
distribution_F = ot.Normal(75e3, 5e3)
marginals = [distribution_R, distribution_F]
distribution = ot.ComposedDistribution(marginals)

# %%
# Create the model.

# %%
model = ot.SymbolicFunction(['R', 'F'], ['R-F/(pi_*100.0)'])

# %%
# Create the event whose probability we want to estimate.

# %%
vect = ot.RandomVector(distribution)
G = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(G, ot.Less(), 0.0)

# %%
# Create a LHS algorithm.

# %%
algo = ot.LHS(event)
algo.setMaximumCoefficientOfVariation(0.05)
algo.setMaximumOuterSampling(int(1e5))
algo.run()

# %%
# Retrieve results.

# %%
result = algo.getResult()
probability = result.getProbabilityEstimate()
print('Pf=', probability)

