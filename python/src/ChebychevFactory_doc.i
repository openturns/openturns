%feature("docstring") OT::ChebychevFactory
R"RAW(Chebychev specific orthonormal univariate polynomial family.

For the :class:`~openturns.Arcsine` distribution :math:`\cA(-1,1)` which is a :class:`~openturns.Beta` distribution
:math:`B(1/2, 1/2, -1,1)`.

Notes
-----
Any sequence of orthogonal polynomials has a recurrence formula relating any
three consecutive polynomials as follows:

.. math::

    P_{i + 1} & = (a_i x + b_i) P_i + c_i P_{i - 1}, \quad  i \geq 0 \\
    P_{-1} & = 0 \\
    P_0 & = 1

The recurrence coefficients for the Chebychev polynomials come analytically and
read :math:`a_0=\sqrt{2}, b_0=0, c_0=0`, :math:`a_1=2, b_1=0,c_1=-\sqrt{2}` and for :math:`i>1`, :math:`a_i=2, b_i=0, c_i=-1`.


See also
--------
StandardDistributionPolynomialFactory

Examples
--------
>>> import openturns as ot
>>> polynomial_factory = ot.ChebychevFactory()
>>> for i in range(3):
...     print(polynomial_factory.build(i))
1
1.41421 * X
-1.41421 + 2.82843 * X^2
>>> print(polynomial_factory.getRecurrenceCoefficients(1))
[2,0,-1.41421]
)RAW"

