"""
Cramer-Von Mises normality test
===============================
"""
# %%
# In this example we are going to evaluate whether a sample follows a normal distribution.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# Generate a sample
sample = ot.Normal().getSample(200)

# %%
# Test normality
test_result = ot.NormalityTest.CramerVonMisesNormal(sample)
print('Component is normal?', test_result.getBinaryQualityMeasure(),
      'p-value=%.6g' % test_result.getPValue(),
      'threshold=%.6g' % test_result.getThreshold())
