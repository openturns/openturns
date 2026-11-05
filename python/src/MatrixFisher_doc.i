%feature("docstring") OT::MatrixFisher
R"RAW(Matrix Fisher distribution on the rotation group :math:`SO(3)`.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The Matrix Fisher distribution is defined on the group of rotation matrices
:math:`SO(3)` with probability density function:

.. math::

    f(\matr{R}) = \frac{\exp\big(\mathrm{tr}(\matr{F}^T\matr{R})\big)}
    {a_0(\matr{F})}

with respect to the Haar measure of :math:`SO(3)`, where
:math:`\matr{F}` is a :math:`3\times 3` parameter matrix and
:math:`\mathrm{tr}(\matr{F}^T\matr{R})` is the Frobenius inner product
of :math:`\matr{F}` and :math:`\matr{R}` over all nine matrix entries. The
resulting **mean** matrix is:

.. math::

    \mathbb{E}[\matr{R}] = \matr{U}\,\mathrm{diag}
    \big(r_1, r_2, r_3\big)\matr{V}^T

where :math:`\matr{F} = \matr{U}\mathrm{diag}(\sigma_1,\sigma_2,\sigma_3)
\matr{V}^T` with :math:`\sigma_1 \ge \sigma_2 \ge \sigma_3`.

The **covariance** matrix is the :math:`9\times 9` covariance of the
row-major flattened rotation:

.. math::

    \mathrm{Cov}(\mathrm{vec}(\matr{R})) =
    \mathbb{E}[\mathrm{vec}(\matr{R})\mathrm{vec}(\matr{R})^T]
    - \mathbb{E}[\mathrm{vec}(\matr{R})]
    \mathbb{E}[\mathrm{vec}(\matr{R})]^T

The normalization constant :math:`a_0(\matr{F})` is computed by
Gauss-Legendre quadrature over the Euler angles
:math:`(\phi,\theta,\psi)\in[0,2\pi]\times[0,\pi]\times[0,2\pi]`:

.. math::

    a_0(\matr{F}) = 4\pi^3 e^{m(\matr{F})} \int_{[0,1]^3}
    \exp\big(\mathrm{tr}(\matr{F}^T\matr{R}(\phi,\theta,\psi))
    - m(\matr{F})\big)
    \sin\theta\,\mathrm{d}u\,\mathrm{d}v\,\mathrm{d}w

where :math:`m(\matr{F}) = \max_{\matr{R}\in SO(3)}
\mathrm{tr}(\matr{F}^T\matr{R})` is the maximum of the trace over
:math:`SO(3)`: with :math:`\sigma_1 \ge \sigma_2 \ge \sigma_3` the singular
values of :math:`\matr{F}`, :math:`m(\matr{F}) = \sigma_1 + \sigma_2 +
\sigma_3`, except :math:`\sigma_1 + \sigma_2 - \sigma_3` when
:math:`\det\matr{F} < 0`. Subtracting :math:`m(\matr{F})` keeps the
integrand below one for numerical stability and the implementation restores
it through :math:`\log a_0(\matr{F}) = m(\matr{F}) + \log(\mathrm{integral})`.

**Sampling** uses the acceptance-rejection method with a uniform distribution
on :math:`SO(3)` as enveloping distribution. The latter is sampled from
random unit quaternions. For large concentrations the acceptance rate becomes
small, hence the sampler may become slow for high concentration parameters.

The **entropy** is:

.. math::

    \mathcal{H} = \log a_0(\matr{F})
    - \mathbb{E}[\mathrm{tr}(\matr{F}^T\matr{R})]

Parameters
----------
F : :class:`~openturns.SquareMatrix`
    The :math:`3\times 3` parameter matrix.
epsilon : float, optional
    Relative tolerance for the validation of the orthogonality of the
    statically normalized matrices. Default value is given by the
    ``MatrixFisher-OrthogonalityThreshold`` ResourceMap key.

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``MatrixFisher-OrthogonalityThreshold`` (``Scalar``, default: ``1.0e-12``): relative tolerance for the
  validation of the orthogonality of the sampled matrices.
- ``MatrixFisher-QuadratureOrder`` (``UnsignedInteger``, default: ``50``): number of quadrature nodes per dimension
  used in the evaluation of the normalization constant.
- ``MatrixFisher-QuadratureGrowthFactor`` (``Scalar``, default: ``7.0``): the effective quadrature order is
  the maximum of the base order and this factor times the square root of the
  trace bound.

Examples
--------
Create a distribution on :math:`SO(3)`:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> F = ot.SquareMatrix([[1.0, 0.0, 0.0], [0.0, 0.5, 0.0], [0.0, 0.0, 0.1]])
>>> distribution = otexp.MatrixFisher(F)

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MatrixFisher::getF
R"RAW(Accessor to the parameter matrix.

Returns
-------
F : :class:`~openturns.SquareMatrix`
    The :math:`3\times 3` parameter matrix.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MatrixFisher::setF
R"RAW(Accessor to the parameter matrix.

Parameters
----------
F : :class:`~openturns.SquareMatrix`
    The :math:`3\times 3` parameter matrix.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MatrixFisher::getEpsilon
R"RAW(Accessor to the orthogonality threshold.

Returns
-------
epsilon : float
    Relative tolerance for the validation of the orthogonality of the
    sampled matrices.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MatrixFisher::setEpsilon
R"RAW(Accessor to the orthogonality threshold.

Parameters
----------
epsilon : float
    Relative tolerance for the validation of the orthogonality of the
    sampled matrices.
)RAW"
