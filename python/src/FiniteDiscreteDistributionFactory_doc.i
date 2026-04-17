%feature("docstring") OT::FiniteDiscreteDistributionFactory
"FiniteDiscreteDistribution factory.

Notes
-----
The distribution is estimated by an Uniform distribution over the sample.

See also
--------
DistributionFactory, FiniteDiscreteDistribution"

// ---------------------------------------------------------------------

%feature("docstring") OT::FiniteDiscreteDistributionFactory::buildAsFiniteDiscreteDistribution
"Estimate the distribution as native distribution.

**Available usages**:

    buildAsFiniteDiscreteDistribution()

    buildAsFiniteDiscreteDistribution(*sample*)

    buildAsFiniteDiscreteDistribution(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data.
param : sequence of float
    The parameters of the :class:`~openturns.FiniteDiscreteDistribution`.

Returns
-------
dist : :class:`~openturns.FiniteDiscreteDistribution`
    The estimated distribution as a FiniteDiscreteDistribution.
    
    In the first usage, the default :class:`openturns.FiniteDiscreteDistribution` distribution is built."
