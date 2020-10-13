"""
Select fitted distributions
===========================
"""

# %%
# In this example help to make a choice between several distributions fitted to a sample.
#
# Several methods can be used:
#
# - the ranking by the Kolmogorov p-values (for continuous distributions),
# - the ranking by the ChiSquared p-values (for discrete distributions),
# - the ranking by BIC values.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# Create a sample from a continuous distribution
distribution = ot.Beta(2.0, 2.0, 0.0, 1.)
sample = distribution.getSample(1000)
graph = ot.UserDefined(sample).drawCDF()
view = viewer.View(graph)

# %%
# **1. Specify the model only**

# %%
# Create the list of distribution estimators
factories = [ot.BetaFactory(), ot.TriangularFactory()]

# %%
# Rank the continuous models by the Kolmogorov p-values:
estimated_distribution, test_result = ot.FittingTest.BestModelKolmogorov(sample, factories)
test_result

# %%
# Rank the continuous models wrt the BIC criteria (no test result):
ot.FittingTest.BestModelBIC(sample, factories)

# %%
# Rank the continuous models wrt the AIC criteria (no test result)
ot.FittingTest.BestModelAIC(sample, factories)

# %%
# Rank the continuous models wrt the AICc criteria (no test result):
ot.FittingTest.BestModelAICC(sample, factories)

# %%
# **2. Specify the model and its parameters**

# %%
# Create a collection of the distributions to be tested
distributions = [ot.Beta(2.0, 2.0, 0.0, 1.0), ot.Triangular(0.0, 0.5, 1.0)]

# %%
# Rank the continuous models by the Kolmogorov p-values:
estimated_distribution, test_result = ot.FittingTest.BestModelKolmogorov(sample, distributions)
test_result

# %%
# Rank the continuous models wrt the BIC criteria:
ot.FittingTest.BestModelBIC(sample, distributions)

# %%
# Rank the continuous models wrt the AIC criteria:
ot.FittingTest.BestModelAIC(sample, distributions)

# %%
# Rank the continuous models wrt the AICc criteria:
ot.FittingTest.BestModelAICC(sample, distributions)

# %%
# **Discrete distributions**

# %%
# Create a sample from a discrete distribution
distribution = ot.Poisson(2.0)
sample = distribution.getSample(1000)
graph = ot.UserDefined(sample).drawCDF()
view = viewer.View(graph)

# %%
# Create the list of distribution estimators
distributions = [ot.Poisson(2.0), ot.Geometric(0.1)]

# %%
# Rank the discrete models wrt the ChiSquared p-values:
estimated_distribution, test_result = ot.FittingTest.BestModelChiSquared(sample, distributions)
test_result

# %%
# Rank the discrete models wrt the BIC criteria:
ot.FittingTest.BestModelBIC(sample, distributions)
plt.show()
