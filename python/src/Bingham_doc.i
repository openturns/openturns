%feature("docstring") OT::Bingham
R"RAW(Bingham distribution.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The Bingham distribution is defined on the sphere :math:`\mathcal{S}^{n-1}`
with probability density function:

.. math::

    f(\mathbf{x}) = \frac{\exp(-\mathbf{x}^T\boldsymbol{\Gamma}\,\mathcal{Z}
    \,\boldsymbol{\Gamma}^T\mathbf{x})}{F(\boldsymbol{\zeta})}

where :math:`\mathcal{Z} = \mathrm{diag}(\zeta_1,\dots,\zeta_n)` is the
concentration matrix, :math:`\boldsymbol{\Gamma}` is an orthogonal matrix
whose columns are the orthonormal frame of the axes and
:math:`F(\boldsymbol{\zeta})` is the normalization constant:

.. math::

    F(\boldsymbol{\zeta}) = \frac{2\pi^{n/2}}{\Gamma(n/2)}
    \sum_{k=0}^{\infty}\frac{\Gamma(n/2)}{\Gamma(n/2+k)}
    \sum_{\|\mathbf{m}\|=k}\prod_{i=1}^{n}
    \frac{(1/2)_{m_i}\zeta_i^{m_i}}{m_i!}

where :math:`(a)_m` is the Pochhammer symbol. Because the density is invariant
under the shift :math:`\boldsymbol{\zeta}\mapsto\boldsymbol{\zeta}+c\mathbf{1}`,
the concentration parameters are normalized so that
:math:`\zeta_n = \min_i\zeta_i = 0`

The distribution is symmetric, hence the **mean** is zero. The **covariance**
matrix is:

.. math::

    \mathrm{Cov}(\mathbf{X})
    = \boldsymbol{\Gamma}\,\mathrm{diag}\big(\mathbb{E}[u_1^2],\dots,
    \mathbb{E}[u_n^2]\big)\boldsymbol{\Gamma}^T

where :math:`\mathbf{u} = \boldsymbol{\Gamma}^T\mathbf{X}` and the second
moments :math:`\mathbb{E}[u_i^2]` are computed by finite differences of
:math:`\log F`.

The **entropy** is:

.. math::

    \mathcal{H} = \log F(\boldsymbol{\zeta})
    - \sum_{i=1}^{n}\zeta_i\,\mathbb{E}[u_i^2]

Parameters
----------
zeta : sequence of float
    Concentration parameters of length :math:`n`, ordered so that
    :math:`\zeta_1 \ge \dots \ge \zeta_n = 0`.
gamma : :class:`openturns.SquareMatrix`
    Orthogonal matrix whose columns are the axes of the distribution.
epsilon : float, optional
    Relative tolerance for the validation of the orthogonality of the
    basis matrix. Default value is given by the
    ``Bingham-OrthogonalityThreshold`` ResourceMap key.

Notes
-----
The following :class:`openturns.ResourceMap` keys are used:

- ``Bingham-OrthogonalityThreshold``: relative tolerance for the validation of
  the orthogonality of the basis matrix.
- ``Bingham-MaximumIteration``: maximum number of terms in the series
  expansion of the normalization constant.

Sampling uses the acceptance-rejection algorithm with an Angular Central
Gaussian enveloping distribution [kent2013]_.

Examples
--------
Create a distribution on the sphere :math:`\mathcal{S}^2`:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> gamma = ot.SquareMatrix([[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
>>> distribution = otexp.Bingham([0.5, 0.2, 0.0], gamma)

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Bingham::getZeta
R"RAW(Accessor to the concentration parameters.

Returns
-------
zeta : :class:`openturns.Point`
    Concentration parameters :math:`(\zeta_1,\dots,\zeta_n)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Bingham::setZeta
R"RAW(Accessor to the concentration parameters.

Parameters
----------
zeta : sequence of float
    Concentration parameters :math:`(\zeta_1,\dots,\zeta_n)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Bingham::getGamma
R"RAW(Accessor to the orthogonal basis matrix.

Returns
-------
gamma : :class:`openturns.SquareMatrix`
    Orthogonal matrix whose columns are the axes of the distribution.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Bingham::setGamma
R"RAW(Accessor to the orthogonal basis matrix.

Parameters
----------
gamma : :class:`openturns.SquareMatrix`
    Orthogonal matrix whose columns are orthonormal basis vectors.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Bingham::getEpsilon
R"RAW(Accessor to the orthogonality threshold.

Returns
-------
epsilon : float
    Relative tolerance for the validation of the orthogonality of the
    basis matrix.)RAW"