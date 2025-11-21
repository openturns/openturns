%feature("docstring") OT::Laplace
R"RAW(Laplace distribution.

Its probability density function is defined as:

.. math::

    f_X(x) = \frac{\lambda}{2}
             \exp\left(- \lambda |x - \mu|\right),
             \quad x \in \Rset

with :math:`\lambda > 0` and :math:`\mu \in \Rset`.

Its first moments are:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Expect{X} & = & \mu \\
        \Var{X} & = & \frac{2}{\lambda^2}
    \end{eqnarray*}

Parameters
----------
mu : float
    Mean parameter :math:`\mu`.

    Default value is 0.0.
lambda : float, :math:`\lambda > 0`
    Rate parameter :math:`\lambda`.

    Default value is 1.0.

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> distribution = ot.Laplace(0.0, 1.0)

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Laplace::getLambda
R"RAW(Accessor to the distribution's rate parameter :math:`\lambda`.

Returns
-------
lambda : float
    Rate parameter :math:`\lambda`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Laplace::getMu
R"RAW(Accessor to the distribution's mean parameter :math:`\mu`.

Returns
-------
mu : float
    Mean parameter :math:`\mu`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Laplace::setLambda
R"RAW(Accessor to the distribution's rate parameter :math:`\lambda`.

Parameters
----------
lambda : float, :math:`\lambda > 0`
    Scale parameter :math:`\lambda`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Laplace::setMu
R"RAW(Accessor to the distribution's mean parameter :math:`\mu`.

Parameters
----------
mu : float, :math:`\mu \in \Rset`
    Mean parameter :math:`\mu`.)RAW"
