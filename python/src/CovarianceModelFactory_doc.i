// CovarianceModelFactory docstrings are defined in CovarianceModelFactoryImplementation_doc.i.in

%feature("docstring") OT::CovarianceModelFactory
OT_CovarianceModelFactory_doc


%feature("docstring") OT::CovarianceModelFactory::build
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

