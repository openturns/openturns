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
Sample manipulation
===================
"""
# %% 

# %%
# This example will describe the main statistical functionalities on data through the Sample object.

# %%
from __future__ import print_function
import openturns as ot

# %%
# Generate a sample of dimension 3
sample = ot.Normal(3).getSample(500)
sample[:5]

# %%
# Get min and max per component
sample.getMin(), sample.getMax()

# %%
# Get the range per component (xmax-xmin)
sample.computeRange()

# %%
# Get the mean per component
sample.computeMean()

# %%
# Get the standard deviation per component
sample.computeStandardDeviationPerComponent()

# %%
# Get the Variance per component
sample.computeVariance()

# %%
# Get the Skewness per component
sample.computeSkewness()

# %%
# Get the Kurtosis per component
sample.computeKurtosis()

# %%
# Get the median per component
sample.computeMedian()

# %%
# Get the empirical 0.95 quantile per component
sample.computeQuantilePerComponent(0.95)

# %%
# Get the sample covariance
sample.computeCovariance()

# %%
# Get the sample standard deviation
sample.computeStandardDeviation()

# %%
# Get the sample Pearson correlation matrix
sample.computePearsonCorrelation()

# %%
# Get  the sample Kendall correlation matrix
sample.computeKendallTau()

# %%
# Get  the sample Spearman  correlation matrix
sample.computeSpearmanCorrelation()

# %%
# Get the value of the empirical CDF at a point
point = [1.1, 2.2, 3.3]
sample.computeEmpiricalCDF(point)
