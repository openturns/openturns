%feature("docstring") OT::GumbelCopula
R"RAW(Gumbel copula.

The Gumbel copula is a bivariate asymmetric Archimedean copula, exhibiting
greater dependence in the positive tail than in the negative. It is defined by:

.. math::

    C(u_1, u_2) = \exp(-((-log(u_1))^{\theta} + (-log(u_2))^{\theta}))^{1/\theta})

for :math:`(u_1, u_2) \in [0, 1]^2`

And its generator is:

.. math::

    \varphi(t) = (-\log(t))^{\theta}

Parameters
----------
theta : float, optional
    Parameter :math:`\theta`, :math:`\theta \geq 1`.
    Default value is 2.0.

See also
--------
ArchimedeanCopula

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> copula = ot.GumbelCopula(2.5)

Draw a sample:

>>> sample = copula.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GumbelCopula::getTheta
R"RAW(Get the parameter :math:`\theta`.

Returns
-------
theta : float
    Parameter :math:`\theta` of the copula.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GumbelCopula::setTheta
R"RAW(Set the parameter :math:`\theta`.

Parameters
----------
theta : float, :math:`\theta \geq 1`
    Parameter :math:`\theta` of the copula.)RAW"
