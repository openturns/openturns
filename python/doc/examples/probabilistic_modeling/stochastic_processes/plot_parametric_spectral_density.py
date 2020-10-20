"""
Create a parametric spectral density function
=============================================
"""
# %%
# This example illustrates how the User can create a density spectral
# function from parametric models.
#
# The library implements the *Cauchy spectral model* as a parametric model for the spectral density function
# :math:`S`.
#
# The library defines this model thanks to the object *CauchyModel*.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# 1. Define a spectral density function from correlation matrix
amplitude = [1.0, 2.0, 3.0]
scale = [4.0, 5.0, 6.0]
spatialCorrelation = ot.CorrelationMatrix(3)
spatialCorrelation[0, 1] = 0.8
spatialCorrelation[0, 2] = 0.6
spatialCorrelation[1, 2] = 0.1
spectralModel_Corr = ot.CauchyModel(amplitude, scale, spatialCorrelation)
spectralModel_Corr

# %%
# 2. Define a spectral density function from a covariance matrix
spatialCovariance = ot.CovarianceMatrix(3)
spatialCovariance[0, 0] = 4.0
spatialCovariance[1, 1] = 5.0
spatialCovariance[2, 2] = 6.0
spatialCovariance[0, 1] = 1.2
spatialCovariance[0, 2] = 0.9
spatialCovariance[1, 2] = -0.2
spectralModel_Cov = ot.CauchyModel(scale, spatialCovariance)
spectralModel_Cov
