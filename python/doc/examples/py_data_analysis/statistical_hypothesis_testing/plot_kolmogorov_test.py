"""
Test distribution fitting using Kolmogorov
==========================================
"""
# %%
# In this example we are going to perform a Kolmogorov goodness-of-fit test for an 1-d continuous distribution.

# %%
from __future__ import print_function
import openturns as ot

# %%
# Create the data.

# %%
distribution = ot.Normal()
sample = distribution.getSample(50)

# %%
# Case 1 : the distribution parameters are known.
# -----------------------------------------------
#
# In the case where the parameters of the distribution are known, the second parameter of the `Kolmogorov` static method is the distribution to be tested.

# %%
result = ot.FittingTest.Kolmogorov(sample, distribution, 0.01)
print('Conclusion=', result.getBinaryQualityMeasure(), 'P-value=', result.getPValue())

# %%
# Test succeeded ?

# %%
result.getBinaryQualityMeasure()

# %%
# P-Value associated to the risk

# %%
result.getPValue()

# %%
# Threshold associated to the test.

# %%
result.getThreshold()

# %%
# Observed value of the statistic.

# %%
result.getStatistic()

# %%
# Case 2 : the distribution parameters are estimated from the sample.
# -------------------------------------------------------------------
#
# In the case where the parameters of the distribution are estimated from the sample, the second parameter of the `Kolmogorov` static method is the associated distribution factory to be tested.

# %%
ot.ResourceMap.SetAsUnsignedInteger("FittingTest-KolmogorovSamplingSize",1000)

# %%
distributionFactory = ot.NormalFactory()

# %%
dist, result = ot.FittingTest.Kolmogorov(sample, distributionFactory, 0.01)
print('Conclusion=', result.getBinaryQualityMeasure(), 'P-value=', result.getPValue())

# %%
dist

# %%
# Test succeeded ?

# %%
result.getBinaryQualityMeasure()

# %%
# P-Value associated to the risk

# %%
result.getPValue()

# %%
# Threshold associated to the test.

# %%
result.getThreshold()

# %%
# Observed value of the statistic.

# %%
result.getStatistic()
