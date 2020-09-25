"""
Estimate a probability with FORM-v2
===================================
"""
# %%
# In this example we estimate a failure probability with the `FORM` algorithm on the cantilever beam example (see :ref:`use-case-cantilever-beam`). More precisely, we show how to use the associated results:
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
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
from use_cases import use_case_cantilever_beam

cb = use_case_cantilever_beam.cantilever_beam()

# %%
model = cb.model
distribution = cb.distribution

# %%
Y = cb.Y
event = ot.ThresholdEvent(Y, ot.Greater(), 30.0)
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
marginalSensitivity

# %%
# Error history
optimResult = result.getOptimizationResult()
graphErrors = optimResult.drawErrorHistory()
graphErrors.setLegendPosition('bottom')
graphErrors.setYMargin(0.0)
view = viewer.View(graphErrors)
graphErrors

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
