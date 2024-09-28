"""
Estimate a multivariate integral with IteratedQuadrature
========================================================
"""
# %%
#
# Introduction
# ------------
#
# In this example, we consider a function :math:`f: \Rset^d \mapsto \Rset`
# and we compute the following integral:
#
# .. math::
#    I_f = \int_{a}^{b}\, \int_{l_1(x_0)}^{u_1(x_0)}\, \int_{l_2(x_0, x_1)}^{u_2(x_0,x_1)}\, \int_{l_{d-1}(x_0, \dots, x_{d-2})}^{u_{d-1}(x_0, \dots, x_{d-2})}
#    \, f(x_0, \dots, x_{d-1})\mathrm{d}{x_{d-1}}\dots\mathrm{d}{x_0}
#
# using an iterated quadrature algorithm :class:`~openturns.IteratedQuadrature`, based on the Gauss-Kronrod
# :class:`~openturns.GaussKronrod` 1d quadrature algorithm.

import openturns as ot
import openturns.viewer as otv
import math as m

# %%
# To get better colours for the function iso-lines.
ot.ResourceMap.SetAsString("Contour-DefaultColorMapNorm", "rank")

# %%
# Case 1: The integrand function presents a lot of discontinuities.
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# We consider the function:
#
# .. math::
#    f : \begin{array}{lcl}
#          \Rset^2 & \rightarrow & \Rset \\
#          (x,y) & \rightarrow & 1_{{x^2+y^2 \leq 4}}(x,y)
#          \end{array}
#
# the domain :math:`\set{D} =[-2,2]\times [-2,2]` and the integral:
#
# .. math::
#    I = \int_{\set{D}} f(x,y)\,dxdy = 4\pi
#

# %%
# We first define the integrand :math:`f` and the domain :math:`\set{D}`.


def kernel1(x):
    if x[0]**2 + x[1]**2 <= 4:
        return [1.0]
    else:
        return [0.0]


integrand = ot.PythonFunction(2, 1, kernel1)
domain = ot.Interval([-2.0, -2.0], [2.0, 2.0])

# %%
# We draw the iso-lines of the integrand function which is
# constant equal to 1 inside the circle with radius equal to 2 and equal to 0.0 outside.
g = integrand.draw([-3.0, -3.0], [3.0, 3.0])
g.setTitle(r'Function $f(x,y) = 1_{\{x^2+y^2 \leq 4\}}(x,y)$')
g.setXTitle('x')
g.setYTitle('y')
view = otv.View(g)

# %%
# We compute the integral using an  :class:`~openturns.IteratedQuadrature`
# algorithm with
# the :class:`~openturns.GaussKronrodRule`  *G11K23*, which means that we use
# on each subinterval:
#
# - 11 points for the Gauss approximations,
# - 23 points for the Kronrod approximation including the 11 previous ones.
#
# We limit the number of subintervals and we  define a maximum error between both approximations:
# when the difference between both approximations is lower, a new subdivision is performed.
maximumSubIntervals = 32
maximumError = 1e-4
GKRule = ot.GaussKronrodRule(ot.GaussKronrodRule.G1K3)
integ_algo = ot.GaussKronrod(maximumSubIntervals, maximumError, GKRule)
integration_algo = ot.IteratedQuadrature(integ_algo)

# %%
# To get the nodes used to approximate the integral, we need to use a :class:`~openturns.MemoizeFunction`
# that stores all the calls to the function.
integrand_memoized = ot.MemoizeFunction(integrand)
I_value = integration_algo.integrate(integrand_memoized, domain)
print('I = ', I_value[0])
print('Exact value = ', 4*m.pi)
nodes = integrand_memoized.getInputHistory()
print('Nodes : ', nodes)

# %%
# We can draw in blue the nodes on the graph that contains the iso-lines of the
# function :math:`f`. We can see that the algorithm focuses on the nodes where the
# function has its discontinuities.
cloud_nodes = ot.Cloud(nodes)
cloud_nodes.setPointStyle('dot')
cloud_nodes.setColor('blue')

g.add(cloud_nodes)
g.setLegendPosition('')
view = otv.View(g)

# %%
# Case 2: The integrand function has large gradients.
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# We consider the function:
#
# .. math::
#
#    f : \begin{array}{lcl}
#          \Rset^2 & \rightarrow & \Rset \\
#          (x,y) & \rightarrow & e^{-(x^2+y^2)} + e^{-8((x-4)^2+(y-4)^2)}
#          \end{array}
#
# the domain :math:`\set{D} =[-2,2]\times [-2,2]` and the integral:
#
# .. math::
#    I = \int_{\set{D}} f(x,y)\,dxdy.
#
# Using Mapple, we obtain the reference value: 
#
# .. math::
#    I_{ref} = 3.51961338289448753812591427600

# %%
# We first define the integrand :math:`f` and the domain :math:`\set{D}`.


def kernel2(x):
    return [m.exp(-(x[0]**2 + x[1]**2)) + m.exp(-8 * ((x[0] - 4)**2 + (x[1] - 4)**2))]


integrand = ot.PythonFunction(2, 1, kernel2)
domain = ot.Interval([-2.0, -2.0], [6.0, 6.0])

# %%
# We draw the iso-lines of the integrand function.
g = integrand.draw([-3.0, -3.0], [7.0, 7.0])
g.setTitle(r'Function $f(x,y) = e^{-(x^2+y^2)} + e^{-8((x-4)^2+(y-4)^2)} $')
g.setXTitle('x')
g.setYTitle('y')

# %%
# To get the nodes used to approximate the integral, we need to use a :class:`~openturns.MemoizeFunction`
# that stores all the calls to the function.
integrand_memoized = ot.MemoizeFunction(integrand)
I_value = integration_algo.integrate(integrand_memoized, domain)
print('I = ', I_value[0])
nodes = integrand_memoized.getInputHistory()
print('Nodes : ', nodes)

# %%
# We can draw in blue the nodes on the graph that contains the iso-lines of the
# function :math:`f`. We can see that the algorithm focuses on the nodes where the function
# has fast variations.
cloud_nodes = ot.Cloud(nodes)
cloud_nodes.setPointStyle('dot')
cloud_nodes.setColor('blue')

g.add(cloud_nodes)
g.setLegendPosition('')
view = otv.View(g)


# %%
# Case 3: The integration domain is complex.
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# We consider the function:
#
# .. math::
#
#    f : \begin{array}{lcl}
#          \Rset^2 & \rightarrow & \Rset \\
#          (x,y) & \rightarrow & \cos(2x) \sin(1.5y)
#        \end{array}
#
# the domain :math:`\set{D} = \{(x,y)\, |\, -2\pi \leq x \leq 2\pi, -2 - \cos(x) \leq y  \leq 2 + \cos(x) \}`
# and the integral:
#
# .. math::
#    I = \int_{\set{D}} f(x,y)\,dxdy.
#
# Using Mapple, we obtain the reference value: 
#
# .. math::
#    I_{ref} = -0.548768615494004063851543284777

# %%
# We first define the integrand :math:`f` and the domain :math:`\set{D}`.


def kernel3(x):
    return [m.cos(2.0 * x[0]) * m.cos(1.5 * x[1])]


integrand = ot.PythonFunction(2, 1, kernel3)

# %%
# The domain of integration is defined by the two following functions:
#
# .. math::
#
#    x \rightarrow u(x) = 2 + \cos(x)
#
# .. math::
#
#    x \rightarrow  \ell(x) = -2 - \cos(x)
#
upper_func = ot.SymbolicFunction(["x"], [" 2 + cos(x)"])
lower_func = ot.SymbolicFunction(["x"], ["-2 - cos(x)"])

# %%
# We draw the iso-lines of the integrand function in the domain of integration.
# To do that, we define a new function which is the restriction
# of the integrand function to the integration domain.


def kernel3_insideDomain(x):
    low_x = lower_func([x[0]])[0]
    up_x = upper_func([x[0]])[0]
    if x[1] > low_x and x[1] < up_x :
        return kernel3(x)
    else:
        return [0.0]


integrand_domain = ot.PythonFunction(2, 1, kernel3_insideDomain)
a = 2 * m.pi
b = 4.0
# here, we ask for more levels than the default number store in :class:`~openturns.ResourceMap`.
ot.ResourceMap.SetAsUnsignedInteger("Contour-DefaultLevelsNumber", 30)
g = integrand_domain.draw([-a, -b], [a, b])

# %%
# We add the bounds of the domain :math:`\set{D}`.
low_bound = lower_func.draw([-a, -b], [a, b]).getDrawable(0)
up_bound = upper_func.draw([-a, -b], [a, b]).getDrawable(0)
low_bound.setColor('red')
low_bound.setLineWidth(2)
up_bound.setColor('red')
up_bound.setLineWidth(2)

g.add(low_bound)
g.add(up_bound)
g.setTitle(r'Function $f(x,y) =  cos(2x)sin(1.5y)$')
g.setXTitle('x')
g.setYTitle('y')
view = otv.View(g)

# %%
# To get the nodes used to approximate the integral, we need to use a :class:`~openturns.MemoizeFunction`
# that stores all the calls to the function.
integrand_memoized = ot.MemoizeFunction(integrand)
maximumSubIntervals = 4
maximumError = 1e-4
GKRule = ot.GaussKronrodRule(ot.GaussKronrodRule.G7K15)
integration_algo = ot.IteratedQuadrature(ot.GaussKronrod(maximumSubIntervals, maximumError, GKRule))
I_value = integration_algo.integrate(integrand_memoized, -a, a, [lower_func], [upper_func])
print('I = ', I_value[0])
nodes = integrand_memoized.getInputHistory()
print('Nodes : ', nodes)

# %%
# We can draw in black the nodes on the graph that contains the iso-lines of the
# function :math:`f`. We can see that the algorithm focuses on the nodes where
# the function has fast variations.
cloud_nodes = ot.Cloud(nodes)
cloud_nodes.setPointStyle('dot')
cloud_nodes.setColor('black')

g.add(cloud_nodes)
g.setLegendPosition('')
view = otv.View(g)

# %%
# Reset ResourceMap
ot.ResourceMap.Reload()

# %%
# Show all the graphs.
view.ShowAll()
