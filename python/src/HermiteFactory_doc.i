%feature("docstring") OT::HermiteFactory
R"RAW(Hermite specific orthonormal univariate polynomial family.

For the :class:`~openturns.Normal` distribution :math:`\cN(0,1)`.

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

The nodes and weights of the associated Gauss-Hermite quadrature rule are
computed using the Golub-Welsch algorithm via the symmetric
tridiagonal Jacobi matrix.

See also
--------
UniVariateDistributionPolynomialFactory

Examples
--------
>>> import openturns as ot
>>> polynomial_factory = ot.HermiteFactory()
>>> for i in range(3):
...     print(polynomial_factory.build(i))
1
X
-0.707107 + 0.707107 * X^2

>>> polynomial_factory = ot.HermiteFactory(1.0, 2.0)
>>> print(polynomial_factory)
class=HermiteFactory measure=class=Normal name=Normal dimension=1 mean=class=Point name=Unnamed dimension=1 values=[1] sigma=class=Point name=Unnamed dimension=1 values=[2] correlationMatrix=class=CorrelationMatrix dimension=1 implementation=class=MatrixImplementation name=Unnamed rows=1 columns=1 values=[1])RAW"
