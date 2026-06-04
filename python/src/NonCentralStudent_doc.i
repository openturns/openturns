%feature("docstring") OT::NonCentralStudent
R"RAW(NonCentralStudent distribution.

Its probability density function is defined as:

.. math::

    f_X(x) = \frac{exp \left(-\delta^2 / 2 \right)}
                  {\sqrt{\nu\pi} \Gamma \left(\frac{\nu}{2} \right)}
             \left(\frac{\nu}{\nu + (x - \gamma)^2}\right) ^ {\frac{\nu + 1}{2}}
             \sum_{j=0}^{\infty}
             \frac{\Gamma \left(\frac{\nu + j + 1}{2}\right)}{\Gamma(j + 1)}
             \left(\delta(x - \gamma)
             \sqrt{\frac{2}{\nu + (x - \gamma)^2}}\right) ^ j,
             \quad x \in \Rset

With :math:`\Gamma` denotes Euler's Gamma function
:meth:`~openturns.SpecFunc.Gamma`.

Its first moments are:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Expect{X} & = & \gamma + \delta \sqrt{\frac{\nu}{2}}
                         \frac{\Gamma\left(\frac{\nu - 1}{2}\right)}
                              {\Gamma \left(\frac{\nu}{2}\right)}
                       \quad \text{ if }\nu > 1\\
        \Var{X} & = & \frac{\nu}{\nu - 2} (1 + \delta^2) -
                      \left(\frac{\nu}{2}\delta^2
                      \frac{\Gamma \left(\frac{\nu - 1}{2}\right)}
                           {\Gamma \left(\frac{\nu}{2}\right)}
                       - \gamma \right)^2
                    \quad \text{ if }\nu>2
    \end{eqnarray*}

Parameters
----------
nu : float, :math:`\nu > 0`
    Generalised number degree of freedom.
    
    Default value is 5.0.
delta : float
    Non-centrality parameter :math:`\delta`.

    Default value is 0.0.
gamma : float
    Position parameter :math:`\gamma`.

    Default value is 0.0.

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> distribution = ot.NonCentralStudent(6.0)

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::NonCentralStudent::getNu
R"RAW(Accessor to the distribution's generalised number degrees of freedom.

Returns
-------
nu : float
    Generalised number degrees of freedom :math:`\nu`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::NonCentralStudent::getDelta
R"RAW(Accessor to the distribution's non-centrality parameter.

Returns
-------
delta : float
    Non-centrality parameter :math:`\delta`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::NonCentralStudent::getGamma
R"RAW(Accessor to the distribution's position from the origin.

Returns
-------
gamma : float
    Position parameter :math:`\gamma`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::NonCentralStudent::setNu
R"RAW(Accessor to the distribution's generalised number degrees of freedom.

Parameters
----------
nu : float, :math:`\nu > 0`
    Generalised number degrees of freedom :math:`\nu`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::NonCentralStudent::setDelta
R"RAW(Accessor to the distribution's non-centrality parameter.

Parameters
----------
delta : float
    Non-centrality parameter :math:`\delta`.)RAW"
// ---------------------------------------------------------------------

%feature("docstring") OT::NonCentralStudent::setGamma
R"RAW(Accessor to the distribution's position from the origin.

Parameters
----------
gamma : float
    Position parameter :math:`\gamma`.)RAW"
