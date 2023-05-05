%define OT_HSICStat_doc
"Base class of HSICStat.

Notes
-----
This is a base class for the implementation of an HSIC statistic
but cannot be used by itself. An HSIC statistic object can be created by using the
derived classes: :class:`~openturns.HSICUStat` and :class:`~openturns.HSICVStat`.

See also
--------
:class:`~openturns.HSICUStat`, :class:`~openturns.HSICVStat`"
%enddef
%feature("docstring") OT::HSICStatImplementation
OT_HSICStat_doc

// ---------------------------------------------------------------------

%define OT_HSICStat_computeHSICIndex_doc
"Compute the HSIC index between two samples.

Parameters
----------

covarianceMatrixX : :class:`~openturns.CovarianceMatrix`
    The xi-covariance model discretized over the input marginal sample xi.

covarianceMatrixY : :class:`~openturns.CovarianceMatrix`
    The covariance model associated with the output sample, discretized
    over the last one.

weights : :class:`~openturns.Point` or :class:`~openturns.Matrix` (deprecated)
    The weight argument used for the statistic.

Returns
-------
hsicIndex : the HSIC index of the two :class:`~openturns.Sample`.

"
%enddef
%feature("docstring") OT::HSICStatImplementation::computeHSICIndex
OT_HSICStat_computeHSICIndex_doc

// ---------------------------------------------------------------------

%define OT_HSICStat_isCompatibleWithConditionalAnalysis_doc
"Indicate the compatibility with a conditional HSIC estimator.

Returns
-------
isCompatible : bool
    Indicate the compatibility with a conditional HSIC estimator."
%enddef
%feature("docstring") OT::HSICStatImplementation::isCompatibleWithConditionalAnalysis
OT_HSICStat_isCompatibleWithConditionalAnalysis_doc

// ---------------------------------------------------------------------

%define OT_HSICStat_computePValue_doc
"Compute the p-value of the statistic.

Parameters
----------
dist : :class:`~openturns.Gamma`
    A :class:`~openturns.Gamma` distribution to compute the p-value.

n : int
    The size of the samples.

HSIC_obs : float
    The previously computed HSIC index.

mHSIC : float
    Bias-correcting term (only actually used by U-statistics).

Returns
-------
pvalue : the p-value of the statistic."
%enddef
%feature("docstring") OT::HSICStatImplementation::computePValue
OT_HSICStat_computePValue_doc

// ---------------------------------------------------------------------
