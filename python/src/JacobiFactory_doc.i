%feature("docstring") OT::JacobiFactory
R"RAW(Jacobi specific orthonormal univariate polynomial family.

For the :class:`~openturns.Beta` distribution.

Any sequence of orthogonal polynomials follows the three-term recurrence
formula detailed in :ref:`orthonormal_polynomials`.

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

The nodes and weights of the associated Gauss-Jacobi quadrature rule
are computed by the fast Jacobi rule mapped to the measure: polished
eigensolver below 100 nodes (see [golub1969]_), Hale-Townsend asymptotic
expansions above (see [hale2013]_).
At or above 100 nodes the rule reaches a relative accuracy better than
``5e-13`` and is faster than the generic solver.

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

See also
--------
UniVariateDistributionPolynomialFactory

Notes
-----
The following
:class:`~openturns.ResourceMap` key is used:

- ``FastJacobi-AsymptoticThreshold`` (``UnsignedInteger``, default:
  ``100``): number of nodes from which the asymptotic expansions are
  used. Set it to a large number to force the generic polished
  eigensolver: improved accuracy beyond ``5e-13`` at the price of a much
  larger CPU effort.

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
