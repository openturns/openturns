"""
Sobol' sensitivity indices using rank-based algorithm
=====================================================
"""

import openturns as ot
import openturns.viewer as otv
from openturns.usecases import ishigami_function

# %%
# In this example we are going to compute global sensitivity indices in a data driven framework.
# For that purpose, we are using the rank-based algorithm which provides the first order Sobol' indices.
# We present the method on the :ref:`Ishigami function<use-case-ishigami>`.


# %%
# Definition of the model
# -----------------------
#
# We load the model from the usecases module.
im = ishigami_function.IshigamiModel()

# %%
# We generate an input sample of size 500 (and dimension 3).
size = 500
ot.RandomGenerator.SetSeed(10)
X = im.distribution.getSample(size)

# %%
# We compute the output by applying the Ishigami model to the input sample.
Y = im.model(X)

# %%
# Estimate Sobol' first order indices via rank-based algorithm.
mySobol = ot.RankSobolSensitivityAlgorithm(X, Y)
indices = mySobol.getFirstOrderIndices()
print("First order indices:", indices)

# %%
# Draw Sobol' indices.
#
# Options for confidence interval estimation can be defined.
ot.ResourceMap.SetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize", 200)
ot.ResourceMap.SetAsScalar(
    "RankSobolSensitivityAlgorithm-DefaultBootstrapSampleRatio", 0.85
)

# %%
# if `SobolIndicesAlgorithm-DefaultBootstrapSize` > 1, the draw() method will call bootstrap method to compute the confidence intervals.

graph = mySobol.draw()
graph.setTitle("Sobol' indices")
view = otv.View(graph)

print("confidence intervals:", mySobol.getFirstOrderIndicesInterval())

# %%
# It is possible that Sobol' indices estimates take negative values, that is inconsistent with the theory. Therefore, a larger number of samples is required to get consistent indices.
