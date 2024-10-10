"""
Estimate Sobol' indices for the beam by simulation algorithm
============================================================
"""
# %%
#
# In this example, we estimate the Sobol' indices for the
# :ref:`cantilever beam <use-case-cantilever-beam>` by simulation algorithm.
#

# %%
# Introduction
# ------------
#
# In this example we are going to quantify the correlation between the input
# variables and the output variable of a model thanks to Sobol indices.
#
# Sobol indices are designed to evaluate the importance of a single variable
# or a specific set of variables.
# Here the Sobol indices are estimated by sampling from the distributions of
# the input variables and propagating uncertainty through a function.
#
# In theory, Sobol indices range from 0 to 1; the closer an index value is
# to 1, the better the associated input variable explains the function output.
#
# Let us denote by :math:`d` the input dimension of the model.
#
# Sobol' indices can be computed at different orders.
#
# * First order indices evaluate the importance of one input variable
#   at a time.
#
# * Total indices give the relative importance of one input variable
#   and all its interactions with other variables.
#   Alternatively, they can be viewed as measuring how much wriggle room
#   remains to the output when all but one input variables are fixed.
#
# * In general, we are only interested in first order and total Sobol' indices.
#   There are situations, however, where we want to estimate interactions.
#   Second order indices evaluate the importance of every pair of input variables.
#   The number of second order indices is:
#
# .. math::
#    \binom{d}{2} = \frac{d \times \left( d-1\right)}{2}.
#
# In practice, when the number of input variables is not small (say,
# when :math:`d>5`), then the number of second order indices is too large
# to be easily analyzed.
# In these situations, we limit the analysis to the first order and total
# Sobol' indices.

# %%
# Define the model
# ----------------

# %%
from openturns.usecases import cantilever_beam
import openturns as ot
import openturns.viewer as otv

ot.Log.Show(ot.Log.NONE)

# %%
# We load the distribution and model from the example:
beam = cantilever_beam.CantileverBeam()
distribution = beam.independentDistribution
model = beam.model

# %%
# Estimate the Sobol' indices
# ---------------------------

# %%
# We first create a design of experiments with the `SobolSimulationAlgorithm`.
# Since we are not interested in second order indices for the moment,
# we use the default value of the third argument (we will come back to this
# topic later).

# %%
estimator = ot.SaltelliSensitivityAlgorithm()
estimator.setUseAsymptoticDistribution(True)
algo = ot.SobolSimulationAlgorithm(distribution, model, estimator)
algo.setMaximumOuterSampling(50)  # number of iterations
algo.setBlockSize(1000)  # size of Sobol experiment at each iteration
algo.setBatchSize(4)  # number of points evaluated simultaneously
# algo.setIndexQuantileLevel(0.05)  # alpha
# algo.setIndexQuantileEpsilon(1e-2)  # epsilon
algo.run()

# %%
# Extract the results
# -------------------
result = algo.getResult()
fo = result.getFirstOrderIndicesEstimate()
to = result.getTotalOrderIndicesEstimate()
foDist = result.getFirstOrderIndicesDistribution()
graph = result.draw()
_ = otv.View(graph)

# %%
# Using a different estimator
# ---------------------------
#
# We have used the `SaltelliSensitivityAlgorithm` class to estimate the indices.
# Others are available in the library:
#
# * `SaltelliSensitivityAlgorithm`
# * `MartinezSensitivityAlgorithm`
# * `JansenSensitivityAlgorithm`
# * `MauntzKucherenkoSensitivityAlgorithm`
#

# %%
otv.View.ShowAll()
