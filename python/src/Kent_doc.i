%feature("docstring") OT::Kent
R"RAW(Kent (Fisher-Bingham) distribution.

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

See Also
--------
VonMisesFisher

Parameters
----------
kappa : float, :math:`\kappa > 0`
    Concentration parameter.
beta : float, :math:`0 \le 2\beta < \kappa`
    Ovalness parameter.
gamma1 : sequence of float
    First basis vector (unit vector).
gamma2 : sequence of float
    Second basis vector (unit vector, orthogonal to gamma1).
gamma3 : sequence of float
    Third basis vector (unit vector, orthogonal to gamma1 and gamma2).
epsilon : float, optional
    Relative tolerance for the normalization constant series expansion.
    Default value is 1e-15.

Notes
-----
The normalization constant is computed using the series expansion:

.. math::

    C(\kappa,\beta) = 2\pi\sum_{j=0}^{\infty}
    \frac{\Gamma(j+\frac12)}{\Gamma(j+1)}
    \frac{\beta^{2j}}{(\kappa/2)^{2j+\frac12}} I_{2j+\frac12}(\kappa)

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> gamma1 = [1.0, 0.0, 0.0]
>>> gamma2 = [0.0, 1.0, 0.0]
>>> gamma3 = [0.0, 0.0, 1.0]
>>> distribution = ot.Kent(25.0, 0.5, gamma1, gamma2, gamma3)

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Kent::getKappa
"Accessor to the concentration parameter.

Returns
-------
kappa : float
    Concentration parameter :math:`\kappa`."

// ---------------------------------------------------------------------

%feature("docstring") OT::Kent::setKappa
R"RAW(Accessor to the concentration parameter.

Parameters
----------
kappa : float, :math:`\kappa > 0`
    Concentration parameter.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Kent::getBeta
"Accessor to the ovalness parameter.

Returns
-------
beta : float
    Ovalness parameter :math:`\beta`."

// ---------------------------------------------------------------------

%feature("docstring") OT::Kent::setBeta
R"RAW(Accessor to the ovalness parameter.

Parameters
----------
beta : float, :math:`0 \le 2\beta < \kappa`
    Ovalness parameter.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Kent::getGamma1
"Accessor to the first basis vector.

Returns
-------
gamma1 : :class:`openturns.Point`
    First basis vector :math:`\boldsymbol{\gamma}_1`."

// ---------------------------------------------------------------------

%feature("docstring") OT::Kent::setGamma1
"Accessor to the first basis vector.

Parameters
----------
gamma1 : sequence of float
    First basis vector (unit vector)."

// ---------------------------------------------------------------------

%feature("docstring") OT::Kent::getGamma2
"Accessor to the second basis vector.

Returns
-------
gamma2 : :class:`openturns.Point`
    Second basis vector :math:`\boldsymbol{\gamma}_2`."

// ---------------------------------------------------------------------

%feature("docstring") OT::Kent::setGamma2
"Accessor to the second basis vector.

Parameters
----------
gamma2 : sequence of float
    Second basis vector (unit vector, orthogonal to gamma1)."

// ---------------------------------------------------------------------

%feature("docstring") OT::Kent::getGamma3
"Accessor to the third basis vector.

Returns
-------
gamma3 : :class:`openturns.Point`
    Third basis vector :math:`\boldsymbol{\gamma}_3`."

// ---------------------------------------------------------------------

%feature("docstring") OT::Kent::setGamma3
"Accessor to the third basis vector.

Parameters
----------
gamma3 : sequence of float
    Third basis vector (unit vector, orthogonal to gamma1 and gamma2)."
