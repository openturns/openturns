"""
Viscous free fall: metamodel of a field function
================================================
"""

# %%
#
# In this example, we present how to create the metamodel of a field function.
# This example considers the :ref:`free fall model <use-case-viscous-fall>`.
# We create the metamodel automatically using :class:`openturns.PointToFieldFunctionalChaosAlgorithm`
# and then also with a manual approach:
# We first compute the Karhunen-Loève decomposition of a sample of trajectories.
# Then we create a create a polynomial chaos which takes the inputs and returns
# the KL decomposition modes as outputs.
# Finally, we create a metamodel by
# combining the KL decomposition and the polynomial chaos.

# %%
# Define the model
# ----------------

# %%
import openturns as ot
import openturns.viewer as otv
from openturns.usecases import viscous_free_fall


# %%
# Load the viscous free fall example.
vff = viscous_free_fall.ViscousFreeFall()
distribution = vff.distribution
model = vff.model

# %%
# Generate a training sample.
size = 2000
inputSample = distribution.getSample(size)
outputSample = model(inputSample)

# %%
# Compute the global metamodel
algo = ot.PointToFieldFunctionalChaosAlgorithm(
    inputSample, outputSample, distribution
)
algo.run()
result = algo.getResult()
metaModel = result.getPointToFieldMetaModel()

# %%
# Validate the metamodel
# ----------------------

# %%
# Create a validation sample.

# %%
size = 10
validationInputSample = distribution.getSample(size)
validationOutputSample = model(validationInputSample)

# %%
graph = validationOutputSample.drawMarginal(0)
graph.setColors(["red"])
graph2 = metaModel(validationInputSample).drawMarginal(0)
graph2.setColors(["blue"])
graph.add(graph2)
graph.setTitle("Model/metamodel comparison")
graph.setXTitle(r"$t$")
graph.setYTitle(r"$z$")
view = otv.View(graph)

# %%
# We see that the blue trajectories (i.e. the metamodel) are close to the red
# trajectories (i.e. the validation sample).
# This shows that the metamodel is quite accurate.
# However, we observe that the trajectory singularity that occurs when the object
# touches the ground (i.e. when :math:`z` is equal to zero), makes the metamodel less accurate.

# %%
# Sensitivity analysis
# --------------------

# %%
# Compute the sensitivity indices
sensitivity = ot.FieldFunctionalChaosSobolIndices(result)
s1 = sensitivity.getFirstOrderIndices()
st = sensitivity.getTotalOrderIndices()

# %%
# We can notice that `v0` and `m` are the most influencial parameters
# and that there are almost no interactions (total indices being close to first order indices)
print(s1, st)

# %%
# Draw the sensitivity indices
graph = sensitivity.draw()
view = otv.View(graph)

# %%
# Manual approach
# ---------------

# %%
# Step 1: compute the KL decomposition of the output

# %%
algo = ot.KarhunenLoeveSVDAlgorithm(outputSample, 1.0e-6)
algo.run()
klResult = algo.getResult()
scaledModes = klResult.getScaledModesAsProcessSample()

# %%
graph = scaledModes.drawMarginal(0)
graph.setTitle("KL modes")
graph.setXTitle(r"$t$")
graph.setYTitle(r"$z$")
view = otv.View(graph)

# %%
# We create the lifting function which takes coefficients of the Karhunen-Loève (KL) modes as inputs and returns the trajectories.
klLiftingFunction = ot.KarhunenLoeveLifting(klResult)

# %%
# The `project` method computes the projection of the output sample (i.e. the trajectories) onto the KL modes.
outputSampleChaos = klResult.project(outputSample)

# %%
# Step 2: compute the metamodel of the KL modes

# We create a polynomial chaos metamodel which takes the input sample and returns the KL modes.
algo = ot.FunctionalChaosAlgorithm(inputSample, outputSampleChaos, distribution)
algo.run()
chaosMetamodel = algo.getResult().getMetaModel()

# %%
# The final metamodel is a composition of the KL lifting function and the polynomial chaos metamodel.
# We combine these two functions using the :class:`~openturns.PointToFieldConnection` class.

# %%
metaModel = ot.PointToFieldConnection(klLiftingFunction, chaosMetamodel)

# %%
otv.View.ShowAll()
