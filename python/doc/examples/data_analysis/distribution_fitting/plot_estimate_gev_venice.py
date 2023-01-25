"""
Estimate a GEV on the Venice sea levels data
============================================
"""
# %%
# In this example we are going to estimate the parameters of a generalized extreme value distribution
# on the Venice sea levels data from [coles2001]_:
#
# - First with the R maxima estimators (paragraph 3.5.3)

# %%
# Load the Venice dataset of 10 highest sea levels per year
import openturns as ot
import openturns.viewer as otv
from openturns.usecases import coles
data = coles.Coles().venice
print(data[:5])

# %%
# Plot the 10 largest sea levels per year
graph = ot.Graph('Venice sea level', 'year', 'Sea level (cm)', True, '')
for r in range(10):
    cloud = ot.Cloud(data[:, [0, 1 + r]])
    graph.add(cloud)
graph.setIntegerXTick(True)
view = otv.View(graph)

# %%
# First drop the year column to keep only the maxima values
sample_rmax = data[:, 1:]
print(sample_rmax[:5])

# %%
# Estimate the parameters of the GEV from the r largest observations
# for different values of r
factory = ot.GeneralizedExtremeValueFactory()
r_candidate = [1, 5, 10]
for r in r_candidate:
    estimate = factory.buildRMaxima(sample_rmax, r)
    desc = estimate.getParameterDescription()
    p = estimate.getParameter()
    pretty_p = ", ".join([f"{param}: {value:.3f}" for param, value in zip(desc, p)])
    print(f"r={r:2} {pretty_p}")

# %%
# It is possible to retrieve the value of r which maximizes the likelihood
best_r, llh = factory.buildBestRMaxima(sample_rmax, r_candidate)
print(best_r)

# %%
# We can also get the associated likelihood value
for i in range(3):
    print(f"r={r_candidate[i]} likelihood={llh[i]}")

# %%
otv.View.ShowAll()
