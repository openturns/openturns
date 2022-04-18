"""
Use the Smolyak quadrature
===========================
"""
# %%
# The goal of this example is to use Smolyak's quadrature. 

# %%
import openturns as ot
import openturns.viewer as otv
from matplotlib import pylab as plt

# %%
# In the first example, we print the nodes and weights 
# Smolyak-Legendre quadrature of level 3.
 
uniform = ot.GaussProductExperiment(ot.Uniform(-1.0, 1.0))
collection = [uniform] * 2
level = 3
print("level = ", level)
experiment = ot.SmolyakExperiment(collection, level)
nodes, weights = experiment.generateWithWeights()
print(nodes)
print(weights)

# %%
# We see that some of the weights are nonpositive. This is a significant 
# difference with tensorized Gauss quadrature. 

# %%
# We now create an exponential product problem from 
# [morokoff1995]_, table 1, page 221.
# This example is also used in [gerstner1998]_ page 221 to demonstrate 
# the properties of Smolyak's quadrature. 
# It is defined by the equation :
# 
# .. math::
# 
#     g(\boldsymbol{x}) = (1 + 1 / d)^d * \prod_{i = 1}^d x_i^(1 / d)
# 
# for any :math:`\boldsymbol{x} \in [0, 1]^d` where :math:`d` is the 
# dimension of the problem. 
#
# We are interested in the integral:
# 
# .. math::
# 
#     I = \int_{[0, 1]^d} g(\boldsymbol{x}) f(\boldsymbol{x}) d \boldsymbol{x}
#
# where :math:`f(\boldsymbol{x}) = 1` is the uniform density probability 
# function in the :math:`[0, 1]^d` interval. 
#
# When the dimension increases, the variance goes to zero, 
# but the variation in the sense of Hardy and Krause goes to infinity.
# 

dimension = 4
def g_function_py(x):
    value = (1.0 + 1.0 / dimension) ** dimension
    for i in range(dimension):
        value *= x[i] ** (1.0 / dimension)
    return [value]

g_function = ot.PythonFunction(dimension, 1, g_function_py)
interval = ot.Interval([0.0] * dimension, [1.0] * dimension)
integral = 1.0
name = "ExponentialProduct"

