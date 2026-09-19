%feature("docstring") OT::WrappedNormal
R"RAW(Wrapped Normal distribution on the circle :math:`S^1` and the torus
:math:`T^n`.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The Wrapped Normal distribution is the distribution of
:math:`\mathbf{X} = \mathbf{Z} \mod p`, where
:math:`\mathbf{Z}\sim\mathcal{N}(\boldsymbol{\mu},\boldsymbol{\Sigma})` is a
multivariate normal random vector and :math:`p` is the common period of all
coordinates. Its probability density function is:

.. math::

    f(\mathbf{x}) = \sum_{\mathbf{k}\in\mathbb{Z}^n}
    \varphi_{\boldsymbol{\mu},\boldsymbol{\Sigma}}(\mathbf{x} +
    p\mathbf{k})

where :math:`\varphi_{\boldsymbol{\mu},\boldsymbol{\Sigma}}` is the PDF of
the normal distribution.

The **mean** (wrapped mean) is the projection of :math:`\boldsymbol{\mu}`
onto the fundamental domain :math:`[-p/2,p/2]^n`. The **covariance** matrix
is :math:`\boldsymbol{\Sigma}`. The **CDF** is implemented for dimension 1:

.. math::

    F(x) = \sum_{k\in\mathbb{Z}}
    \left[\Phi\left(\frac{x-\mu+pk}{\sigma}\right)
    - \Phi\left(\frac{-p/2-\mu+pk}{\sigma}\right)\right]

Parameters
----------
mu : sequence of float
    Mean vector, mapped onto the fundamental domain :math:`[-p/2,p/2]^n`.
sigma : :class:`openturns.CovarianceMatrix`
    Covariance matrix of the underlying normal distribution.
period : float, optional
    Common period of the wrapped coordinates, by default :math:`2\pi`.

Notes
-----
The following :class:`openturns.ResourceMap` keys are used:

- ``WrappedNormal-MaxLatticeTerms``: maximum number of lattice terms
  for exact sum in PDF computation (default: 100000).

Examples
--------
Create a distribution on the circle:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> distribution = otexp.WrappedNormal([0.0], ot.CovarianceMatrix([[0.5]]))

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::WrappedNormal::setMu
R"RAW(Accessor to the mean vector.

Parameters
----------
mu : sequence of float
    Mean vector, mapped onto the fundamental domain :math:`[-p/2,p/2]^n`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::WrappedNormal::getMu
R"RAW(Accessor to the mean vector.

Returns
-------
mu : :class:`openturns.Point`
    Mean vector.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::WrappedNormal::setSigma
R"RAW(Accessor to the covariance matrix.

Parameters
----------
sigma : :class:`openturns.CovarianceMatrix`
    Covariance matrix of the underlying normal distribution.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::WrappedNormal::getSigma
R"RAW(Accessor to the covariance matrix.

Returns
-------
sigma : :class:`openturns.CovarianceMatrix`
    Covariance matrix of the underlying normal distribution.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::WrappedNormal::setMaxLatticeTerms
R"RAW(Accessor to the maximum number of lattice terms for exact sum.

Parameters
----------
maxTerms : int
    Maximum number of lattice terms for exact PDF sum (default: 100000).)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::WrappedNormal::getMaxLatticeTerms
R"RAW(Accessor to the maximum number of lattice terms for exact sum.

Returns
-------
maxTerms : int
    Maximum number of lattice terms for exact PDF sum.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::WrappedNormal::setPeriod
R"RAW(Accessor to the period.

Parameters
----------
period : float
    Common period of the wrapped coordinates, :math:`p > 0`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::WrappedNormal::getPeriod
R"RAW(Accessor to the period.

Returns
-------
period : float
    Common period of the wrapped coordinates.)RAW"