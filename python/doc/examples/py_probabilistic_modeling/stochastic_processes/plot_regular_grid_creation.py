"""
Creation of a regular grid
==========================
"""
# %%
# In this example we will demonstrate how to create a regular grid. Note that a regular grid is a particular mesh of :math:`\mathcal{D}=[0,T] \in \mathbb{R}`.
#
# Here we will assume it represents the time :math:`t` as it is often the case, but it can represent any physical quantity.
#
# A regular time grid is a regular discretization of the interval :math:`[0, T] \in \mathbb{R}` into :math:`N` points, noted :math:`(t_0, \dots, t_{N-1})`.
#
# The time grid can be defined using :math:`(t_{Min}, \Delta t, N)` where :math:`N` is the number of points in the time grid. :math:`\Delta t` the time step between two consecutive instants and :math:`t_0 = t_{Min}`. Then,  :math:`t_k = t_{Min} + k \Delta t` and :math:`t_{Max} = t_{Min} +  (N-1) \Delta t`.
#
#
# Consider :math:`X: \Omega \times \mathcal{D} \rightarrow \mathbb{R}^d` a multivariate stochastic process of dimension :math:`d`,  where :math:`n=1`, :math:`\mathcal{D}=[0,T]` and :math:`t\in \mathcal{D}` is interpreted as a time stamp. Then the mesh associated to the process :math:`X` is a (regular) time grid.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m

# %%
tMin = 0.
tStep = 0.1
n = 10

# Create the grid
time_grid = ot.RegularGrid(tMin, tStep, n)

# %%
# Get the first and the last instants,
# the step and the number of points
start = time_grid.getStart()
step = time_grid.getStep()
grid_size = time_grid.getN()
end = time_grid.getEnd()
print('start=', start, 'step=', step, 'grid_size=', grid_size, 'end=', end)

# %%
# draw the grid
time_grid.setName('time')
graph = time_grid.draw()
view = viewer.View(graph)
plt.show()
