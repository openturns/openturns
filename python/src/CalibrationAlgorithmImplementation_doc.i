%define OT_CalibrationAlgorithm_doc
"Base class for calibration algorithms.

Parameters
----------
outputObservations : 2-d sequence of float
    The output data to be fitted.
prior : :class:`~openturns.Distribution`
    The prior distribution of the parameter

See also
--------
CalibrationResult"
%enddef
%feature("docstring") OT::CalibrationAlgorithmImplementation
OT_CalibrationAlgorithm_doc

// ---------------------------------------------------------------------

%define OT_CalibrationAlgorithm_getModel_doc
"Accessor to the model to be fitted.

Returns
-------
data : :class:`~openturns.Function`
    The model to be fitted."

%enddef
%feature("docstring") OT::CalibrationAlgorithmImplementation::getModel
OT_CalibrationAlgorithm_getModel_doc

// ---------------------------------------------------------------------

%define OT_CalibrationAlgorithm_getInputObservations_doc
"Accessor to the input data to be fitted.

Returns
-------
data : :class:`~openturns.Sample`
    The input data to be fitted."

%enddef
%feature("docstring") OT::CalibrationAlgorithmImplementation::getInputObservations
OT_CalibrationAlgorithm_getInputObservations_doc

// ---------------------------------------------------------------------

%define OT_CalibrationAlgorithm_getOutputObservations_doc
"Accessor to the output data to be fitted.

Returns
-------
data : :class:`~openturns.Sample`
    The output data to be fitted."

%enddef
%feature("docstring") OT::CalibrationAlgorithmImplementation::getOutputObservations
OT_CalibrationAlgorithm_getOutputObservations_doc

// ---------------------------------------------------------------------

%define OT_CalibrationAlgorithm_getParameterPrior_doc
"Accessor to the parameter prior distribution.

Returns
-------
prior : :class:`~openturns.Distribution`
    The parameter prior distribution."
%enddef
%feature("docstring") OT::CalibrationAlgorithmImplementation::getParameterPrior
OT_CalibrationAlgorithm_getParameterPrior_doc

// ---------------------------------------------------------------------

%define OT_CalibrationAlgorithm_run_doc
"Launch the algorithm.

Notes
-----
It launches the algorithm and creates a :class:`~openturns.CalibrationResult`,
structure containing all the results."
%enddef
%feature("docstring") OT::CalibrationAlgorithmImplementation::run
OT_CalibrationAlgorithm_run_doc
