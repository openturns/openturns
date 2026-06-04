%feature("docstring") OT::Logistic
R"RAW(Logistic distribution.

Its probability density function is defined as:

.. math::

    f_X(x) = \frac{\exp\left(\left(- \frac{x - \mu}{\beta}\right)\right)}
                  {\beta \left(1 + \exp\left(- \frac{x-\mu}{\beta}
                                             \right)\right)^2},
             \quad x \in \Rset

with :math:`\mu \in \Rset` and :math:`\beta > 0`.

Its first moments are:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Expect{X} & = & \mu \\
        \Var{X} & = & \frac{1}{3} \pi^2 \beta^2
    \end{eqnarray*}

Parameters
----------
mu : float
    Mean parameter :math:`\mu`.

    Default value is 1.0.
beta : float, :math:`\beta > 0`
    Scale parameter :math:`\beta`.    

    Default value is 1.0.

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> distribution = ot.Logistic(0.0, 1.0)

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Logistic::getMu
R"RAW(Accessor to the distribution's mean parameter :math:`\mu`.

Returns
-------
mu : float
    Mean parameter :math:`\mu`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Logistic::getBeta
R"RAW(Accessor to the distribution's scale parameter :math:`\beta`.

Returns
-------
beta : float
    Scale parameter :math:`\beta`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Logistic::setMu
R"RAW(Accessor to the distribution's mean parameter :math:`\mu`.

Parameters
----------
mu : float, :math:`\mu \in \Rset`
    Mean parameter :math:`\mu`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Logistic::setBeta
R"RAW(Accessor to the distribution's scale parameter :math:`\beta`.

Parameters
----------
beta : float, :math:`\beta > 0`
    Scale parameter :math:`\beta`.)RAW"
