%feature("docstring") OT::JacobiFactory
R"RAW(Jacobi specific orthonormal univariate polynomial family.

For the :class:`~openturns.Beta` distribution.

Parameters
----------
alpha : float
    Shape parameter :math:`\alpha > 0` of the :class:`~openturns.Beta` distribution.
beta : float
    Shape parameter :math:`\beta > 0` of the :class:`~openturns.Beta` distribution.
a : float, optional
    Lower bound :math:`a` of the :class:`~openturns.Beta` distribution.
    Defaults to -1.0.
b : float, optional
    Upper bound :math:`b` of the :class:`~openturns.Beta` distribution.
    Defaults to 1.0.

Any sequence of orthogonal polynomials has a recurrence formula relating any
three consecutive polynomials as follows:

.. math::

    P_{i + 1} = (a_i x + b_i) P_i + c_i P_{i - 1}, \quad 1 < i

The recurrence coefficients for the Jacobi polynomials come analytically and
read:

.. math::

    \begin{array}{rcl}
    a_i & = & \displaystyle K_{2,i} (2 i + \alpha + \beta) \\
    b_i & = & \displaystyle K_{2,i} \frac{(\beta - \alpha)(\alpha + \beta - 2)}{2 i + \alpha + \beta - 2} \\
    c_i & = & \displaystyle - \frac{2 i + \alpha + \beta}{2 i + \alpha + \beta - 2} \left[(i + \alpha - 1) (i + \beta - 1) (i + \alpha + \beta - 2) i \frac{K_{1,i}}{2 i + \alpha + \beta - 3}\right]^{1/2}
    \end{array}, \quad 1 < i

where :math:`\alpha` and :math:`\beta` are the shape parameters
of the :class:`~openturns.Beta` distribution, and:

.. math::

    \begin{array}{rcl}
    K_{1,i} & = & \displaystyle \frac{2 i + \alpha + \beta + 1}{(i + 1) (i + \alpha) (i + \beta) (i + \alpha + \beta - 1)} \\
    K_{2,i} & = & \displaystyle \frac{1}{2} \sqrt{(2 i + \alpha + \beta - 1) K_{1,i}}
    \end{array}, \quad i > 1

See also
--------
UniVariateDistributionPolynomialFactory

Examples
--------
>>> import openturns as ot
>>> polynomial_factory = ot.JacobiFactory()
>>> for i in range(3):
...     print(polynomial_factory.build(i))
1
2.23607 * X
-0.935414 + 4.67707 * X^2

>>> polynomial_factory = ot.JacobiFactory(2.5, 3.5, -1.0, 2.0)
>>> print(polynomial_factory)
class=JacobiFactory alpha=2.5 beta=3.5 measure=class=Beta name=Beta dimension=1 alpha=2.5 beta=3.5 a=-1 b=2)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::JacobiFactory::getAlpha
R"RAW(Accessor to the shape parameter :math:`\alpha`.

Of the :class:`~openturns.Beta` distribution.

Returns
-------
alpha : float
    Shape parameter :math:`\alpha` of the
    :class:`~openturns.Beta` distribution.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::JacobiFactory::getBeta
R"RAW(Accessor to the shape parameter :math:`\beta`.

Of the :class:`~openturns.Beta` distribution.

Returns
-------
beta : float
    Shape parameter :math:`\beta` of the
    :class:`~openturns.Beta` distribution.)RAW"
