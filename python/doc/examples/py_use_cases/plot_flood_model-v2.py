"""
Estimate a flooding probability-v2
==================================
"""
# %% 
#
# In this example, we estimate the probability that the ouput of a function exceeds a given threshold with the FORM method. We consider the flooding model (see :ref:`use-case-flood-model`).

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
from use_cases import use_case_flood_model

# Load the flood model
fm = use_case_flood_model.flood_model()

# %%
model = fm.model
distribution = fm.distribution
event = fm.event

# %%
# Estimate the probability with FORM
# ----------------------------------

# %%
# Define a solver. 

# %%
optimAlgo = ot.Cobyla()
optimAlgo.setMaximumEvaluationNumber(1000)
optimAlgo.setMaximumAbsoluteError(1.0e-10)
optimAlgo.setMaximumRelativeError(1.0e-10)
optimAlgo.setMaximumResidualError(1.0e-10)
optimAlgo.setMaximumConstraintError(1.0e-10)

# %%
# Run FORM. 

# %%
startingPoint = distribution.getMean()
algo = ot.FORM(optimAlgo, event, startingPoint)
algo.run()
result = algo.getResult()
standardSpaceDesignPoint = result.getStandardSpaceDesignPoint()

# %%
# Retrieve results. 

# %%
result = algo.getResult()
probability = result.getEventProbability()
print('Pf=', probability)

# %%
# Importance factors. 

# %%
graph = result.drawImportanceFactors()
view = viewer.View(graph)
if not viewer._noshow:
    plt.show()
