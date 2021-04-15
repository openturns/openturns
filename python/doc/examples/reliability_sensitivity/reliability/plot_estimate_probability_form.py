"""
Use the FORM - SORM algorithms
==============================
"""
# %%
# In this example we estimate a failure probability with the `FORM` algorithm on the :ref:`cantilever beam <use-case-cantilever-beam>` example. More precisely, we show how to use the associated results:
#
# - the design point in both physical and standard space,
# - the probability estimation according to the FORM approximation, and the following SORM ones: Tvedt, Hohenbichler and Breitung,
# - the Hasofer reliability index and the generalized ones evaluated from the Breitung, Tvedt and Hohenbichler approximations,
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
# Model definition
# ----------------

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# We load the model from the usecases module :
from openturns.usecases import cantilever_beam as cantilever_beam
cb = cantilever_beam.CantileverBeam()

# %%
# We use the input parameters distribution from the data class :
distribution = cb.distribution
distribution.setDescription(['E', 'F', 'L', 'I'])

# %%
# We define the model
model = cb.model

# %%
# Create the event whose probability we want to estimate.

# %%
vect = ot.RandomVector(distribution)
G = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(G, ot.Greater(), 0.3)
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
print(result.getStandardSpaceDesignPoint())

# %%
# Design point in the physical X space.

# %%
print(result.getPhysicalSpaceDesignPoint())

# %%
# Importance factors
graph = result.drawImportanceFactors()
view = viewer.View(graph)

# %%
marginalSensitivity, otherSensitivity = result.drawHasoferReliabilityIndexSensitivity()
marginalSensitivity.setLegends(["E","F","L","I"])
marginalSensitivity.setLegendPosition('bottom')
view = viewer.View(marginalSensitivity)

# %%
marginalSensitivity, otherSensitivity = result.drawEventProbabilitySensitivity()
marginalSensitivity.setLegends(["E","F","L","I"])
marginalSensitivity.setLegendPosition('bottom')
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
print(sorm_result.getGeneralisedReliabilityIndexBreitung())

# %%
# ... with Hohenbichler approximation
sorm_result.getGeneralisedReliabilityIndexHohenbichler()

# %%
# .. with Tvedt approximation
print(sorm_result.getGeneralisedReliabilityIndexTvedt())

# %%
# SORM probability of the event with Breitung approximation
sorm_result.getGeneralisedReliabilityIndexTvedt()

# %%
# ... with Hohenbichler approximation
print(sorm_result.getEventProbabilityHohenbichler())

# %%
# ... with Tvedt approximation
print(sorm_result.getEventProbabilityTvedt())

plt.show()
