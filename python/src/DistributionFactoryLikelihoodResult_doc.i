%feature("docstring") OT::DistributionFactoryLikelihoodResult
"Result from likelihood estimation.

Parameters
----------
distribution : :class:`~openturns.Distribution`
    Estimated distribution
parameterDistribution : :class:`~openturns.Distribution`
    Distribution of the parameters
logLikelihood : float
    Optimal log-likelihood value

See also
--------
GeneralizedExtremeValueFactory"

// ---------------------------------------------------------------------

%feature("docstring") OT::DistributionFactoryLikelihoodResult::setLogLikelihood
"Likelihood value accessor.

Parameters
----------
llh : float
    Log-likelihood value.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::DistributionFactoryLikelihoodResult::getLogLikelihood
"Likelihood value accessor.

Returns
-------
llh : float
    Log-likelihood value.
"
