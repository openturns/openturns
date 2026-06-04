%feature("docstring") OT::Rayleigh
R"RAW(Rayleigh distribution.

Its probability density function is defined as:

.. math::

    f_X(x) = \frac{(x - \gamma)} {\beta^2}
             \exp \left(-\frac{(x - \gamma)^2}{2\beta^2}\right)
             \quad x \in [\gamma,+\infty[

Its first moments are:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Expect{X} & = & \beta \sqrt{\frac{\pi}{2}} + \gamma \\
        \Var{X} & = & \frac{4 - \pi}{2} \beta^2
    \end{eqnarray*}

Parameters
----------
beta : float, :math:`\beta > 0`
    Scale parameter :math:`\beta`.

    Default value is 1.0.
gamma : float
    Location parameter :math:`\gamma`.

    Default value is 0.0.

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> distribution = ot.Poisson(10.0)

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Rayleigh::getBeta
R"RAW(Accessor to the distribution's scale parameter :math:`\beta`.

Returns
-------
beta : float
    Scale parameter :math:`\beta`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Rayleigh::getGamma
R"RAW(Accessor to the distribution's location parameter :math:`\gamma`.

Returns
-------
gamma : float
    Location parameter :math:`\gamma`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Rayleigh::setBeta
R"RAW(Accessor to the distribution's scale parameter :math:`\beta`.

Parameters
----------
beta : float, :math:`\beta > 0`
    Scale parameter :math:`\beta`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Rayleigh::setGamma
R"RAW(Accessor to the distribution's location parameter :math:`\gamma`.

Parameters
----------
gamma : float
    Location parameter :math:`\gamma`.)RAW"
