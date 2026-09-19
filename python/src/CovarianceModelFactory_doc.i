// CovarianceModelFactory docstrings are defined in CovarianceModelFactoryImplementation_doc.i.in

%feature("docstring") OT::CovarianceModelFactory
OT_CovarianceModelFactory_doc


%feature("docstring") OT::CovarianceModelFactory::build
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

