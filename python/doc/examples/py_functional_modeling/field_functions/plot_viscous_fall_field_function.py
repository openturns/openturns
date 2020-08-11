"""
Define a function with a field output: the viscous free fall example
====================================================================
"""
# %%
# In this example, we define a function which has a vector input and a field output. This is why we use the `PythonPointToFieldFunction` class to create the associated function and propagate the uncertainties through it.

# %%
# Introduction 
# -------------
#
# We consider an object inside a vertical cylinder which contains a viscous fluid. The fluid generates a drag force which limits the speed of the solid and we assume that the force depends linearily on the object speed:
#
# .. math::
#    m \frac{dv}{dt} = - m g - c v
# 
#
# for any :math:`t \in [0, t_{max}]` where:
#
# - :math:`v` is the speed :math:`[m/s]`,
# - :math:`t` is the time :math:`[s]`,
# - :math:`t_{max}` is the maximum time :math:`[s]`,
# - :math:`g` is the gravitational acceleration :math:`[m.s^{-2}]`,
# - :math:`m` is the mass :math:`[kg]`,
# - :math:`c` is the linear drag coefficient :math:`[kg.s^{-1}]`.
#
# The previous differential equation has the exact solution:
#
# .. math::
#    z(t) = z_0 + v_{inf} t + \tau (v_0 - v_{inf})\left(1 - e^{-\frac{t}{\tau}}\right)
# 
#
# for any :math:`t \in [0, t_{max}]`
#  
# where:
#
# - :math:`z` is the altitude above the surface :math:`[m]`,
# - :math:`z_0` is the initial altitude :math:`[m]`,
# - :math:`v_0` is the initial speed (upward) :math:`[m.s^{-1}]`,
# - :math:`v_{inf}` is the limit speed :math:`[m.s^{-1}]`:
#
# .. math::
#    v_{inf}=-\frac{m g}{c}
# 
#
# - :math:`\tau` is time caracteristic :math:`[s]`:
#
# .. math::
#    \tau=\frac{m}{c}.
# 
#
# The stationnary speed limit at infinite time is equal to :math:`v_{inf}`:
#
# .. math::
#    \lim_{t\rightarrow+\infty} v(t)= v_{inf}. 
# 
#
# When there is no drag, i.e. when :math:`c=0`, the trajectory depends quadratically on :math:`t`:
#
# .. math::
#    z(t) = z_0 + v_0 t -g t^2
# 
#
# for any :math:`t \in [0, t_{max}]`.
#
# Furthermore when the solid touches the ground, we ensure that the altitude remains nonnegative i.e. the final altitude is:
#
# .. math::
#    y(t) = \max(z(t),0)
# 
#
# for any :math:`t \in [0, t_{max}]`.
#
# Probabilistic model
# -------------------
#
# The parameters :math:`z_0`, :math:`v_0`, :math:`m` and :math:`c` are probabilistic:
#
# - :math:`z_0 \sim \mathcal{U}(100, 150)`,
# - :math:`v_0 \sim \mathcal{N}(55, 10)`,
# - :math:`m \sim \mathcal{N}(80, 8)`,
# - :math:`c \sim \mathcal{U}(0, 30)`.

# %%
# References
# ----------
#
# * Steven C. Chapra. Applied numerical methods with Matlab for engineers and scientists, Third edition. 2012. Chapter 7, "Optimization", p.182.

# %%
# Define the model
# ----------------

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import numpy as np

# %%
# We first define the time grid associated with the model. 

# %%
tmin=0.0 # Minimum time
tmax=12. # Maximum time
gridsize=100 # Number of time steps
mesh = ot.IntervalMesher([gridsize-1]).build(ot.Interval(tmin, tmax))

# %%
# The `getVertices` method returns the time values in this mesh.

# %%
vertices = mesh.getVertices()
vertices[0:5]

# %%
# Creation of the input distribution.

# %%
distZ0 = ot.Uniform(100.0, 150.0)
distV0 = ot.Normal(55.0, 10.0)
distM = ot.Normal(80.0, 8.0)
distC = ot.Uniform(0.0, 30.0)
distribution = ot.ComposedDistribution([distZ0, distV0, distM, distC])

# %%
dimension = distribution.getDimension()
dimension


# %%
# Then we define the Python function which computes the altitude at each time value. In order to compute all altitudes with a vectorized evaluation, we first convert the vertices into a `numpy` `array` and use the `numpy` function `exp` and `maximum`: this increases the evaluation performance of the script. 

# %%
def AltiFunc(X):
    g  = 9.81
    z0 = X[0]
    v0 = X[1]
    m  = X[2]
    c  = X[3]
    tau = m / c
    vinf = - m * g / c
    t = np.array(vertices)
    z = z0 + vinf * t + tau * (v0 - vinf) * (1 - np.exp( - t / tau))
    z = np.maximum(z,0.)
    return [[zeta[0]] for zeta in z]


# %%
# In order to create a `Function` from this Python function, we use the `PythonPointToFieldFunction` class. Since the altitude is the only output field, the third argument `outputDimension` is equal to `1`. If we had computed the speed as an extra output field, we would have set `2` instead.

# %%
outputDimension = 1
alti = ot.PythonPointToFieldFunction(dimension, mesh, outputDimension, AltiFunc)

# %%
# Sample trajectories
# -------------------

# %%
# In order to sample trajectories, we use the `getSample` method of the input distribution and apply the field function.

# %%
size = 10
inputSample = distribution.getSample(size)
outputSample = alti(inputSample)

# %%
ot.ResourceMap.SetAsUnsignedInteger('Drawable-DefaultPalettePhase', size)

# %%
# Draw some curves.

# %%
graph = outputSample.drawMarginal(0)
graph.setTitle('Viscous free fall: %d trajectories' % (size))
graph.setXTitle(r'$t$')
graph.setYTitle(r'$z$')
view = viewer.View(graph)
plt.show()
# %%
# We see that the object first moves up and then falls down. Not all objects, however, achieve the same maximum altitude. We see that some trajectories reach a higher maximum altitude than others. Moreover, at the final time :math:`t_{max}`, one trajectory hits the ground: :math:`z(t_{max})=0` for this trajectory.
