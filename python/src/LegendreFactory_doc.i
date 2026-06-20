%feature("docstring") OT::LegendreFactory
R"RAW(Legendre specific orthonormal univariate polynomial family.

For the :class:`~openturns.Uniform` distribution :math:`\cU(-1,1)`.

Notes
-----
Any sequence of orthogonal polynomials has a recurrence formula relating any
three consecutive polynomials as follows:

.. math::

    P_{i + 1} & = (a_i x + b_i) P_i + c_i P_{i - 1}, \quad  i \geq 0 \\
    P_{-1} & = 0 \\
    P_0 & = 1

The recurrence coefficients for the Legendre polynomials come analytically and
read  for :math:`i \geq 0`:

.. math::

        a_i & =  \displaystyle \frac{\sqrt{(2 i + 1) (2 i + 3)}}
                                     {i + 1} \\
        b_i & =  0 \\
        c_i & =  \displaystyle - \frac{i \sqrt{2 i + 3}}
                                       {(i + 1) \sqrt{2 i - 1}}

See also
--------
StandardDistributionPolynomialFactory

Examples
--------
>>> import openturns as ot
>>> polynomial_factory = ot.LegendreFactory()
>>> for i in range(3):
...     print(polynomial_factory.build(i))
1
1.73205 * X
-1.11803 + 3.3541 * X^2
>>> print(polynomial_factory.getRecurrenceCoefficients(1))
[1.93649,0,-1.11803]
)RAW"
