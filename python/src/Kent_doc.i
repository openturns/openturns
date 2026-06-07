%feature("docstring") OT::Kent
R"RAW(Kent (Fisher-Bingham) distribution.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The Kent distribution is defined on the sphere :math:`\mathcal{S}^2` with 
probability density function:

.. math::

    f(\mathbf{x}) = \frac{\exp\big(\kappa\,\mathbf{x}^T\boldsymbol{\gamma}_1 
    + \beta\big((\mathbf{x}^T\boldsymbol{\gamma}_2)^2 - (\mathbf{x}^T\boldsymbol{\gamma}_3)^2\big)\big)}
    {C(\kappa,\beta)}

where :math:`\kappa > 0` is the concentration parameter, :math:`\beta` satisfies
:math:`0 \le 2\beta < \kappa`, and 
:math:`(\boldsymbol{\gamma}_1,\boldsymbol{\gamma}_2,\boldsymbol{\gamma}_3)` is 
an orthonormal basis of :math:`\mathbb{R}^3`.

When :math:`\beta = 0`, the Kent distribution reduces to the von Mises-Fisher 
distribution on the sphere.

The normalization constant is computed using the series expansion:

.. math::

    C(\kappa,\beta) = 2\pi\sum_{j=0}^{\infty}
    \frac{\Gamma(j+\frac12)}{\Gamma(j+1)}
    \frac{\beta^{2j}}{(\kappa/2)^{2j+\frac12}} I_{2j+\frac12}(\kappa)

Parameters
----------
kappa : float, :math:`\kappa > 0`
    Concentration parameter.
beta : float, :math:`0 \le 2\beta < \kappa`
    Ovalness parameter.
gamma : :class:`openturns.SquareMatrix`
    Orthogonal matrix whose columns are the basis vectors
    :math:`(\boldsymbol{\gamma}_1,\boldsymbol{\gamma}_2,\boldsymbol{\gamma}_3)`.
epsilon : float, optional
    Relative tolerance for the normalization constant series expansion.
    Default value is 1e-15.

Notes
-----
The following :class:`openturns.ResourceMap` keys are used:

- "Kent-OrthogonalityThreshold": relative tolerance for the validation of the
  orthogonality of the basis matrix.
- "Kent-MaximumIteration": maximum number of iterations for the series expansion
  and the Newton-Raphson solver.

Sampling uses the BACG acceptance-rejection algorithm with an Angular Central
Gaussian envelope [kent2013]_.

Examples
--------
Create a distribution from an orthogonal matrix:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> gamma = ot.SquareMatrix([[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
>>> distribution = otexp.Kent(25.0, 0.5, gamma)

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Kent::getKappa
R"RAW(Accessor to the concentration parameter.

Returns
-------
kappa : float
Concentration parameter :math:`\kappa`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Kent::setKappa
R"RAW(Accessor to the concentration parameter.

Parameters
----------
kappa : float, :math:`\kappa > 0`
    Concentration parameter.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Kent::getBeta
R"RAW(Accessor to the ovalness parameter.

Returns
-------
beta : float
    Ovalness parameter :math:`\beta`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Kent::setBeta
R"RAW(Accessor to the ovalness parameter.

Parameters
----------
beta : float, :math:`0 \le 2\beta < \kappa`
    Ovalness parameter.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Kent::getGamma
R"RAW(Accessor to the orthogonal basis matrix.

Returns
-------
gamma : :class:`openturns.SquareMatrix`
    Orthogonal matrix whose columns are the basis vectors
    :math:`(\boldsymbol{\gamma}_1,\boldsymbol{\gamma}_2,\boldsymbol{\gamma}_3)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Kent::setGamma
"Accessor to the orthogonal basis matrix.

Parameters
----------
gamma : :class:`openturns.SquareMatrix`
    Orthogonal matrix whose columns are orthonormal basis vectors."
