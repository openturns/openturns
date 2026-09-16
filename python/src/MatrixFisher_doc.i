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
:math:`\mathbf{F}` is a :math:`3\times 3` matrix weighted by the first three
components of the columnwise vectorization of :math:`\mathbf{R}`. The
resulting **mean** matrix is:

.. math::

    \mathbb{E}[\mathbf{R}] = \mathbf{U}\,\mathrm{diag}
    \big(r_1, r_2, r_3\big)\mathbf{V}^T

where :math:`\mathbf{F} = \mathbf{U}\mathrm{diag}(\sigma_1,\sigma_2,\sigma_3)
\mathbf{V}^T` with :math:`\sigma_1 \ge \sigma_2 \ge \sigma_3`.

The **covariance** matrix is:

.. math::

    \mathrm{Cov}(\mathbf{R}) = \mathbb{E}[\mathbf{R}\mathbf{R}^T]
    - \mathbb{E}[\mathbf{R}]\mathbb{E}[\mathbf{R}]^T

The normalization constant :math:`a_0(\mathbf{F})` is computed by
Gauss-Legendre quadrature over the Euler angles
:math:`(\phi,\theta,\psi)\in[0,2\pi]\times[0,\pi]\times[0,2\pi]`:

.. math::

    a_0(\mathbf{F}) = 4\pi^3 \int_{[0,1]^3}
    \exp\big(\mathrm{tr}(\mathbf{F}^T\mathbf{R}(\phi,\theta,\psi))
    - \sigma_1-\sigma_2-\sigma_3\big)
    \sin\theta\,\mathrm{d}u\,\mathrm{d}v\,\mathrm{d}w

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
F : :class:`openturns.SquareMatrix`
    The :math:`3\times 3` parameter matrix.
epsilon : float, optional
    Relative tolerance for the validation of the orthogonality of the
    statically normalized matrices. Default value is given by the
    ``MatrixFisher-OrthogonalityThreshold`` ResourceMap key.

Notes
-----
The following :class:`openturns.ResourceMap` keys are used:

- ``MatrixFisher-OrthogonalityThreshold``: relative tolerance for the
  validation of the orthogonality of the sampled matrices.
- ``MatrixFisher-MaximumIteration``: number of quadrature nodes per dimension
  used in the evaluation of the normalization constant.
- ``MatrixFisher-ConcentrationThreshold``: threshold for splitting between
  small and large concentration approximations in the factory
  (default: 0.5).

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
F : :class:`openturns.SquareMatrix`
    The :math:`3\times 3` parameter matrix.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MatrixFisher::setF
R"RAW(Accessor to the parameter matrix.

Parameters
----------
F : :class:`openturns.SquareMatrix`
    The :math:`3\times 3` parameter matrix.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MatrixFisher::getEpsilon
R"RAW(Accessor to the orthogonality threshold.

Returns
-------
epsilon : float
    Relative tolerance for the validation of the orthogonality of the
    sampled matrices.)RAW"