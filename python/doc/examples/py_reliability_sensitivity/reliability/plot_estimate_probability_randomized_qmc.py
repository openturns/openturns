# ---
# jupyter:
#   jupytext:
#     text_representation:
#       extension: .py
#       format_name: percent
#       format_version: '1.3'
#       jupytext_version: 1.5.1
#   kernelspec:
#     display_name: Python 3
#     language: python
#     name: python3
# ---

# %%
"""
Estimate a probability using randomized QMC
===========================================
"""
# %% 

# %%
# In this example we are going to estimate a failure probability.

# %%
# Introduction
# ------------
#
# Let us consider the following analytical example of a cantilever beam with Young's modulus E, length L and section modulus I.
#
# One end of the cantilever beam is built in a wall and we apply a concentrated bending load F at the other end of the beam, resulting in a deviation:
#
# .. math::
#    d = \frac{FL^3}{3EI}
# 
#
# Failure occurs when the beam deviation is too large:
#
# .. math::d \ge 30 (cm) :math:``
#   
# Four independent random variables are considered:
#
#  - E: Young's modulus [Pa]
#  - F: load [N]
#  - L: length [m]
#  - I: section [m^4]
#
# Stochastic model (simplified model, no units):
#
#  - E ~ Beta(0.93, 2.27, 2.8e7, 4.8e7)
#  - F ~ LogNormal(30000, 9000, 15000)
#  - L ~ Uniform(250, 260)
#  - I ~ Beta(2.5, 1.5, 3.1e2, 4.5e2)
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# Create the marginal distributions of the parameters.

# %%
dist_E = ot.Beta(0.93, 2.27, 2.8e7, 4.8e7)
dist_F = ot.LogNormalMuSigma(30000, 9000, 15000).getDistribution()
dist_L = ot.Uniform(250, 260)
dist_I = ot.Beta(2.5, 1.5, 3.1e2, 4.5e2)
marginals = [dist_E, dist_F, dist_L, dist_I]

# %%
# Create the Copula.

# %%
RS = ot.CorrelationMatrix(4)
RS[2, 3] = -0.2
# Evaluate the correlation matrix of the Normal copula from RS
R = ot.NormalCopula.GetCorrelationFromSpearmanCorrelation(RS)
# Create the Normal copula parametrized by R
copula = ot.NormalCopula(R) 

# %%
# Create the joint probability distribution.

# %%
distribution = ot.ComposedDistribution(marginals, copula)

# %%
# Create the model.

# %%
model = ot.SymbolicFunction(['E', 'F', 'L', 'I'], ['F*L^3/(3*E*I)'])

# %%
# Create the event whose probability we want to estimate.

# %%
vect = ot.RandomVector(distribution)
G = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(G, ot.Greater(), 30.0)

# %%
# Define the low discrepancy sequence.

# %%
sequence = ot.SobolSequence()

# %%
# Create a simulation algorithm.

# %%
experiment = ot.LowDiscrepancyExperiment(sequence, 1)
experiment.setRandomize(True)
algo = ot.ProbabilitySimulationAlgorithm(event, experiment)
algo.setMaximumCoefficientOfVariation(0.05)
algo.setMaximumOuterSampling(int(1e5))
algo.run()

# %%
# Retrieve results.

# %%
result = algo.getResult()
probability = result.getProbabilityEstimate()
print('Pf=', probability)
