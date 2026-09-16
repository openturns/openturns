%feature("docstring") OT::RiemannianGaussian
R"RAW(Riemannian Gaussian distribution on the manifold of symmetric positive
definite matrices.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The Riemannian Gaussian distribution is defined on the set :math:`\mathcal{P}_n`
of :math:`n\times n` symmetric positive definite matrices endowed with the
affine-invariant metric. Its probability density function is:

.. math::

    f(\mathbf{X}) = \frac{1}{C} \exp\left(-\frac{1}{2}
    \left\|\mathrm{log}_{\overline{\mathbf{X}}}(\mathbf{X})\right\|^2_\sigma
    \right)

where :math:`\mathrm{log}_{\overline{\mathbf{X}}}(\mathbf{X})` is the
logarithm map at the Frechet mean :math:`\overline{\mathbf{X}}`,
:math:`\|\cdot\|_\sigma` is the Mahalanobis norm of a tangent vector
:math:`\mathbf{v}` along the covariance :math:`\boldsymbol{\Sigma}`:

.. math::

    \|\mathbf{v}\|^2_\sigma = \mathrm{vec}(\mathbf{v})^T
    \boldsymbol{\Sigma}^{-1}\mathrm{vec}(\mathbf{v})

defined on the tangent space at the mean. In these coordinates the
distribution is a standard Gaussian, hence the normalization constant is:

.. math::

    \log C = \frac{d}{2}\log(2\pi) + \frac{1}{2}\log|\boldsymbol{\Sigma}|

with :math:`d = n(n+1)/2`.

The **mean** matrix is the Frechet mean
:math:`\overline{\mathbf{X}}` and the **covariance** matrix is
:math:`\boldsymbol{\Sigma}`, both expressed in the coordinate system of
the :math:`d`-dimensional tangent space.

Parameters
----------
mean : :class:`openturns.SymmetricMatrix`
    Frechet mean of the distribution, a symmetric positive definite
    :math:`n\times n` matrix.
sigma : :class:`openturns.SquareMatrix`
    Covariance matrix :math:`\boldsymbol{\Sigma}` in the tangent space at
    the mean, of dimension :math:`d = n(n+1)/2`.
epsilon : float, optional
    Relative tolerance for the validation of the positive definiteness of
    the mean and covariance matrices. Default value is given by the
    ``RiemannianGaussian-PositiveDefiniteThreshold`` ResourceMap key.

Notes
-----
The following :class:`openturns.ResourceMap` keys are used:

- ``RiemannianGaussian-PositiveDefiniteThreshold``: relative tolerance for
  the validation of the positive definiteness of the mean and covariance
  matrices.
- ``RiemannianGaussian-LogJacobianTaylorThreshold``: threshold on the
  exponential coordinates norm below which a Taylor expansion is used to
  compute the log-Jacobian of the exponential map.

Examples
--------
Create a distribution on the manifold of :math:`2\times 2` symmetric
positive definite matrices:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> mean = ot.SymmetricMatrix([[1.0, 0.0], [0.0, 1.0]])
>>> sigma = ot.SquareMatrix([[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
>>> distribution = otexp.RiemannianGaussian(mean, sigma)

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::RiemannianGaussian::setMean
R"RAW(Accessor to the Frechet mean matrix.

Parameters
----------
mean : :class:`openturns.SymmetricMatrix`
    Symmetric positive definite mean matrix.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::RiemannianGaussian::getMeanMatrix
R"RAW(Accessor to the Frechet mean matrix.

Returns
-------
mean : :class:`openturns.SymmetricMatrix`
    Symmetric positive definite mean matrix.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::RiemannianGaussian::getMean
R"RAW(Accessor to the mean.

Returns
-------
mean : :class:`openturns.Point`
    Mean of the distribution, the upper-triangle flattened Frechet mean
    matrix.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::RiemannianGaussian::setSigma
R"RAW(Accessor to the covariance matrix in the tangent space.

Parameters
----------
sigma : :class:`openturns.SquareMatrix`
    Covariance matrix.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::RiemannianGaussian::getSigma
R"RAW(Accessor to the covariance matrix in the tangent space.

Returns
-------
sigma : :class:`openturns.SquareMatrix`
    Covariance matrix.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::RiemannianGaussian::getEpsilon
R"RAW(Accessor to the positive-definiteness threshold.

Returns
-------
epsilon : float
    Relative tolerance for the validation of the positive definiteness of
    the mean and covariance matrices.)RAW"