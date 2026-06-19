%feature("docstring") OT::CharlierFactory
R"RAW(Charlier specific orthonormal univariate polynomial family.

For :class:`~openturns.Poisson`'s distribution.

Parameters
----------
lambda : float, :math:`\lambda > 0`
    Location parameter of the underlying :class:`~openturns.Poisson`'s distribution

    Be default, :math:`\lambda = 1`.

Notes
-----
Any sequence of orthogonal polynomials has a recurrence formula relating any
three consecutive polynomials as follows:

.. math::

    P_{i + 1} & = (a_i x + b_i) P_i + c_i P_{i - 1}, \quad i \geq 0 \\
    P_{-1} & = 0 \\
    P_0 & = 1
    

The recurrence coefficients for the Charlier polynomials come analytically and
read :

.. math::

        a_i & =  \displaystyle - \frac{1}{\sqrt{\lambda (i + 1)}} \\
        b_i & =  \displaystyle \frac{i + \lambda}{\sqrt{\lambda (i + 1)}} \\
        c_i & =  \displaystyle - \sqrt{1 - \frac{1}{i + 1}}

where :math:`\lambda` is the location parameter of the underlying
:class:`~openturns.Poisson`'s distribution.

See also
--------
StandardDistributionPolynomialFactory

Examples
--------
>>> import openturns as ot
>>> polynomial_factory = ot.CharlierFactory()
>>> for i in range(3):
...     print(polynomial_factory.build(i))
1
1 - X
0.707107 - 2.12132 * X + 0.707107 * X^2)
>>> print(polynomial_factory.getRecurrenceCoefficients(1))
[-0.707107,1.41421,-0.707107]RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CharlierFactory::getLambda
"Accessor to the :math:`\lambda` parameter.

Returns
-------
lambda : float
    Location parameter of :class:`~openturns.Poisson`'s distribution."
