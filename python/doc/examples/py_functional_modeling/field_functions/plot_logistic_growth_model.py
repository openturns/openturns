"""
Logistic growth model
=====================
"""
# %% 
#
# In this example, we use the logistic growth model in order to show how to define a function which has a vector input and a field output. We use the `OpenTURNSPythonPointToFieldFunction` class to define the derived class and its methods. 

# %%
# Introduction
# ------------
#
# The logistic growth model is the differential equation:
#
# .. math::
#    \frac{dy(t)}{dt} = ay(t) - by(t)^2
# 
#
# for any :math:`t\in[t_0, t_{final}]`, with the initial condition:
#
# .. math::
#    y(t_0) = y_0
# 
#
# where
#
# - :math:`a > 0` and :math:`b > 0` are two real parameters, 
# - :math:`y(t)` is the size of the population at time :math:`t`, 
# - :math:`t_0` is the initial time,
# - :math:`y_0` is the initial population at time :math:`t=t_0`, 
# - :math:`t_{final}` is the final time.
#
# The :math:`a` parameter sets the growth rate of the population. The :math:`b` parameter acts as a competition parameter which limits the size of the population by increasing the competition between its members. 
#
# In [1], the author uses this model to simulate the growth of the U.S. population. To do this, the author uses the U.S. census data from 1790 to 1910. For this time interval, R. Pearl and L. Reed [2] computed the following values of the parameters:
#
# .. math::
#    a = 0.03134, \qquad b = 1.5887 \times 10^{-10}.
#
#
# Our goal is to use the logistic growth model in order to simulate the solution for a larger time interval, from 1790 to 2000:
#
# .. math::
#    t_0 = 1790, \qquad t_{final} = 2000.
# 
#
# Then we can compare the predictions of this model with the real evolution of the U.S. population.
#
# We can prove that, if :math:`y_0 > 0`, then the limit population is:
#
# .. math::
#    y_{limit} =\frac{a}{b}.
# 
#
# In 1790, the U.S. population was 3.9 Millions inhabitants:
#
# .. math::
#    y_0 = 3.9 \times 10^6.
# 
#
# We can prove that the exact solution of the ordinary differential equation is:
#
# .. math::
#    y(t)=\frac{ay_0}{by_0+(a-by_0 ) \exp(-a(t-t_0)) }
# 
#
# for any :math:`t\in[t_0, t_{final}]`.
#
# We want to see the solution of the ordinary differential equation when uncertainties are taken into account in the parameters:
#
# - the initial U.S. population :math:`y_0`,
# - the parameters :math:`a` and :math:`b`.
#
# Indeed, Pearl and Reed [2] estimated the parameters :math:`a` and :math:`b` using the U.S. census data from 1790 to 1910 while we have the data up to 2000. Moreover, the method used by Pearl and Reed to estimate the parameters could be improved; they only used 3 dates to estimate the parameters instead of using least squares, for example. Finally, Pearl and Reed did not provide confidence intervals for the parameters :math:`a` and :math:`b`. 

# %%
#
# Normalizing the data
# --------------------
#
# The order of magnitude of the population is :math:`10^6`. This is why we consider the normalized population in millions:
#
# .. math::
#    z(t) = \frac{y(t)}{10^6}
# 
#
# for any :math:`t\in[t_0, t_{final}]`.
#
# Let :math:`z_0` be the initial population:
#
# .. math::
#    z_0 = z(t_0).
# 

# %%
# Uncertainties
# -------------
#
# Uncertainty can be accounted for by turning :math:`z_0`, :math:`a` and :math:`c` into independent random variables :math:`Z_0`, :math:`A` and :math:`C` with Gaussian distributions. From this point onward, :math:`z_0`, :math:`a` and :math:`b` respectively denote :math:`\mathbb{E}[Z_0]`, :math:`\mathbb{E}[A]` and :math:`\mathbb{E}[C]`.
#
# =============  ============================================================
# Variable       Distribution
# =============  ============================================================
#  :math:`Z_0`    gaussian, mean :math:`z_0`, coefficient of variation 10%
#  :math:`A`      gaussian, mean :math:`a`, coefficient of variation 30%
#  :math:`C`      gaussian, mean :math:`c`, coefficient of variation 30%
# =============  ============================================================
#
# No particular probabilistic method was used to set these distributions. An improvement would be to use calibration methods to get a better quantification of these distributions. An improvement would be to use calibration methods to get a better quantification of these distributions. 

# %%
# Notes
# -----
#
# * This example is based on [1], chapter "First order differential equations", page 28. 
# The data used in [1] are from [3]. 
# The logistic growth model was first suggested by Pierre François Verhulst near 1840. 
# The data are from [1] for the time interval from 1790 to 1950, then from [2] for the time interval from 1960 to 2000.
# * Calibrating this model may require to take into account for the time dependency of the measures.

# %%
# References
# ----------
#
# [1] Martin Braun. Differential equations and their applications, Fourth Edition. Texts in applied
# mathematics. Springer, 1993.
#
# [2] Cleve Moler. Numerical Computing with Matlab. Society for Industrial Applied Mathematics,
# 2004.
#
# [3] Raymond Pearl and Lowell Reed. On the rate of growth of the population of the united states
# since 1790 and its mathematical representation. Proceedings of the National Academy of Sciences,
# 1920.

# %%
# Define the model
# ----------------

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
from numpy import linspace, exp, maximum

# %%
# The data is based on 22 dates from 1790 to 2000.

# %%
ustime=list(range(1790,2001,10))
uspop=[3.9,5.3,7.2,9.6,13.,17.,23.,31.,39., 50.,62.,76.,92.,106.,123.,132.,151.,179., 203.,221.,250.,281.]

# %%
# Creation of the input distribution.

# %%
y0 = 3.9e6
a = 0.03134
b = 1.5887e-10
distY0 = ot.Normal(y0, 0.1 * y0)
distA  = ot.Normal(a, 0.3 * a)
distB  = ot.Normal(b, 0.3 * b)
distX = ot.ComposedDistribution([distY0, distA, distB])


# %%
# The model.

# %%
class Popu(ot.OpenTURNSPythonPointToFieldFunction):

    def __init__(self, t0 = 1790.0, tfinal = 2000.0, nt = 1000):
        grid = ot.RegularGrid(t0, (tfinal - t0) / (nt - 1), nt)
        super(Popu, self).__init__(3, grid, 1)
        self.setInputDescription(['y0', 'a', 'b'])
        self.setOutputDescription(['N'])
        self.ticks_ = [t[0] for t in grid.getVertices()]
        self.phi_ = ot.SymbolicFunction(['t', 'y', 'a', 'b'], ['a*y - b*y^2'])
        
    def _exec(self, X):
        y0 = X[0]
        a  = X[1]
        b  = X[2]
        phi_ab = ot.ParametricFunction(self.phi_, [2, 3], [a, b])
        phi_t = ot.ParametricFunction(phi_ab, [0], [0.0])
        solver = ot.RungeKutta(phi_t)
        initialState = [y0]
        values = solver.solve(initialState, self.ticks_)
        return values * [1.0e-6]

F = Popu(1790.0, 2000.0, 1000)
popu = ot.PointToFieldFunction(F)

# %%
# Generate a sample from the model
# --------------------------------

# %%
# Sample from the model

# %%
size = 10
inputSample = distX.getSample(size)
outputSample = popu(inputSample)

# %%
ot.ResourceMap.SetAsUnsignedInteger('Drawable-DefaultPalettePhase', size)

# %%
# Draw some curves

# %%
graph = outputSample.drawMarginal(0)
graph.setTitle('US population')
graph.setXTitle(r':math:`t` (years)')
graph.setYTitle(r':math:`N` (millions)')
cloud = ot.Cloud(ustime, uspop)
cloud.setPointStyle('circle')
cloud.setLegend('Data')
graph.add(cloud)
graph.setLegendPosition('topleft')
view = viewer.View(graph)
plt.show()

