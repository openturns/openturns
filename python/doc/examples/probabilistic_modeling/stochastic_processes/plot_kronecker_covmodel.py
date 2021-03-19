"""
Create a Kronecker covariance model
===================================
"""
# %%
# This example illustrates how the User can create a Kronecker covariance function, which allows defining a multivariate covariance function with a specific covariance structure.
#
# For that purpose, a spatial covariance matrix :math:`\mat{C}^{stat}` is needed in addition to a scalar correlation function :math:`\rho`.
#
#  We recall that the Kronecker covariance model is defined by
#
# .. math::
#
#    C(\vect{s}, \vect{t}) = \rho\left(\dfrac{\vect{s}}{\theta}, \dfrac{\vect{t}}{\theta}\right) C^{stat}
#
# where the output covariance matrix :math:`C^{stat}` is given by
#
# .. math::
#    C^{stat} = \mbox{Diag}(\vect{\sigma}) \, \mat{R} \,  \mbox{Diag}(\vect{\sigma}).
#
#
# :math:`\vect{\sigma}` and :math:`\vect{\theta}` are respectively the amplitude and scale vectors.
#
# The library implements this multivariate model thanks to the *KroneckerCovarianceModel* class which can be instanciated from:
#
#
# - the correlation function and amplitude vectors :math:`(\rho, \vect{\sigma})`: in that case, by default :math:`\mat{R} = \mat{I}`;
# - the correlation function, the amplitude vector and the spatial correlation matrix :math:`(\rho, \vect{\sigma}, \mat{R})`;
# - the correlation function, the amplitude vector and the spatial covariance matrix :math:`(\rho, \vect{\sigma}, \mat{C})`;
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m
ot.Log.Show(ot.Log.NONE)

# %%
# Scale vector (input dimension 1)
theta = [4.0]

# Create the rho function
rho = ot.MaternModel(theta, 1.5)

# Create the amplitude vector (output dimension 2)
sigma = [1.0, 2.0]

# output correlation
R = ot.CorrelationMatrix(2)
R[1, 0] = 0.01

# output covariance
C = ot.CovarianceMatrix(2)
C[0, 0] = 4.0
C[1, 1] = 5.0
C[1, 0] = 0.5

# %%
# Create the covariance model from the amplitude and correlation, no spatial correlation
ot.KroneckerCovarianceModel(rho, sigma)

# %%
# or from the correlation function, amplitude, and spatialCovariance
ot.KroneckerCovarianceModel(rho, sigma, R)

# %%
# or from the correlation model and spatialCovariance
ot.KroneckerCovarianceModel(rho, C)
