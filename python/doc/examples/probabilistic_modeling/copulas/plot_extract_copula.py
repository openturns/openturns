"""
Extract the copula from a distribution
======================================
"""

# %%
# In this example we are going to retrieve the copula from a multidimensional distribution.
#

# %%
import openturns as ot


# %%
# Create a multivariate Normal distribution
mu = [0.0] * 2
sigma = [1.0] * 2
R = ot.CorrelationMatrix(2)
R[0, 1] = 0.25
distribution = ot.Normal(mu, sigma, R)
print(distribution)

# %%
# Extract the copula
copula = distribution.getCopula()
print(copula)
