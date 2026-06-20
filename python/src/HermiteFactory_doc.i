%feature("docstring") OT::HermiteFactory
R"RAW(Hermite specific orthonormal univariate polynomial family.

For the :class:`~openturns.Normal` distribution :math:`\cN(0,1)`.

Notes
-----
Any sequence of orthogonal polynomials has a recurrence formula relating any
three consecutive polynomials as follows:

.. math::

    P_{i + 1} & = (a_i x + b_i) P_i + c_i P_{i - 1}, \quad  i \geq 0 \\
    P_{-1} & = 0 \\
    P_0 & = 1

The recurrence coefficients for the Hermite polynomials come analytically and
read for :math:`i \geq 0`:

.. math::

        a_i & =  \displaystyle \frac{1}{\sqrt{i + 1}} \\
        b_i & =  0 \\
        c_i & =  \displaystyle - \sqrt{\frac{i}{i + 1}}

See also
--------
StandardDistributionPolynomialFactory

Examples
--------
>>> import openturns as ot
>>> polynomial_factory = ot.HermiteFactory()
>>> for i in range(3):
...     print(polynomial_factory.build(i))
1
X
-0.707107 + 0.707107 * X^2
>>> print(polynomial_factory.getRecurrenceCoefficients(1))
[0.707107,0,-0.707107]
)RAW"
