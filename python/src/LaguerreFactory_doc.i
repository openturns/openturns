%feature("docstring") OT::LaguerreFactory
R"RAW(Laguerre specific orthonormal univariate polynomial family.

For the :class:`~openturns.Gamma` distribution.

Parameters
----------
k : float
    Shape parameter :math:`k > 0` of the :class:`~openturns.Gamma` distribution.
lambda : float, optional
    Scale parameter :math:`\lambda > 0` of the :class:`~openturns.Gamma`
    distribution. Defaults to 1.0.
gamma : float, optional
    Location parameter :math:`\gamma` of the :class:`~openturns.Gamma`
    distribution. Defaults to 0.0.

Any sequence of orthogonal polynomials has a recurrence formula relating any
three consecutive polynomials as follows:

.. math::

    P_{i + 1} = (a_i x + b_i) P_i + c_i P_{i - 1}, \quad 1 < i

The recurrence coefficients for the Laguerre polynomials come analytically and
read:

.. math::

    \begin{array}{rcl}
    a_i & = & \omega_i \\
    b_i & = & - (2 i + k_a + 1) \omega_i \\
    c_i & = & - \sqrt{(i + k_a) i} \omega_i
    \end{array}, \quad 1 < i

where :math:`k_a` is the alternative shape parameter of the
:class:`~openturns.Gamma` distribution, and:

.. math::

    \omega_i = \frac{1}{\sqrt{(i + 1) (i + k_a + 1)}} , \quad 1 < i

See also
--------
experimental.UniVariateDistributionPolynomialFactory

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
"Accessor to the alternative shape parameter :math:`k_a`.

Of the :class:`~openturns.Gamma` distribution.

Returns
-------
k_a : float
    Alternative shape parameter :math:`k_a = k - 1` of the
    :class:`~openturns.Gamma` distribution."
