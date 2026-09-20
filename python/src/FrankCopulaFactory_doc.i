%feature("docstring") OT::FrankCopulaFactory
R"RAW(Frank Copula factory.

Notes
-----
The parameters are estimated using the following equations:

:math:`\Hat{\theta}_n` is solution of

.. math::

    \displaystyle \Hat{\tau}_n = 1-4\left( \frac{1-D(\Hat{\theta}_n, 1)^{\strut}}{\theta} \right)

where :math:`D` is the Debye function defined as

.. math::

    \displaystyle D(x, n)=\frac{n}{x^n}\int_0^x \frac{t^n}{e^t-1_{\strut}} dt

The following :class:`~openturns.ResourceMap` keys are used. All the keys drive the precision
of the optimization solver involved in the estimation:

- ``FrankCopulaFactory-AbsolutePrecision`` (``Scalar``, default: ``1e-14``): absolute precision of the solver.
- ``FrankCopulaFactory-MaximumIteration`` (``UnsignedInteger``, default: ``100``): maximum number of iterations of the solver.
- ``FrankCopulaFactory-RelativePrecision`` (``Scalar``, default: ``1e-14``): relative precision of the solver.
- ``FrankCopulaFactory-ResidualPrecision`` (``Scalar``, default: ``1e-14``): residual precision of the solver.

See also
--------
DistributionFactory, FrankCopula
)RAW"


// ---------------------------------------------------------------------

%feature("docstring") OT::FrankCopulaFactory::buildAsFrankCopula
R"RAW(Estimate the copula as native copula.

**Available usages**:

    buildAsFrankCopula()

    buildAsFrankCopula(*sample*)

    buildAsFrankCopula(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data of dimension 2.
param : sequence of float of size 1
   The parameter :math:`\theta`.

Returns
-------
copula : :class:`~openturns.FrankCopula`
    The estimated copula as a Frank copula.
    
    In the first usage, the default Frank copula is built.)RAW"
