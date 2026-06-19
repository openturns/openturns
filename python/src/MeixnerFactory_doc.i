%feature("docstring") OT::MeixnerFactory
R"RAW(Meixner specific orthonormal univariate polynomial family.

For the :class:`~openturns.Polya` distribution :math:`\cP(r,p)`.

Parameters
----------
r : float, :math:`r > 0`
    Parameter :math:`r` of the underlying :class:`~openturns.Polya`
    distribution.
    
    By default, :math:`r = 1`.
p : float, :math:`0 < p < 1`
    Parameter :math:`p` of the underlying :class:`~openturns.Polya`
    distribution.
    
    By default, :math:`p = 0.5`.

Notes
-----
Any sequence of orthogonal polynomials has a recurrence formula relating any
three consecutive polynomials as follows:

.. math::

    P_{i + 1} = (a_i x + b_i) P_i + c_i P_{i - 1}, \quad i \geq 0\\
    P_{-1} & = 0 \\
    P_0 & = 1

The recurrence coefficients for the Meixner polynomials come analytically
and read for :math:`i \geq 0`:

.. math::

        a_i & =  \displaystyle \frac{p - 1}
                                     {\sqrt{p (i + 1) (i + r)}} \\
        b_i & =  \displaystyle \frac{p (i + r) + i}
                                     {\sqrt{p (i + 1) (i + r)}} \\
        c_i & =  \displaystyle - \frac{\sqrt{p i (i + r - 1)}}
                                       {\sqrt{p (i + 1) (i + r)}}

where :math:`r` and :math:`p` are the parameters of the underlying
:class:`~openturns.Polya` distribution.

See also
--------
StandardDistributionPolynomialFactory

Examples
--------
>>> import openturns as ot
>>> polynomial_factory = ot.MeixnerFactory()
>>> for i in range(3):
...     print(polynomial_factory.build(i))
1
0.707107 - 0.707107 * X
0.5 - 1.25 * X + 0.25 * X^2
>>> print(polynomial_factory.getRecurrenceCoefficients(1))
[-0.353553,1.41421,-0.5]
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MeixnerFactory::getP
"Accessor to the :math:`p` parameter.

Of the :class:`~openturns.Polya` distribution.

Returns
-------
p : float
    Parameter :math:`p` of the underlying :class:`~openturns.Polya`
    distribution."

// ---------------------------------------------------------------------

%feature("docstring") OT::MeixnerFactory::getR
"Accessor to the :math:`r` parameter.

Of the :class:`~openturns.Polya` distribution.

Returns
-------
r : float
    Parameter :math:`r` of the underlying :class:`~openturns.Polya`
    distribution."
