%feature("docstring") OT::Gumbel
R"RAW(Gumbel distribution.

Its cumulative distribution function and its probability density function are defined as:

.. math::
    :nowrap:

    \begin{eqnarray*}
        F_X(x) & = & \exp \left( -  \exp \left( -\dfrac{x-\gamma}{\beta} \right) \right), \quad x \in \Rset \\
        f_X(x) & = & \dfrac{1}{\beta} \exp \left(- \dfrac{x-\gamma}{\beta} - \exp \left(- \dfrac{x-\gamma}{\beta} \right) \right), \quad x \in \Rset
    \end{eqnarray*}

with :math:`\beta > 0` and :math:`\gamma \in \Rset`.

Its first moments are:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Expect{X} & = & \gamma + \gamma_e \beta \\
        \Var{X} & = & \frac{1}{6} \pi^2 \beta^2
    \end{eqnarray*}

where :math:`\gamma_e` is the Euler-Mascheroni constant.

Parameters
----------
beta : float
    scale parameter :math:`\beta > 0`.
    
    Default value is 1.0.
gamma : float
    position parameter :math:`\gamma`.
    
    Default value is 0.0.

Notes
-----
It is possible to create a Gumbel distribution from the alternative parametrizations :math:`(\mu, \sigma)`:
see :class:`~openturns.GumbelMuSigma` or :math:`(\lambda,\gamma)`: see  :class:`~openturns.GumbelLambdaGamma`.
In that case, all the results are presented in that new parametrization.

In order to use the alternative parametrization only to create the distribution, see the example below:
all the results will be presented in the native parametrization :math:`(\beta, \gamma)`.

Examples
--------
Create a distribution in its native parameters :math:`(\beta, \gamma)`:

>>> import openturns as ot
>>> myDist = ot.Gumbel(1.0, 0.0)

Create it from the alternative parametrization :math:`(\mu, \sigma)`:

>>> myDist2 = ot.Gumbel()
>>> myDist2.setParameter(ot.GumbelMuSigma()([0.58, 1.28]))

Create it from the alternative parametrization :math:`(\lambda, \gamma)`:

>>> myDist3 = ot.Gumbel()
>>> myDist3.setParameter(ot.GumbelLambdaGamma()([1.0, 1.0]))

Create it from :math:`(\mu, \sigma)` and keep that parametrization for the remaining study: 

>>> myParam = ot.GumbelMuSigma(0.58, 1.28)
>>> myDist4 = ot.ParametrizedDistribution(myParam)

Create it from :math:`(\lambda, \gamma)` and keep that parametrization for the remaining study: 

>>> myParam = ot.GumbelLambdaGamma(1.0, 1.0)
>>> myDist5 = ot.ParametrizedDistribution(myParam)

Draw a sample:

>>> sample = myDist.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Gumbel::getGamma
R"RAW(Accessor to the distribution's position parameter :math:`\gamma`.

Returns
-------
gamma : float
    Scale parameter :math:`\gamma`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Gumbel::getBeta
R"RAW(Accessor to the distribution's scale parameter :math:`\beta`.

Returns
-------
beta : float
    Scale parameter :math:`\beta`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Gumbel::setGamma
R"RAW(Accessor to the distribution's position parameter :math:`\gamma`.

Parameters
----------
gamma : float, :math:`\beta \in \Rset`
    Scale parameter :math:`\gamma`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Gumbel::setBeta
R"RAW(Accessor to the distribution's scale parameter :math:`\beta`.

Parameters
----------
beta : float, :math:`\beta > 0`
    Scale parameter :math:`\beta`.)RAW"

