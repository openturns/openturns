%feature("docstring") OT::HermiteFactory
R"RAW(Hermite specific orthonormal univariate polynomial family.

For the :class:`~openturns.Normal` distribution :math:`\cN(0,1)`.

Any sequence of orthogonal polynomials follows the three-term recurrence
formula detailed in :ref:`orthonormal_polynomials`.

The recurrence coefficients for the Hermite polynomials come analytically and
read for :math:`i \geq 0`:

.. math::

        a_i & =  \displaystyle \frac{1}{\sqrt{i + 1}} \\
        b_i & =  0 \\
        c_i & =  \displaystyle - \sqrt{\frac{i}{i + 1}}

The nodes and weights of the associated Gauss-Hermite quadrature rule are
computed by the fast Hermite rule mapped to the measure: polished
eigensolver below 256 nodes (see [golub1969]_), Townsend-Trogdon-Olver
Airy expansion above (see [townsend2016]_).
At or above 256 nodes the rule reaches a relative accuracy better than
``5e-13`` and is faster than the generic solver.

See also
--------
UniVariateDistributionPolynomialFactory

Notes
-----
The following
:class:`~openturns.ResourceMap` key is used:

- ``FastHermite-AsymptoticThreshold`` (``UnsignedInteger``, default:
  ``256``): number of nodes from which the asymptotic expansion is used.
  Set it to a large number to force the generic polished eigensolver:
  improved accuracy beyond ``5e-13`` at the price of a much larger CPU
  effort.

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

>>> polynomial_factory = ot.HermiteFactory(1.0, 2.0)
>>> print(polynomial_factory)
class=HermiteFactory measure=class=Normal name=Normal dimension=1 mean=class=Point name=Unnamed dimension=1 values=[1] sigma=class=Point name=Unnamed dimension=1 values=[2] correlationMatrix=class=CorrelationMatrix dimension=1 implementation=class=MatrixImplementation name=Unnamed rows=1 columns=1 values=[1])RAW"
