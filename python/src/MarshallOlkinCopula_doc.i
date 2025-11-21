%feature("docstring") OT::MarshallOlkinCopula
R"RAW(MarshallOlkin copula.

The MarshallOlkin copula is a bivariate copula defined by:

.. math::

    C(u_1, u_2) = 
    \begin{cases}
    u_1^{1-\alpha} u_2, \textrm{ if } u_1^\alpha \geq u_2^\beta \\
    u_1 u_2^{1-\beta}, \textrm{ otherwise.}
    \end{cases}

for :math:`(u_1, u_2) \in [0, 1]^2`. 

This copula is also known as the generalized Cuadras-Augé copula.

Independence corresponds to :math:`\alpha = 0` or :math:`\beta = 0`.

The minimum copula corresponds to :math:`\alpha = \beta = 1`.

More details on this copula can be found in [nelsen2006]_, 
chapter 3, section 3.1.1, page 52,
or in [dobrolowski2014]_.

Parameters
----------
alpha : float
    Parameter :math:`\alpha`, :math:`0 \leq \alpha \leq 1`.
    Default value is 0.5.

beta : float
    Parameter :math:`\beta`, :math:`0 \leq \beta \leq 1`.
    Default value is 0.5.

Examples
--------
Create a Marshall-Olkin copula with default parameters:

>>> import openturns as ot
>>> copula = ot.MarshallOlkinCopula()

Create a Marshall-Olkin copula:

>>> import openturns as ot
>>> copula = ot.MarshallOlkinCopula(0.3,0.7)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MarshallOlkinCopula::getAlpha
R"RAW(Get the parameter :math:`\alpha`.

Returns
-------
alpha : float
    Parameter :math:`\alpha` of the copula.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MarshallOlkinCopula::setAlpha
R"RAW(Set the parameter :math:`\alpha`.

Parameters
----------
alpha : float, :math:`0 \leq \alpha \leq 1`
    Parameter :math:`\alpha` of the copula.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MarshallOlkinCopula::getBeta
R"RAW(Get the parameter :math:`\beta`.

Returns
-------
beta : float
    Parameter :math:`\beta` of the copula.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MarshallOlkinCopula::setBeta
R"RAW(Set the parameter :math:`\beta`.

Parameters
----------
beta : float, :math:`0 \leq \beta \leq 1`
    Parameter :math:`\beta` of the copula.)RAW"
