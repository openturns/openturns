# ---
# jupyter:
#   jupytext:
#     text_representation:
#       extension: .py
#       format_name: percent
#       format_version: '1.3'
#       jupytext_version: 1.5.1
#   kernelspec:
#     display_name: Python 3
#     language: python
#     name: python3
# ---

# %%
"""
Test normality using Anderson-Darling
=====================================
"""
# %% 

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
test_result = ot.NormalityTest.AndersonDarlingNormal(sample)
print('Component is normal?', test_result.getBinaryQualityMeasure(),
      'p-value=%.6g' % test_result.getPValue(),
      'threshold=%.6g' % test_result.getThreshold())

# %%
# Test succeeded ?
test_result.getBinaryQualityMeasure()

# %%
# P-Value
test_result.getPValue()
