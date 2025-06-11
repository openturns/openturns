"""
Define a function with a field output: the viscous free fall example
====================================================================
"""

# %%
# In this example, we define a function which has a vector input and a field output.
# This is why we use the :class:`~openturns.PythonPointToFieldFunction` class to create the associated function and propagate the uncertainties through it.
#
# We consider a viscous free fall as explained :ref:`here <use-case-viscous-fall>`.

# %%
# Define the model
# ----------------

# %%
import openturns as ot
import openturns.viewer as viewer
from openturns.usecases import viscous_free_fall


# %%
# Load the viscous free fall example.
vff = viscous_free_fall.ViscousFreeFall()
distribution = vff.distribution
model = vff.model

# %%
# Sample trajectories
# -------------------

# %%
# In order to sample trajectories, we use the `getSample` method of the input distribution and apply the field function.

# %%
size = 10
inputSample = distribution.getSample(size)
outputSample = model(inputSample)

# %%
ot.ResourceMap.SetAsUnsignedInteger("Drawable-DefaultPalettePhase", size)

# %%
# Draw some curves.

# %%
graph = outputSample.drawMarginal(0)
graph.setTitle("Viscous free fall: %d trajectories" % (size))
graph.setXTitle(r"$t$")
graph.setYTitle(r"$z$")
view = viewer.View(graph)

# %%
# We see that the object first moves up and then falls down.
# Not all objects, however, achieve the same maximum altitude.
# We see that some trajectories reach a higher maximum altitude than others.
# Moreover, at the final time :math:`t_{max}`, one trajectory hits the ground: :math:`z(t_{max})=0` for this trajectory.
viewer.View.ShowAll()
