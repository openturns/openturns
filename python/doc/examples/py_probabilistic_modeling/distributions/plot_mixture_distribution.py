"""
Create a mixture of PDFs
========================
"""
# %%
# In this example we are going to build a distribution whose PDF is defined by a linear combination of probability density functions:
#
# .. math::
#    f(x) =  \sum_{i=1}^N \alpha_i p_i(x), \quad \alpha_i \geq 0, \quad \sum_i \alpha_i = 1
#   
# The weigths are automatically normalized.
#
# It is also possible to create a mixture of copulas.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# create a collection of distribution and the associated weights
distributions = [ot.Triangular(1.0, 2.0, 4.0), ot.Normal(-1.0, 1.0), ot.Uniform(5.0, 6.0)]
weights = [0.4, 1.0, 0.2]

# %%
# create the mixture
distribution = ot.Mixture(distributions, weights)
print(distribution)

# %%
# draw PDF
graph = distribution.drawPDF()
view = viewer.View(graph)

# %%
# define a list of copulas and the associated weights
copulas = [ot.GumbelCopula(4.5), ot.ClaytonCopula(2.3)]
weights = [0.2, 0.8]

# %%
# create a mixture of copulas
distribution = ot.Mixture(copulas, weights)
print(distribution)

# %%
# draw PDF
graph = distribution.drawPDF()
view = viewer.View(graph)
if not viewer._noshow:
    plt.show()
