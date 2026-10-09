%feature("docstring") OT::LaguerreFactory
R"RAW(Laguerre specific orthonormal univariate polynomial family.

For the :class:`~openturns.Gamma` distribution.

Any sequence of orthogonal polynomials follows the three-term recurrence
formula detailed in :ref:`orthonormal_polynomials`.

The recurrence coefficients for the Laguerre polynomials come analytically and
read:

.. math::

    \begin{array}{rcl}
    a_i & = & \omega_i \\
    b_i & = & - (2 i + k) \omega_i \\
    c_i & = & - \sqrt{(i + k - 1) i} \omega_i
    \end{array}, \quad 1 < i

where :math:`k` is the shape parameter of the
:class:`~openturns.Gamma` distribution, and:

.. math::

    \omega_i = \frac{1}{\sqrt{(i + 1) (i + k)}} , \quad 1 < i

The nodes and weights of the associated Gauss-Laguerre quadrature rule
are computed by the fast Laguerre rule mapped to the measure:
polished eigensolver below 8 nodes (see [golub1969]_), Gil-Segura-Temme
iterative sweeps above (see [gil2019]_).
At or above 8 nodes the rule reaches a relative accuracy better than
``5e-13`` and is faster than the generic solver.

Parameters
----------
k : float
    Shape parameter :math:`k > 0` of the :class:`~openturns.Gamma` distribution.
lambda : float, optional
    Rate parameter :math:`\lambda > 0` of the :class:`~openturns.Gamma`
    distribution. Defaults to 1.0.
gamma : float, optional
    Location parameter :math:`\gamma` of the :class:`~openturns.Gamma`
    distribution. Defaults to 0.0.

See also
--------
UniVariateDistributionPolynomialFactory

Notes
-----
The following
:class:`~openturns.ResourceMap` key is used:

- ``FastLaguerre-IterativeThreshold`` (``UnsignedInteger``, default:
  ``8``): number of nodes from which the iterative sweeps are used.
  Set it to a large number to force the generic polished eigensolver:
  improved accuracy beyond ``5e-13`` at the price of a much larger CPU
  effort.

Examples
--------
>>> import openturns as ot
>>> polynomial_factory = ot.LaguerreFactory()
>>> for i in range(3):
...     print(polynomial_factory.build(i))
1
-1 + X
1 - 2 * X + 0.5 * X^2

>>> polynomial_factory = ot.LaguerreFactory(2.5, 2.0, -1.0)
>>> print(polynomial_factory)
class=LaguerreFactory k=2.5 measure=class=Gamma name=Gamma dimension=1 k=2.5 lambda=2 gamma=-1)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LaguerreFactory::getK
"Accessor to the shape parameter :math:`k`.

Of the :class:`~openturns.Gamma` distribution.

Returns
-------
k : float
    Shape parameter :math:`k` of the
    :class:`~openturns.Gamma` distribution."
