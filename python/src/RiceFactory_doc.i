%feature("docstring") OT::RiceFactory
"Rice factory.

Notes
-----
The parameters are estimated by method of moments, according to [koay2006]_.

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
    

Notes
-----
This class uses the following entries of :class:`~openturns.ResourceMap`:

- *RiceFactory-AbsolutePrecision* (:raw:`Scalar`, default: `1e-12`)
- *RiceFactory-MaximumIteration* (:raw:`UnsignedInteger`, default: `10`)
- *RiceFactory-RelativePrecision* (:raw:`Scalar`, default: `1e-12`)
- *RiceFactory-ResidualPrecision* (:raw:`Scalar`, default: `1e-12`)

    In the first usage, the default Rice distribution is built."


