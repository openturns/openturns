%feature("docstring") OT::CalibrationResult
"Base class for calibration result.

Parameters
----------
parameterPrior : :class:`~openturns.Distribution`
    The prior distribution of the parameter.
parameterPosterior : :class:`~openturns.Distribution`
    The posterior distribution of the parameter.
calibratedParameter : sequence of float
    The calibrated value of the parameter.
observationsError : :class:`~openturns.Distribution`
    The distribution of the observations error.
inputObservations : :class:`~openturns.Sample`
    The sample of input observations.
outputObservations : :class:`~openturns.Sample`
    The sample of output observations.
residualFunction : :class:`~openturns.Function`
    The residual function.
bootstrapSize : UnsignedInteger
    The bootstrap size used to compute parameterPosterior (or 0 if none was used).

See also
--------
GaussianCalibrationResult, LeastSquaresCalibrationResult

Notes
-----
The residual function returns `model(inputObservations) - outputObservations`.

The following :class:`~openturns.ResourceMap` keys are used:

- ``CalibrationResult-PriorLineStyle`` (``String``, default: ``dashed``): line style for prior visualization,
- ``CalibrationResult-PosteriorLineStyle`` (``String``, default: ``dotdash``): line style for posterior visualization,
- ``CalibrationResult-ObservationLineStyle`` (``String``, default: ``solid``): line style for observation visualization,
- ``CalibrationResult-PriorPointStyle`` (``String``, default: ``circle``): point style for prior visualization,
- ``CalibrationResult-PosteriorPointStyle`` (``String``, default: ``diamond``): point style for posterior visualization,
- ``CalibrationResult-ObservationPointStyle`` (``String``, default: ``fsquare``): point style for observation visualization.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::CalibrationResult::isBayesian
"Bayesian method accessor.

Returns
-------
bayesian : bool
    Whether the method is Bayesian"

// ---------------------------------------------------------------------

%feature("docstring") OT::CalibrationResult::getObservationsError
"Accessor to the observations error distribution.

Returns
-------
observationsError : :class:`~openturns.Distribution`
    The observations error distribution."

// ---------------------------------------------------------------------

%feature("docstring") OT::CalibrationResult::getInputObservations
"Accessor to the input observations.

Returns
-------
inputObservations : :class:`~openturns.Sample`
    The sample of input observations."

// ---------------------------------------------------------------------

%feature("docstring") OT::CalibrationResult::setInputObservations
"Accessor to the input observations.

Parameters
----------
inputObservations : :class:`~openturns.Sample`
    The sample of input observations."

// ---------------------------------------------------------------------

%feature("docstring") OT::CalibrationResult::getOutputObservations
"Accessor to the output observations.

Returns
-------
outputObservations : :class:`~openturns.Sample`
    The sample of output observations."

// ---------------------------------------------------------------------

%feature("docstring") OT::CalibrationResult::setOutputObservations
"Accessor to the output observations.

Parameters
----------
outputObservations : :class:`~openturns.Sample`
    The sample of output observations."

// ---------------------------------------------------------------------

%feature("docstring") OT::CalibrationResult::getResidualFunction
"Accessor to the residual function.

Returns
-------
residualFunction : :class:`~openturns.Function`
    The residual function."

// ---------------------------------------------------------------------

%feature("docstring") OT::CalibrationResult::getBootstrapSize
"Accessor to the bootstrap size used to sample the posterior distribution.

Returns
-------
bootstrapSize : int
    Bootstrap size used to sample the posterior distribution.
    A value of 0 means that no bootstrap was performed and that the posterior
    distribution was computed using a linearization of the model."

// ---------------------------------------------------------------------

%feature("docstring") OT::CalibrationResult::setOutputAtPriorAndPosteriorMean
"Accessor to the output at prior/posterior mean.

Parameters
----------
outputAtPrior : :class:`~openturns.Sample`
    The sample of output at prior mean.
outputAtPosterior : :class:`~openturns.Sample`
    The sample of output at posterior mean."

// ---------------------------------------------------------------------

%feature("docstring") OT::CalibrationResult::getOutputAtPriorMean
"Accessor to the output observations.

Returns
-------
outputAtPrior : :class:`~openturns.Sample`
    Output at prior mean."

// ---------------------------------------------------------------------

%feature("docstring") OT::CalibrationResult::getOutputAtPosteriorMean
"Accessor to the output observations.

Returns
-------
outputAtPosterior : :class:`~openturns.Sample`
    Output at posterior mean."

// ---------------------------------------------------------------------

%feature("docstring") OT::CalibrationResult::drawParameterDistributions
"Draw parameter prior/posterior.

Plots the prior and posterior distribution of the calibrated parameter theta.

Returns
-------
grid : :class:`~openturns.GridLayout`
    Graph array."

// ---------------------------------------------------------------------

%feature("docstring") OT::CalibrationResult::drawResiduals
"Draw residuals.

Plot the distribution of the sample residuals before and after calibration 
using kernel smoothing and the distribution of the observation errors.

Returns
-------
grid : :class:`~openturns.GridLayout`
    Graph array."

// ---------------------------------------------------------------------

%feature("docstring") OT::CalibrationResult::drawObservationsVsInputs
"Draw observations/inputs.

Plot the observed output of the model depending on the observed input before
and after calibration.

Returns
-------
grid : :class:`~openturns.GridLayout`
    Graph array."

// ---------------------------------------------------------------------

%feature("docstring") OT::CalibrationResult::drawObservationsVsPredictions
"Draw observations/predictions.

Plots the output of the model depending on the output observations before and
after calibration.

Returns
-------
grid : :class:`~openturns.GridLayout`
    Graph array."

// ---------------------------------------------------------------------

%feature("docstring") OT::CalibrationResult::drawResidualsNormalPlot
"Draw residuals normal plot.

Plots the quantile-quantile graphs of the empirical residual 
after calibration vs the Gaussian distribution.

Returns
-------
grid : :class:`~openturns.GridLayout`
    Graph array."
