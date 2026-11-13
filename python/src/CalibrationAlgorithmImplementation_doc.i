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

%define OT_CalibrationAlgorithm_getBounds_doc
"Accessor to the parameter bounds.

Returns
-------
bounds : :class:`~openturns.Interval`
    Parameter bounds used to constrain the calibration.
    An empty interval means that no bounds are applied."
%enddef
%feature("docstring") OT::CalibrationAlgorithmImplementation::getBounds
OT_CalibrationAlgorithm_getBounds_doc

// ---------------------------------------------------------------------

%define OT_CalibrationAlgorithm_setBounds_doc
"Accessor to the parameter bounds.

Parameters
----------
bounds : :class:`~openturns.Interval`
    Parameter bounds used to constrain the calibration.
    An empty interval means that no bounds are applied.
    A non-empty interval must have the calibration parameter dimension.

Notes
-----
The bounds constrain the optimization of the calibration.

For :class:`~openturns.LinearLeastSquaresCalibration` and
:class:`~openturns.NonLinearLeastSquaresCalibration`, parameters lying on a
bound up to the absolute
``LinearLeastSquaresCalibration-ActiveBoundTolerance`` tolerance are
considered fixed when the posterior is computed from the linearized model:
the covariance is zero in the fixed directions, and the residual degrees of
freedom exclude the fixed parameters."
%enddef
%feature("docstring") OT::CalibrationAlgorithmImplementation::setBounds
OT_CalibrationAlgorithm_setBounds_doc

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

// ---------------------------------------------------------------------

%define OT_CalibrationAlgorithm_getResult_doc
"Get the result structure.

Returns
-------
resCalibration : :class:`~openturns.CalibrationResult`
    The structure containing all the results of the calibration problem.

Notes
-----
The structure contains all the results of the calibration problem."
%enddef
%feature("docstring") OT::CalibrationAlgorithmImplementation::getResult
OT_CalibrationAlgorithm_getResult_doc

// ---------------------------------------------------------------------

%define OT_CalibrationAlgorithm_setResult_doc
"Accessor to optimization result.

Parameters
----------
result : :class:`~openturns.CalibrationResult`
    Result class."
%enddef
%feature("docstring") OT::CalibrationAlgorithmImplementation::setResult
OT_CalibrationAlgorithm_setResult_doc

