%feature("docstring") OT::InverseNormal
R"RAW(Inverse normal distribution.

Parameters
----------
mu : float, :math:`\mu > 0`
    Mean parameter :math:`\mu`.
lambda : float, :math:`\lambda > 0`
    Rate parameter :math:`\lambda`.

Notes
-----
Its probability density function is defined as:

.. math::

    f_X(x) = \sqrt{\frac{\lambda}{2 \pi x^3}}
             \exp\left(\frac{- \lambda (x - \mu)^2}{2 \mu^2 x}\right),
             \quad x \in [0; +\infty[

with :math:`\lambda, \mu > 0`.

Its first moments are:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Expect{X} & = & \mu \\
        \Var{X} & = & \frac{\mu^3}{\lambda}
    \end{eqnarray*}

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> distribution = ot.InverseNormal(1.0, 1.0)

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::InverseNormal::getLambda
R"RAW(Accessor to the distribution's rate parameter.

Returns
-------
lambda : float
    Rate parameter :math:`\lambda`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::InverseNormal::getMu
R"RAW(Accessor to the distribution's mean parameter.

Returns
-------
mu : float
    Mean parameter :math:`\mu`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::InverseNormal::setMuLambda
R"RAW(Accessor to the distribution's parameters.

Parameters
----------
mu : float, :math:`\mu > 0`
    Mean parameter :math:`\mu`.
lambda : float, :math:`\lambda > 0`
    Rate parameter :math:`\lambda`.)RAW"

