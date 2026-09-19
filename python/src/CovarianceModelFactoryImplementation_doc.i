%define OT_CovarianceModelFactory_doc
"Estimation of the covariance model of a process.

This class is an interface class for all the classes that build covariance models. OpenTURNS provides two covariance model factories: :class:`~openturns.StationaryCovarianceModelFactory` for stationary processes and :class:`~openturns.NonStationaryCovarianceModelFactory` for all processes. This factory does not make any hypothesis on the stationarity of the process.

Notes
-----
This class uses the following entries of :class:`~openturns.ResourceMap`:

- *CovarianceModel-DefaultNuggetFactor* (``Scalar``, default: `1e-12`)"

%enddef
%feature("docstring") OT::CovarianceModelFactoryImplementation
OT_CovarianceModelFactory_doc



%feature("docstring") OT::CovarianceModelFactoryImplementation::build
"Estimate a covariance model from data.

Available usages:
    build(*sample*)

    build(*timeSeries*)

Parameters
----------
sample : :class:`~openturns.ProcessSample`
    The sample of a stochastic process.
timeSeries : :class:`~openturns.Field`
    The time series.

Returns
-------
covModel : :class:`~openturns.CovarianceModel`
    The estimated covariance model."

