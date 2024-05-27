"""
Create a stationary covariance model
====================================
"""
# %%
# This use case illustrates how the User can create a covariance function from parametric models.
# The library implements the *multivariate Exponential model* as a parametric model for the covariance function where the spatial covariance function :math:`\rho` writes:
#
# .. math::
#    \rho(\vect{s}, \vect{t} ) = e^{-\left\| \vect{s}- \vect{t} \right\|_2} \quad \forall (\vect{s}, \vect{t}) \in \mathcal{D}
#
# It is possible to define the exponential model from the spatial covariance matrix :math:`\mat{C}^{spat}`
# rather than the correlation matrix :math:`\mat{R}`:
#
# .. math::
#     \forall \vect{t} \in \mathcal{D},\quad \mat{C}^{spat} = \mathbb{E} \left[ X_{\vect{t}} X^t_{\vect{t}} \right]
#     = \mat{A}\,\mat{R}, \mat{A}
#
# with:
#
# .. math::
#    \mat{A} = \mbox{Diag}(a_1, \dots, a_d)
#
# We call :math:`\vect{a}` the amplitude vector and :math:`\vect{\lambda}` the scale vector.
#
# The library implements the multivariate exponential model thanks to the object *ExponentialModel* which is created from:
#
#
# - the scale and amplitude vectors :math:`(\vect{\lambda}, \vect{a})`: in that case, by default :math:`\vect{R} = \vect{I}`;
# - the scale and amplitude vectors and the spatial correlation matrix  :math:`(\vect{\lambda}, \vect{a},\vect{R})`;
# - the scale and amplitude vectors and the spatial covariance matrix  :math:`(\vect{\lambda}, \vect{a},\vect{C})`;
#   Then :math:`\vect{C}` is mapped into the associated correlation matrix :math:`\vect{R}` and the previous constructor is used.
#

# %%
import openturns as ot

ot.Log.Show(ot.Log.NONE)

# %%
# Create the amplitude vector (output dimension 3)
amplitude = [1.0, 2.0, 3.0]

# Scale vector (input dimension 1)
scale = [4.0]

# SpatialCorrelation
spatialCorrelation = ot.CorrelationMatrix(3)
spatialCorrelation[0, 1] = 0.8
spatialCorrelation[0, 2] = 0.6
spatialCorrelation[1, 2] = 0.1

# SpatialCovariance
spatialCovariance = ot.CovarianceMatrix(3)
spatialCovariance[0, 0] = 4.0
spatialCovariance[1, 1] = 5.0
spatialCovariance[2, 2] = 6.0
spatialCovariance[0, 1] = 1.2
spatialCovariance[0, 2] = 0.9
spatialCovariance[1, 2] = -0.2

# %%
# Create the covariance model from the amplitude and scale, no spatial correlation
ot.ExponentialModel(scale, amplitude)

# %%
# or from the amplitude, scale and spatial vorrelation
ot.ExponentialModel(scale, amplitude, spatialCorrelation)

# %%
# or from the scale and spatial covariance
ot.ExponentialModel(scale, spatialCovariance)
