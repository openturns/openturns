"""
Test a discrete distribution
============================
"""

# %%
# In this example we are going to perform a Chi2 goodness-of-fit test for an 1-d discrete distribution.

# %%
import openturns as ot

ot.Log.Show(ot.Log.NONE)

# %%
# Create data
distribution = ot.Poisson()
sample = distribution.getSample(30)

# %%
# Fit a Poisson distribution
distribution = ot.PoissonFactory().build(sample)

# %%
# Test the fitted distribution
result = ot.FittingTest.ChiSquared(sample, distribution, 0.01)
print("Conclusion=", result.getBinaryQualityMeasure(), "P-value=", result.getPValue())
