"""
Visualize pairs between two samples
===================================
"""

# %%
# In this example we are going to a visualize the relationships between the marginal samples of two given samples.
# This is usual when we analyze the relationship of output sample of a model depending on input sample.
# This can be achieved by plotting the outputs versus the inputs.
# When there are several inputs (which is the most often encountered case) and several outputs (which is less often).
# The :meth:`~openturns.VisualTest.DrawPairs` method provides a tool to plot the pairs of input and output marginal samples and see the correlations graphically.

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# %%
# Create the model
model = ot.SymbolicFunction(
    ["X0", "X1", "X2"],
    ["1.0 + 2.0 * X0 - 1.0 * X1 + 4.0 * X2",
     "-2.0 - 3.0 * X0 + 5.0 * X1 - 1.0 * X2"],)

# %%
# Create the input data to visualize
distribution = ot.Normal(3)
distribution.setDescription(model.getInputDescription())
input_sample = distribution.getSample(100)

# %% Create the output data to visualize
output_sample = model(input_sample)

# %% Visualize the data
graph = ot.VisualTest.DrawPairs(input_sample, output_sample)
graph.setTitle("Clouds of input / output samples")
view = viewer.View(graph)
viewer.View.ShowAll()
