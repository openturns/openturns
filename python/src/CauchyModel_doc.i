%feature("docstring") OT::CauchyModel
R"RAW(Cauchy spectral model.

Refer to :any:`parametric_spectral_model`.

Available constructors:
    CauchyModel(*theta, sigma*)

    CauchyModel(*theta, sigma, spatialCorrelation*)

    CauchyModel(*theta, spatialCovariance*)

Parameters
----------
theta : sequence of float
    Scale :math:`\theta`
    Vector of size 1
sigma : sequence of float
    Amplitude vector :math:`\vect{\sigma}`
    Vector of size d
spatialCorrelation : :class:`~openturns.CorrelationMatrix`
    Spatial correlation matrix :math:`\mat{R}` of size :math:`d \times d`.
spatialCovariance : :class:`~openturns.CovarianceMatrix`
    Spatial covariance matrix :math:`\mat{C}^{spatial} = \diag(\vect{\sigma}) \mat{R}\diag(\vect{\sigma})`.

Notes
-----
The spectral density function of input dimension 1 and output dimension **d** writes:

.. math::

    \forall f \geq 0, \forall (i,j) \in [0,d-1]^2, S_{i,j}(f) =  2 \mat{C}^{spatial}_{i,j} \frac{\theta}{1 + (2\pi \theta f)^2}

It is the spectral density associated to the :class:`~openturns.AbsoluteExponential` covariance model in dimension 1.


Examples
--------
>>> import openturns as ot
>>> spectralModel = ot.CauchyModel([3.0], [2.0])
>>> f = 0.3
>>> print(spectralModel(f))
[[ (0.727769,0) ]]
>>> f = 10
>>> print(spectralModel(f))
[[ (0.000675456,0) ]])RAW"

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

  \forall f \in \Rset, \rho(f) = \frac{2\theta}{1 + (2\pi \theta f)^2}
)RAW"
%enddef
%feature("docstring") OT::CauchyModel::computeStandardRepresentative
OT_CauchyModel_computeStandardRepresentative_doc

// ---------------------------------------------------------------------
