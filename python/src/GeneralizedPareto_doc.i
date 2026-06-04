%feature("docstring") OT::GeneralizedPareto
R"RAW(Generalized Pareto distribution.

Its cumulative distribution function and its probability density function are defined as:

.. math::
    :nowrap:

    \begin{eqnarray*}
        F_X(x) &  = & 1-t(x), \quad \forall x \in \cS \\
        p_X(x) & = & -t'(x), \quad \forall x \in \cS
    \end{eqnarray*}

where 

.. math::
    :nowrap:

    \begin{eqnarray*}
        t(x) &  = & \left(1+\xi \dfrac{x-u}{\sigma}\right)^{-1/\xi} \quad \text{ if } \xi \neq 0\\
        t(x) &  = & \exp(-\dfrac{x-u}{\sigma}) \quad \text{ if } \xi= 0
    \end{eqnarray*}

and 

.. math::
    :nowrap:

    \begin{eqnarray*}
        \cS &  = &  [u, +\infty) \quad \text{ if } \xi \geq 0\\
            &  = &  [u, u-\sigma/\xi] \quad \text{ if } \xi < 0
    \end{eqnarray*}

with :math:`\sigma > 0` and :math:`\xi \in \Rset`.

Its first moments are:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Expect{X} & = & u + \frac{\sigma}{1 - \xi}
                         \quad \text{ if } \xi < 1 \\
        \Var{X} & = & \frac{\sigma^2}{(1 - 2 \xi) (1 - \xi)^2}
                      \quad \text{ if } \xi < \frac{1}{2}
    \end{eqnarray*}

Parameters
----------
sigma : float, :math:`\sigma > 0`
    Scale parameter :math:`\sigma`.
    
    Default value is 1.0.
xi : float
    Shape parameter :math:`\xi`.
    
    Default value is 0.0.
u : float, default=0.0
    Location parameter :math:`u`.
    
    Default value is 0.0.

Notes
-----
When the constructor from a :class:`~openturns.Pareto`:math:`(\beta, \alpha, \gamma)` distribution is used,  then it creates :class:`~openturns.GeneralizedPareto`:math:`(\sigma, \xi, u)` where:

.. math::
    :nowrap:

    \begin{eqnarray*}
       u & = & \gamma + \beta \\
       \sigma & = & \dfrac{\beta}{\alpha} \\
       \xi & = & \dfrac{1}{\alpha}
    \end{eqnarray*}


Examples
--------
Create a distribution:

>>> import openturns as ot
>>> dist1 = ot.GeneralizedPareto(1.0, 0.0, 0.0)

Create it from a Pareto distribution:

>>> pareto = ot.Pareto(1.0, 2.0, 3.0)
>>> dist1 = ot.GeneralizedPareto(pareto)

Draw a sample:

>>> sample = dist1.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GeneralizedPareto::getSigma
R"RAW(Accessor to the distribution's scale parameter :math:`\sigma`.

Returns
-------
sigma : float
    Scale parameter :math:`\sigma`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GeneralizedPareto::getXi
R"RAW(Accessor to the distribution's shape parameter :math:`\xi`.

Returns
-------
xi : float
    Shape parameter :math:`\xi`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GeneralizedPareto::setSigma
R"RAW(Accessor to the distribution's scale parameter :math:`\sigma`.

Parameters
----------
sigma : float, :math:`\sigma > 0`
    Scale parameter :math:`\sigma`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GeneralizedPareto::setXi
R"RAW(Accessor to the distribution's shape parameter :math:`\xi`.

Parameters
----------
xi : float, :math:`\xi \in \Rset`
    Shape parameter :math:`\xi`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GeneralizedPareto::setU
R"RAW(Accessor to the distribution's location parameter :math:`u`.

Parameters
----------
u : float, :math:`u \in \Rset`
    Location parameter :math:`u`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GeneralizedPareto::getU
"Accessor to the distribution's location parameter :math:`u`.

Returns
-------
u : float
    Location parameter :math:`u`."

// ---------------------------------------------------------------------

%feature("docstring") OT::GeneralizedPareto::asPareto
R"RAW(Pareto distribution conversion.

Returns
-------
pareto : :class:`~openturns.Pareto`
    Pareto distribution.

Notes
-----
The Pareto associated to the :class:`~openturns.GeneralizedPareto`:math:`(\sigma, \xi, u)` is a  :class:`~openturns.Pareto`:math:`(\beta, \alpha, \gamma)` such that: 

.. math::
    :nowrap:

    \begin{eqnarray*}
       \beta & = & \dfrac{\sigma}{\xi} \\
       \alpha & = & \dfrac{1}{\xi} \\
       \gamma & = & u-\dfrac{\sigma}{\xi}
    \end{eqnarray*}

)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GeneralizedPareto::computeReturnLevel
"Accessor to the return level.

Parameters
----------
m : float
    Return period

Returns
-------
q : float
    Return level computed as the quantile of level :math:`1-1/m`."

// ---------------------------------------------------------------------

%feature("docstring") OT::GeneralizedPareto::drawReturnLevel
"Plot the return level.

Returns
-------
graph : :class:`~openturns.Graph`
    Return level graph.

Notes
-----
The return level plot bounds are defined by the :class:`~openturns.ResourceMap`
entries `GeneralizedPareto-MMin` and `GeneralizedPareto-MMax`"
