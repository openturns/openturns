%define OT_HSICEstimator_doc
"Base class of HSIC estimators.

Notes
-----
HSIC-based analyses must be performed by using the derived classes: :class:`~openturns.HSICEstimatorConditionalSensitivity`, :class:`~openturns.HSICEstimatorGlobalSensitivity` and :class:`~openturns.HSICEstimatorTargetSensitivity`.

See also
--------
:class:`~openturns.HSICEstimatorConditionalSensitivity`, :class:`~openturns.HSICEstimatorGlobalSensitivity`, :class:`~openturns.HSICEstimatorTargetSensitivity`"
%enddef
%feature("docstring") OT::HSICEstimatorImplementation
OT_HSICEstimator_doc

// --------------------------------------------------------------------------

%define OT_HSICEstimator_setPermutationSize_doc
"Set the number of permutations to be used for p-value estimate.

Parameters
----------
B : int
    The number of permutation used for p-value estimates."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::setPermutationSize
OT_HSICEstimator_setPermutationSize_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_getPermutationSize_doc
"Get the number of permutations.

Returns
-------
permutationSize : int
    The number of permutations."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::getPermutationSize
OT_HSICEstimator_getPermutationSize_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_getCovarianceModelCollection_doc
"Get the list of all covariance models used.

Returns
-------
coll : CovarianceModelCollection
    The list of all covariance models used. The last one is the output covariance model."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::getCovarianceModelCollection
OT_HSICEstimator_getCovarianceModelCollection_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_setCovarianceModelCollection_doc
"Set the covariance models.

Parameters
----------
coll : CovarianceModelCollection
    The list of all covariance models."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::setCovarianceModelCollection
OT_HSICEstimator_setCovarianceModelCollection_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_getInputSample_doc
"Get the input sample.

Returns
-------
inSample : :class:`~openturns.Sample`
    The input sample used for analysis."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::getInputSample
OT_HSICEstimator_getInputSample_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_setInputSample_doc
"Set the input sample.

Parameters
----------
inputSample : 2-d sequence of float
    The input sample to be used."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::setInputSample
OT_HSICEstimator_setInputSample_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_getOutputSample_doc
"Get the output sample.

Returns
-------
outSample : :class:`~openturns.Sample`
    The output sample used for analysis."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::getOutputSample
OT_HSICEstimator_getOutputSample_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_setOutputSample_doc
"Set the output sample.

Parameters
----------
outputSample : 2-d sequence of float
    The output sample to be used."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::setOutputSample
OT_HSICEstimator_setOutputSample_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_getDimension_doc
"Get the dimension of the input sample.

Returns
-------
dim : int
    The dimension of the input sample."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::getDimension
OT_HSICEstimator_getDimension_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_getSize_doc
"Get the size of the input sample.

Returns
-------
size : int
    The size of the input sample."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::getSize
OT_HSICEstimator_getSize_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_getEstimator_doc
"Get the estimator used for computations.

Returns
-------
estimator : :class:`~openturns.HSICStat`
    The estimator used for internal computations."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::getEstimator
OT_HSICEstimator_getEstimator_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_getHSICIndices_doc
"Get the HSIC indices.

Returns
-------
hsicIndices : :class:`~openturns.Point`
    The HSIC indices of all components."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::getHSICIndices
OT_HSICEstimator_getHSICIndices_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_getR2HSICIndices_doc
"Get the R2-HSIC indices.

Returns
-------
r2hsicIndices : :class:`~openturns.Point`
    The R2-HSIC indices of all components."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::getR2HSICIndices
OT_HSICEstimator_getR2HSICIndices_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_getPValuesPermutation_doc
"Get the p-value estimated through permutations.

Returns
-------
pval : :class:`~openturns.Point`
    The p-values of all components estimated with permutations of the data."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::getPValuesPermutation
OT_HSICEstimator_getPValuesPermutation_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_drawHSICIndices_doc
"Draw the HSIC indices.

Returns
-------
graph : :class:`~openturns.Graph`
    The graph of all HSIC indices according to components."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::drawHSICIndices
OT_HSICEstimator_drawHSICIndices_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_drawR2HSICIndices_doc
"Draw the R2-HSIC indices.

Returns
-------
graph : :class:`~openturns.Graph`
    The graph of all R2-HSIC indices according to components."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::drawR2HSICIndices
OT_HSICEstimator_drawR2HSICIndices_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_drawPValuesPermutation_doc
"Draw the p-values obtained by permutation.

Returns
-------
graph : :class:`~openturns.Graph`
    The graph of all p-values by permutation according to components."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::drawPValuesPermutation
OT_HSICEstimator_drawPValuesPermutation_doc

// ---------------------------------------------------------------------

%define OT_HSICEstimator_run_doc
"Compute all values at once."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::run
OT_HSICEstimator_run_doc
// ---------------------------------------------------------------------

%define OT_HSICEstimator_getPValuesAsymptotic_doc
"Get the p-values obtained with an asymptotic formula.

Returns
-------
pval : sequence of float
    The p-values for all components."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::getPValuesAsymptotic
OT_HSICEstimator_getPValuesAsymptotic_doc
// ---------------------------------------------------------------------
%define OT_HSICEstimator_drawPValuesAsymptotic_doc
"Draw the p-values obtained with an asymptotic formula.

Returns
-------
graph : :class:`~openturns.Graph`
    The graph of all p-values estimated with an asymptotic formula."
%enddef
%feature("docstring") OT::HSICEstimatorImplementation::drawPValuesAsymptotic
OT_HSICEstimator_drawPValuesAsymptotic_doc
// ---------------------------------------------------------------------
