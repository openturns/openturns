%feature("docstring") OT::ClaytonCopulaFactory
R"RAW(Clayton Copula factory.

Notes
-----
We use the following estimator:

.. math::

    \displaystyle\Hat{\theta}_n=\frac{2\Hat{\tau}_n^{\strut}}{1_{\strut} - \Hat{\tau}_n}

See also
--------
DistributionFactory, ClaytonCopula)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ClaytonCopulaFactory::buildAsClaytonCopula
R"RAW(Estimate the copula as native copula.

**Available usages**:

    buildAsClaytonCopula()

    buildAsClaytonCopula(*sample*)

    buildAsClaytonCopula(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data of dimension 2.
param : sequence of float of size 1
   The parameter :math:`\theta`.

Returns
-------
copula : :class:`~openturns.ClaytonCopula`
    The estimated copula as a Clayton copula.
    
    In the first usage, the default Clayton copula is built.)RAW"
