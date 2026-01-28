%feature("docstring") OT::CauchyModel
R"RAW(Cauchy spectral model.

Refer to :ref:`parametric_spectral_model`.

Available constructors:
    CauchyModel(*theta, sigma*)

    CauchyModel(*theta, sigma, spatialCorrelation*)

    CauchyModel(*theta, spatialCovariance*)

Parameters
----------
theta : sequence of float
    Vector :math:`\theta`
    Vector of size n
sigma : sequence of float
    Amplitude vector :math:`\vect{\sigma}`
    Vector of size d
spatialCorrelation : :class:`~openturns.CorrelationMatrix`
    Spatial correlation matrix :math:`\mat{R}` of size :math:`d \times d`.
spatialCovariance : :class:`~openturns.CovarianceMatrix`
    Spatial covariance matrix :math:`\mat{C}^{spatial} = \diag(\vect{\sigma}) \mat{R}\diag(\vect{\sigma})`.

Notes
-----
The spectral density function of input dimension **n** and output dimension **d** writes:

.. math::

    \forall f \geq 0, \forall (i,j) \in [0,d-1]^2, S_{i,j}(f) =  2 \mat{C}^{spatial}_{i,j} \prod_{k=1}^{n} \frac{\theta_k}{1 + (2\pi \theta_k f)^2}


Examples
--------
>>> import openturns as ot
>>> spectralModel = ot.CauchyModel([3.0, 2.0], [2.0])
>>> f = 0.3
>>> print(spectralModel(f))
[[ (0.191364,0) ]]
>>> f = 10
>>> print(spectralModel(f))
[[ (1.71084e-07,0) ]])RAW"

// ---------------------------------------------------------------------

%define OT_CauchyModel_computeStandardRepresentative_doc
R"RAW(Compute the standard representant of the spectral density function.

Parameters
----------
tau : float
    Frequency value.

Returns
-------
rho : Complex
     Standard representant factor of the spectral density function.

Notes
-----
Using definitions in :class:`~openturns.SpectralModel`: the standard representative function writes:

.. math::

  \forall \vect{f} \in \Rset^n, \rho(\vect{f} \odot \vect{\theta}) =  \prod_{k=1}^{n} \frac{1}{1 + (2\pi \theta_k f)^2}

where :math:`(\vect{f} \odot \vect{\theta})_k = \vect{f}_k \vect{\theta}_k`)RAW"
%enddef
%feature("docstring") OT::CauchyModel::computeStandardRepresentative
OT_CauchyModel_computeStandardRepresentative_doc

// ---------------------------------------------------------------------
