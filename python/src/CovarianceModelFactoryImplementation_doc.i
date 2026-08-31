%define OT_CovarianceModelFactory_doc
"Estimation of the covariance model of a process.

Notes
-----
This class is an interface class for all the classes that build covariance models. OpenTURNS provides two covariance model factories: :class:`~openturns.StationaryCovarianceModelFactory` for stationary processes and :class:`~openturns.NonStationaryCovarianceModelFactory` for all processes. This factory does not make any hypothesis on the stationarity of the process.

See also
--------
StationaryCovarianceModelFactory, NonStationaryCovarianceModelFactory

Notes
-----
This class uses the following entries of :class:`~openturns.ResourceMap`:

- *CovarianceModel-DefaultNuggetFactor* (:raw:`Scalar`, default: `1e-12`)"







%enddef
%feature("docstring") OT::CovarianceModelFactoryImplementation
OT_CovarianceModelFactory_doc



%feature("docstring") OT::CovarianceModelFactoryImplementation::build
"Estimate a covariance model from data.

Available usages:
    build(*sample*)

    build(*sample, center*)

Parameters
----------
sample : :class:`~openturns.NumericalSample`
    The sample of a stochastic process.
center : :class:`~openturns.NumericalPoint`
    The center of the process.

Returns
-------
covModel : :class:`~openturns.CovarianceModel`
    The estimated covariance model."

