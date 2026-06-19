%feature("docstring") OT::KrawtchoukFactory
R"RAW(Krawtchouk specific orthonormal univariate polynomial family.

For the :class:`~openturns.Binomial` distribution.

Parameters
----------
n : int, :math:`n > 0`
    Parameter :math:`n > 0` of the underlying :class:`~openturns.Binomial`
    distribution.
    
    By default, :math:`n = 1`.
p : float, :math:`0 < p < 1`
    Parameter :math:`0 < p < 1` of the underlying :class:`~openturns.Binomial`
    distribution.
    
    By default, :math:`p = 0.5`.

Notes
-----
Any sequence of orthogonal polynomials has a recurrence formula relating any
three consecutive polynomials as follows:

.. math::

    P_{i + 1} & = (a_i x + b_i) P_i + c_i P_{i - 1}, \quad 0 \leq i < n \\
    P_{-1} & = 0 \\
    P_0 & = 1

The recurrence coefficients for the Krawtchouk polynomials come analytically
and read for :math:`0 \leq i < n`:

.. math::

        a_i & = \displaystyle - \frac{1}
                                       {\sqrt{(i + 1) (n - i) p (1 - p)}} \\
        b_i & =  \displaystyle \frac{p (n - i) + i (1 - p)}
                                     {\sqrt{(i + 1) (n - i) p (1 - p)}} \\
        c_i & =  \displaystyle - \sqrt{\left(1 - \frac{1}{i + 1}\right)
                                        \left(1 + \frac{1}{n - i}\right)}

where :math:`n` and :math:`p` are the parameters of the underlying
:class:`~openturns.Binomial` distribution.

.. warning::

    The Krawtchouk polynomials are only defined up to a degree :math:`m` equal
    to :math:`n - 1`. Indeed, for :math:`i = n`, some factors in the
    denominators of the recurrence coefficients are equal to zero.

See also
--------
StandardDistributionPolynomialFactory

Examples
--------
>>> import openturns as ot
>>> polynomial_factory = ot.KrawtchoukFactory(3, 0.5)
>>> for i in range(3):
...     print(polynomial_factory.build(i))
1
-1.73205 + 1.1547 * X
1.73205 - 3.4641 * X + 1.1547 * X^2
>>> print(polynomial_factory.getRecurrenceCoefficients(1))
[1,-1.5,-0.866025]RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::KrawtchoukFactory::getN
"Accessor to the :math:`n` parameter.

Returns
-------
n : int
    Number of experiments parameter of the :class:`~openturns.Binomial`
    distribution."

// ---------------------------------------------------------------------

%feature("docstring") OT::KrawtchoukFactory::getP
"Accessor to the :math:`p` parameter.

Returns
-------
p : float
    Success probability parameter of the :class:`~openturns.Binomial`
    distribution."
