%feature("docstring") OT::LeastSquaresCalibrationResult
"Least squares calibration result.

Returned by :class:`~openturns.LinearLeastSquaresCalibration` and :class:`~openturns.NonLinearLeastSquaresCalibration`.

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
CalibrationResult

Examples
--------
>>> import openturns as ot

# assume we obtained a result from a least squares calibration algorithm

>>> result = ot.LeastSquaresCalibrationResult()
>>> param = result.getParameter()
>>> distribution = result.getParameterDistribution()
>>> graph1 = result.drawParameterDistributions()  # doctest: +SKIP
>>> graph2 = result.drawResiduals()  # doctest: +SKIP
>>> graph3 = result.drawObservationsVsInputs()  # doctest: +SKIP
>>> graph4 = result.drawObservationsVsPredictions()  # doctest: +SKIP
"

// ---------------------------------------------------------------------

%feature("docstring") OT::LeastSquaresCalibrationResult::isBayesian
"Bayesian method accessor.

Returns
-------
False"

// ---------------------------------------------------------------------

%feature("docstring") OT::LeastSquaresCalibrationResult::getParameterDistribution
"Accessor to the distribution of the calibrated parameter.

Returns
-------
parameterDistribution : :class:`~openturns.Distribution`
    The distribution of the calibrated parameter."

// ---------------------------------------------------------------------

%feature("docstring") OT::LeastSquaresCalibrationResult::getParameter
"Accessor to the calibrated value of the parameter.

Returns
-------
parameter : :class:`~openturns.Point`
    The calibrated value of the parameter."
