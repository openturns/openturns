%feature("docstring") OT::MatrixFisher
R"RAW(Matrix Fisher distribution on the rotation group :math:`SO(3)`.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The Matrix Fisher distribution is defined on the group of rotation matrices
:math:`SO(3)` with probability density function:

.. math::

    f(\mathbf{R}) = \frac{\exp\big(\mathrm{tr}(\mathbf{F}^T\mathbf{R})\big)}
    {a_0(\mathbf{F})}

with respect to the Haar measure of :math:`SO(3)`, where
:math:`\mathbf{F}` is a :math:`3\times 3` parameter matrix and
:math:`\mathrm{tr}(\mathbf{F}^T\mathbf{R})` is the Frobenius inner product
of :math:`\mathbf{F}` and :math:`\mathbf{R}` over all nine matrix entries. The
resulting **mean** matrix is:

.. math::

    \mathbb{E}[\mathbf{R}] = \mathbf{U}\,\mathrm{diag}
    \big(r_1, r_2, r_3\big)\mathbf{V}^T

where :math:`\mathbf{F} = \mathbf{U}\mathrm{diag}(\sigma_1,\sigma_2,\sigma_3)
\mathbf{V}^T` with :math:`\sigma_1 \ge \sigma_2 \ge \sigma_3`.

The **covariance** matrix is the :math:`9\times 9` covariance of the
row-major flattened rotation:

.. math::

    \mathrm{Cov}(\mathrm{vec}(\mathbf{R})) =
    \mathbb{E}[\mathrm{vec}(\mathbf{R})\mathrm{vec}(\mathbf{R})^T]
    - \mathbb{E}[\mathrm{vec}(\mathbf{R})]
    \mathbb{E}[\mathrm{vec}(\mathbf{R})]^T

The normalization constant :math:`a_0(\mathbf{F})` is computed by
Gauss-Legendre quadrature over the Euler angles
:math:`(\phi,\theta,\psi)\in[0,2\pi]\times[0,\pi]\times[0,2\pi]`:

.. math::

    a_0(\mathbf{F}) = 4\pi^3 e^{m(\mathbf{F})} \int_{[0,1]^3}
    \exp\big(\mathrm{tr}(\mathbf{F}^T\mathbf{R}(\phi,\theta,\psi))
    - m(\mathbf{F})\big)
    \sin\theta\,\mathrm{d}u\,\mathrm{d}v\,\mathrm{d}w

where :math:`m(\mathbf{F}) = \max_{\mathbf{R}\in SO(3)}
\mathrm{tr}(\mathbf{F}^T\mathbf{R})` is the maximum of the trace over
:math:`SO(3)`: with :math:`\sigma_1 \ge \sigma_2 \ge \sigma_3` the singular
values of :math:`\mathbf{F}`, :math:`m(\mathbf{F}) = \sigma_1 + \sigma_2 +
\sigma_3`, except :math:`\sigma_1 + \sigma_2 - \sigma_3` when
:math:`\det\mathbf{F} < 0`. Subtracting :math:`m(\mathbf{F})` keeps the
integrand below one for numerical stability and the implementation restores
it through :math:`\log a_0(\mathbf{F}) = m(\mathbf{F}) + \log(\mathrm{integral})`.

**Sampling** uses the acceptance-rejection method with a uniform distribution
on :math:`SO(3)` as enveloping distribution. The latter is sampled from
random unit quaternions. For large concentrations the acceptance rate becomes
small, hence the sampler may become slow for high concentration parameters.

The **entropy** is:

.. math::

    \mathcal{H} = \log a_0(\mathbf{F})
    - \mathbb{E}[\mathrm{tr}(\mathbf{F}^T\mathbf{R})]

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
    The :math:`3\times 3` parameter matrix.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MatrixFisher::setF
R"RAW(Accessor to the parameter matrix.

Parameters
----------
F : :class:`~openturns.SquareMatrix`
    The :math:`3\times 3` parameter matrix.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MatrixFisher::getEpsilon
R"RAW(Accessor to the orthogonality threshold.

Returns
-------
epsilon : float
    Relative tolerance for the validation of the orthogonality of the
    sampled matrices.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MatrixFisher::setEpsilon
R"RAW(Accessor to the orthogonality threshold.

Parameters
----------
epsilon : float
    Relative tolerance for the validation of the orthogonality of the
    sampled matrices.)RAW"