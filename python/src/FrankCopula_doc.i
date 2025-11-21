%feature("docstring") OT::FrankCopula
R"RAW(Frank copula.

The Frank copula is a bivariate symmetric Archimedean copula defined by:

.. math::

    C(u_1, u_2) = -\frac{1}{\theta}
                  log \left( 1 +
                             \frac{(e^{-\theta u_1} - 1)(e^{-\theta u_2} - 1)}
                                  {e^{-\theta} - 1}
                      \right)

for :math:`(u_1, u_2) \in [0, 1]^2`

And its generator is:

.. math::

    \varphi(t) = -\log \left( \frac{e^{-\theta t}-1}{e^{-\theta}-1} \right)

Parameters
----------
theta : float, optional
    Parameter :math:`\theta`, :math:`\theta \in \Rset`.
    Default value is 2.0.

See also
--------
ArchimedeanCopula

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> copula = ot.FrankCopula(2.5)

Draw a sample:

>>> sample = copula.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::FrankCopula::getTheta
R"RAW(Get the parameter :math:`\theta`.

Returns
-------
theta : float
    Parameter :math:`\theta` of the copula.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::FrankCopula::setTheta
R"RAW(Set the parameter :math:`\theta`.

Parameters
----------
theta : float
    Parameter :math:`\theta` of the copula.)RAW"
