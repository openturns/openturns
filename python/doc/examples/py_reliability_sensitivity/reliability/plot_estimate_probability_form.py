"""
Estimate a probability with FORM
================================
"""
# %%
# In this example we estimate a failure probability with the `FORM` algorithm on the cantilever beam example. More precisely, we show how to use the associated results:
#
# - the design point in both physical and standard space,
# - the probability estimation according to the FORM approximation, and the following SORM ones: Tvedt, Hohen-Bichler and Breitung,
# - the Hasofer reliability index and the generalized ones evaluated from the Breitung, Tvedt and Hohen-Bichler approximations,
# - the importance factors defined as the normalized director factors of the design point in the :math:`U`-space
# - the sensitivity factors of the Hasofer reliability index and the FORM probability.
# - the coordinates of the mean point in the standard event space.
#
# The coordinates of the mean point in the standard event space is:
#
# .. math::
#    \frac{1}{E_1(-\beta)}\int_{\beta}^{\infty} u_1 p_1(u_1)du_1
# 
#
# where :math:`E_1` is the spheric univariate distribution of the standard space and :math:`\beta` is the reliability index.

# %%
# Introduction 
# -------------
#
# Let us consider the analytical example of a cantilever beam with Young modulus E, length L and section modulus I.
#
# One end of the cantilever beam is built in a wall and we apply a concentrated bending load F at the other end of the beam, resulting in a deviation:
#
# .. math::
#    d = \frac{FL^3}{3EI}
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
distribution.setDescription(['E', 'F', 'L', 'I'])

# %%
# create the model
model = ot.SymbolicFunction(['E', 'F', 'L', 'I'], ['F*L^3/(3*E*I)'])

# %%
# Create the event whose probability we want to estimate.

# %%
vect = ot.RandomVector(distribution)
G = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(G, ot.Greater(), 30.0)
event.setName("deviation")

# %%
# Define a solver
optimAlgo = ot.Cobyla()
optimAlgo.setMaximumEvaluationNumber(1000)
optimAlgo.setMaximumAbsoluteError(1.0e-10)
optimAlgo.setMaximumRelativeError(1.0e-10)
optimAlgo.setMaximumResidualError(1.0e-10)
optimAlgo.setMaximumConstraintError(1.0e-10)

# %%
# Run FORM
algo = ot.FORM(optimAlgo, event, distribution.getMean())
algo.run()
result = algo.getResult()

# %%
# Probability
result.getEventProbability()

# %%
# Hasofer reliability index
result.getHasoferReliabilityIndex()

# %%
# Design point in the standard U* space.

# %%
result.getStandardSpaceDesignPoint()

# %%
# Design point in the physical X space.

# %%
result.getPhysicalSpaceDesignPoint()

# %%
# Importance factors
graph = result.drawImportanceFactors()
view = viewer.View(graph)

# %%
marginalSensitivity, otherSensitivity = result.drawHasoferReliabilityIndexSensitivity()
marginalSensitivity.setLegendPosition('bottom')
view = viewer.View(marginalSensitivity)

# %%
marginalSensitivity, otherSensitivity = result.drawEventProbabilitySensitivity()
view = viewer.View(marginalSensitivity)

# %%
# Error history
optimResult = result.getOptimizationResult()
graphErrors = optimResult.drawErrorHistory()
graphErrors.setLegendPosition('bottom')
graphErrors.setYMargin(0.0)
view = viewer.View(graphErrors)

# %%
# Get additional results with SORM
algo = ot.SORM(optimAlgo, event, distribution.getMean())
algo.run()
sorm_result = algo.getResult()

# %%
# Reliability index with Breitung approximation
sorm_result.getGeneralisedReliabilityIndexBreitung()

# %%
# ... with HohenBichler approximation
sorm_result.getGeneralisedReliabilityIndexHohenBichler()

# %%
# .. with Tvedt approximation
sorm_result.getGeneralisedReliabilityIndexTvedt()

# %%
# SORM probability of the event with Breitung approximation
sorm_result.getEventProbabilityBreitung()

# %%
# ... with HohenBichler approximation
sorm_result.getEventProbabilityHohenBichler()

# %%
# ... with Tvedt approximation
sorm_result.getEventProbabilityTvedt()

if not viewer._noshow:
    plt.show()
