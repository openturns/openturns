"""
Compute the L2 error between two functions
==========================================
"""
# %%

# %%
# In this example we compute the L2 error between two functions.

# %%
import openturns as ot
import openturns.experimental as otexp
from openturns.usecases import ishigami_function

# %%
# This is easy using the arithmetic operator `-` on two
# instances of the :class:`~openturns.Function` class.

# %%
im = ishigami_function.IshigamiModel()
partOfIshigamiFunction = ot.SymbolicFunction(
    ["x1", "x2", "x3"], ["7 * (sin(x2))^2 + 0.1 * x3^4 * sin(x1)"]
)
ot.RandomGenerator.SetSeed(0)
sampleSize = 1000
experiment = ot.MonteCarloExperiment(im.distributionX, sampleSize)
integration = otexp.ExperimentIntegration(experiment)
functionError = integration.computeL2Norm(im.model - partOfIshigamiFunction)
print(functionError)
