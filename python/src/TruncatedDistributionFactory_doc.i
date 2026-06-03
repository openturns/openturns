%feature("docstring") OT::TruncatedDistributionFactory
"Truncated distribution factory.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Automatically explores possible truncations configurations by maximum likelihood
and selects the best option with Bayesian Information Criterion (BIC).

Parameters
----------
distribution : :class:`~openturns.Distribution`
    The base distribution to be truncated.

See also
--------
openturns.DistributionFactory, openturns.TruncatedDistribution

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> ot.RandomGenerator.SetSeed(0)
>>> distribution = ot.Normal()
>>> sample = ot.TruncatedDistribution(distribution, ot.Interval(-1.0, 1.0)).getSample(50)
>>> factory = otexp.TruncatedDistributionFactory(distribution)
>>> estimated = factory.build(sample)
"

// ---------------------------------------------------------------------

%feature("docstring") OT::TruncatedDistributionFactory::buildAsTruncatedDistribution
"Build the truncated distribution as a :class:`~openturns.TruncatedDistribution`.

Parameters
----------
sample : 2-d sequence of float
    Data.

Returns
-------
dist : :class:`~openturns.TruncatedDistribution`
    The estimated distribution."

// ---------------------------------------------------------------------

%feature("docstring") OT::TruncatedDistributionFactory::build
"Build the truncated distribution.

Parameters
----------
sample : 2-d sequence of float
    Data.

Returns
-------
dist : :class:`~openturns.Distribution`
    The estimated distribution."

// ---------------------------------------------------------------------

%feature("docstring") OT::TruncatedDistributionFactory::setEstimateParameters
"Accessor to the parameters estimation flag.

Parameters
----------
estimateParameters : bool
    Whether to estimate the base distribution parameters along with the
    truncation bounds. Default is True."

// ---------------------------------------------------------------------

%feature("docstring") OT::TruncatedDistributionFactory::getEstimateParameters
"Accessor to the parameters estimation flag.

Returns
-------
estimateParameters : bool
    Whether the base distribution parameters are estimated along with the
    truncation bounds."

// ---------------------------------------------------------------------

%feature("docstring") OT::TruncatedDistributionFactory::setDistribution
"Accessor to the base distribution.

Parameters
----------
distribution : :class:`~openturns.Distribution`
    The base distribution to be truncated."

// ---------------------------------------------------------------------

%feature("docstring") OT::TruncatedDistributionFactory::getDistribution
"Accessor to the base distribution.

Returns
-------
distribution : :class:`~openturns.Distribution`
    The base distribution to be truncated."
