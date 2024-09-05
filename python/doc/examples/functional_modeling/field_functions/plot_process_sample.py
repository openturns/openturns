"""
Create a process sample from a sample
=====================================
"""

# %%
# In this example, a :class:`~openturns.ProcessSample` is created from a sample.
# The purpose is to illustrate how to create a :class:`~openturns.ProcessSample` from an already available sample of a field.
# In addition, the computation of statistics of the process sample is presented.
# The :class:`~openturns.ProcessSample` is defined over a :class:`~openturns.Mesh`.  
#
# For this example, the realizations of a stochastic process are obtained using the Chaboche model.
# A detailed explanation of this mechanical law is presented :ref:`here <use-case-chaboche>`.
# The model is defined such that:
# :math:`\sigma=g(\vect{X},\epsilon)`
# with :math:`\vect{X}=[R,C,\gamma]^T` and
#
# - :math:`R`: LogNormal(750e6,11e6)
# - :math:`C`: Normal(2750e6,250e6)
# - :math:`\gamma`: Normal(10.,2.)
# - the strain :math:`\epsilon\in[0.,0.07]`
# - the stress :math:`\sigma`
#

# %%
import openturns as ot
import openturns.viewer as viewer
from openturns.usecases import chaboche_model
import numpy as np

ot.Log.Show(ot.Log.NONE)

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
mesh = ot.RegularGrid(cm.strainMin, (cm.strainMax-cm.strainMin)/N, N)
vertices = mesh.getVertices()

# %%
# Generate one sample of the field
# --------------------------------

# %%
# One sample of the field is obtained by evaluating the Chaboche model on the mesh vertices for one specific realization of the input vector :math:`\vect{X}_0=[700\times 10^6,2500\times 10^6,8.]^T`.

# %%
R = 700e6  
C = 2500e6  
gamma = 8.0 
X0 = [R, C, gamma]
X_indices = [1, 2, 3]
f = ot.ParametricFunction(cm.model, X_indices, X0)
Y = [f(vertices[i])[0] for i in range(N)]

# %%
# Let's visualize this sample of the field.

# %%
graph = ot.Graph("One realization of the stochastic process", "Strain", "Stress (Pa)", True, "")
curve = ot.Curve(vertices.asPoint(),Y)
graph.add(curve)
viewer.View(graph)

# %%
# The distribution of the input vector :math:`\vect{X}` is defined.

# %%
R_distribution = ot.LogNormal()
R_distribution.setParameter(ot.LogNormalMuSigma()([750e6,11e6, 0.0]))
C_distribution = ot.Normal(2750e6,250e6)
gamma_distribution = ot.Normal(10.,2.)
X_distribution = ot.JointDistribution([R_distribution,C_distribution,gamma_distribution])

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
    f = ot.ParametricFunction(cm.model, X_indices, X_samples[i,:])
    Y = [[f(vertices[i])[0]] for i in range(N)]
    Y_list.append(Y)

# %%
# Let's visualize the 50 resulting samples of the field.

# %%
graph = ot.Graph("Realizations of the stochastic process", "Strain", "Stress (Pa)", True, "")
for i in range(n_samples):
    curve = ot.Curve(vertices,Y_list[i])
    curve.setColor("blue")
    graph.add(curve)
viewer.View(graph)

# %%
# Creation of the process sample
# ------------------------------

# %%
# It is possible to create a :class:`~openturns.ProcessSample` from the obtained field samples.
# For that, each obtained sample is added to the :class:`~openturns.ProcessSample` using the :class:`~openturns.Field` structure. When the :class:`~openturns.ProcessSample` is created, by default a process sample with a value of 0. for all the vertices is stored so it is important to remove it.

# %%
process_sample = ot.ProcessSample(mesh,n_samples,1)
process_sample.clear()
for i in range(n_samples):
    process_sample.add(ot.Field(mesh, Y_list[i]))

# %%
# It is then possible to compute different statistics on the :class:`~openturns.ProcessSample` such as the mean, median, variance, ...

# %%
process_sample_mean     = process_sample.computeMean()
process_sample_median   = process_sample.computeMedian()
process_sample_variance = process_sample.computeVariance()

# %%
# Let's visualize the mean of the process sample.

# %%
graph = ot.Graph("Sample process mean and realizations", "Strain", "Stress (Pa)", True, "")
for i in range(n_samples):
    if i==0:
        curve = ot.Curve(vertices,Y_list[i],"process samples")
        curve.setColor("blue")
        graph.add(curve)
    else:
        curve = ot.Curve(vertices,Y_list[i])
        curve.setColor("blue")
        graph.add(curve) 

curve = ot.Curve(vertices,process_sample_mean,"process sample mean")
curve.setColor("red")
graph.add(curve)
graph.setLegendPosition("topleft")
viewer.View(graph)
