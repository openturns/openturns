%feature("docstring") OT::RiceFactory
"Rice factory.

Notes
-----
The parameters are estimated by method of moments, according to [koay2006]_.

The following :class:`~openturns.ResourceMap` keys are used. All the keys drive the precision
of the optimization solver involved in the estimation:

- ``RiceFactory-AbsolutePrecision`` (``Scalar``, default: ``1e-12``): absolute precision of the solver.
- ``RiceFactory-MaximumIteration`` (``UnsignedInteger``, default: ``10``): maximum number of iterations of the solver.
- ``RiceFactory-RelativePrecision`` (``Scalar``, default: ``1e-12``): relative precision of the solver.
- ``RiceFactory-ResidualPrecision`` (``Scalar``, default: ``1e-12``): residual precision of the solver.

See also
--------
DistributionFactory, Rice"

// ---------------------------------------------------------------------

%feature("docstring") OT::RiceFactory::buildAsRice
"Estimate the distribution as native distribution.

**Available usages**:

    buildAsRice()

    buildAsRice(*sample*)

    buildAsRice(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data.
param : sequence of float
    The parameters of the :class:`~openturns.Rice`.

Returns
-------
dist : :class:`~openturns.Rice`
    The estimated distribution as a Rice.
    
    In the first usage, the default Rice distribution is built.
"


