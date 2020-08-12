"""
Probability estimation: directional simulation
==============================================
"""
# %%
# In this example we estimate a failure probability with the directional simulation algorithm provided by the `DirectionalSampling` class.

# %%
# Introduction
# ------------
#
# The directional simulation algorithm  operates in the standard space based on:
#
# 1. a *root strategy* to evaluate the nearest failure point along each direction and take the contribution of each direction to the failure event probability into account. The available strategies are:
#    - `RiskyAndFast`
#    - `MediumSafe`
#    - `SafeAndSlow`
#  
# 2. a *sampling strategy* to choose directions in the standard space. The available strategies are:
#    - `RandomDirection`
#    - `OrthogonalDirection`
#
# Let us consider the analytical example of a cantilever beam with Young modulus E, length L and section modulus I.
#
# One end is built in a wall and we apply a concentrated bending load F at the other end of the beam, resulting in a deviation:
#
# .. math::
#    d = \frac{F L^3}{3 E I}
# 
#
# Failure occurs when the beam deviation is too large:
#
# .. math::
#    d \ge 30 (cm)
# 
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
# Create the copula.

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
# Root finding algorithm.

# %%
solver = ot.Brent()
rootStrategy = ot.MediumSafe(solver)

# %%
# Direction sampling algorithm.

# %%
samplingStrategy = ot.OrthogonalDirection()

# %%
# Create a simulation algorithm.

# %%
algo = ot.DirectionalSampling(event, rootStrategy, samplingStrategy)
algo.setMaximumCoefficientOfVariation(0.1)
algo.setMaximumOuterSampling(40000)
algo.setConvergenceStrategy(ot.Full())
algo.run()

# %%
# Retrieve results.

# %%
result = algo.getResult()
probability = result.getProbabilityEstimate()
print('Pf=', probability)
