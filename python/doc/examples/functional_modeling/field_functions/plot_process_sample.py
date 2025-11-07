"""
Create a process sample from a sample
=====================================
"""

# sphinx_gallery_thumbnail_number = 3
# %%

# %%
# In this example, a :class:`~openturns.ProcessSample` is created from data.
# The purpose is to illustrate how to create a :class:`~openturns.ProcessSample` from an already available sample of a field.
# In addition, the computation of statistics of the process sample is presented.
# The :class:`~openturns.ProcessSample` is defined over a :class:`~openturns.Mesh`.
#
# For this example, the realizations of a stochastic process are obtained using the Chaboche model.
# A detailed explanation of this mechanical law is presented :ref:`here <use-case-chaboche>`.
# In this example, the model is defined such that:
# :math:`\sigma=g(\vect{X},\epsilon)`
# with :math:`\vect{X}=[R,C,\gamma]^T` a vector of random variables and
#
# - the strain :math:`\epsilon\in[0.,0.07]`
# - the stress :math:`\sigma`
#

# %%
import openturns as ot
import openturns.viewer as otv
from openturns.usecases import chaboche_model


# %%
# Define the model and the mesh
# -----------------------------

# %%
# First, the Chaboche model is loaded.

# %%
cm = chaboche_model.ChabocheModel()

# %%
# Then, a mesh using a :class:`~openturns.RegularGrid` is defined for the strain :math:`\epsilon\in[0.,0.07]` with :math:`N=100` vertices.

# %%
N = 100
mesh = ot.RegularGrid(cm.strainMin, (cm.strainMax - cm.strainMin) / N, N)
vertices = mesh.getVertices()

# %%
# Generate one sample of the field
# --------------------------------

# %%
# One sample of the field is obtained by evaluating the Chaboche model on the mesh vertices
# for one specific realization of the input vector :math:`\vect{X}_0=[700\times 10^6,2500\times 10^6,8.]^T`.

# %%
R = 700e6
C = 2500e6
gamma = 8.0
X0 = [R, C, gamma]
X_indices = [1, 2, 3]
f = ot.ParametricFunction(cm.model, X_indices, X0)
Y = f(vertices)

# %%
# Let's visualize this sample of the field.

# %%
graph = ot.Graph(
    "One realization of the stochastic process", "Strain", "Stress (Pa)", True, ""
)
curve = ot.Curve(vertices, Y)
graph.add(curve)
view = otv.View(graph)

# %%
# The distribution of the input vector :math:`\vect{X}` is defined here:
param_R = ot.LogNormalMuSigma(750e6, 11e6, 0.0)
dist_R = ot.ParametrizedDistribution(param_R)
dist_C = ot.Normal(2750e6, 250e6)
dist_gamma = ot.Normal(10, 2)
X_distribution = ot.JointDistribution([dist_R, dist_C, dist_gamma])

# %%
# Generate different samples of the field
# ---------------------------------------

# %%
# 50 samples of the input vector :math:`\vect{X}` are generated.

# %%
n_samples = 50
X_samples = X_distribution.getSample(n_samples)

# %%
# The Chaboche model is evaluated for each of the input vector samples.

# %%
Y_list = []
for i in range(n_samples):
    f = ot.ParametricFunction(cm.model, X_indices, X_samples[i, :])
    Y = f(vertices)
    Y_list.append(Y)

# %%
# Let's visualize the 50 resulting samples of the field.

# %%
graph = ot.Graph(
    "Realizations of the stochastic process", "Strain", "Stress (Pa)", True, ""
)
for i in range(n_samples):
    curve = ot.Curve(vertices, Y_list[i])
    curve.setColor("blue")
    graph.add(curve)
view = otv.View(graph)

# %%
# Creation of the process sample
# ------------------------------

# %%
# It is possible to create a :class:`~openturns.ProcessSample` from the obtained field samples.
# For that, each obtained sample is added to the :class:`~openturns.ProcessSample` using the :class:`~openturns.Field` structure.
# When the :class:`~openturns.ProcessSample` is created, by default a process sample with a value of 0.
# for all the vertices is stored so it is important to remove it.

# %%
process_sample = ot.ProcessSample(mesh, n_samples, 1)
process_sample.clear()
for i in range(n_samples):
    process_sample.add(ot.Field(mesh, Y_list[i]))

# %%
# It is then possible to compute different statistics on the :class:`~openturns.ProcessSample` such as the mean, median, variance, ...

# %%
process_sample_mean = process_sample.computeMean()
process_sample_median = process_sample.computeMedian()
process_sample_variance = process_sample.computeVariance()

# %%
# Let's visualize the mean of the process sample.

# %%
graph = ot.Graph(
    "Sample process mean and realizations", "Strain", "Stress (Pa)", True, ""
)
for i in range(n_samples):
    if i == 0:
        label = "process samples"
    else:
        label = ""
    curve = ot.Curve(vertices, Y_list[i], label)
    curve.setColor("blue")
    graph.add(curve)

curve = ot.Curve(vertices, process_sample_mean, "process sample mean")
curve.setColor("red")
graph.add(curve)
graph.setLegendPosition("topleft")
view = otv.View(graph)

# %%
# Display all figures

# %%
otv.View.ShowAll()
