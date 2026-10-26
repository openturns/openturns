%feature("docstring") OT::GaussianCalibrationResult
"Gaussian calibration result.

Returned by :class:`~openturns.GaussianLinearCalibration` and :class:`~openturns.GaussianNonLinearCalibration`.

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

# assume we obtained a result from a Gaussian calibration algorithm

>>> result = ot.GaussianCalibrationResult()
>>> pmap = result.getParameterMAP()
>>> prior = result.getParameterPrior()
>>> posterior = result.getParameterPosterior()
>>> graph1 = result.drawParameterDistributions()  # doctest: +SKIP
>>> graph2 = result.drawResiduals()  # doctest: +SKIP
>>> graph3 = result.drawObservationsVsInputs()  # doctest: +SKIP
>>> graph4 = result.drawObservationsVsPredictions()  # doctest: +SKIP
"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianCalibrationResult::isBayesian
"Bayesian method accessor.

Returns
-------
True"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianCalibrationResult::getParameterPrior
"Accessor to the parameter prior distribution.

Returns
-------
parameterPrior : :class:`~openturns.Distribution`
    The prior distribution of the parameter."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianCalibrationResult::getParameterPosterior
"Accessor to the parameter posterior distribution.

Returns
-------
parameterPosterior : :class:`~openturns.Distribution`
    The posterior distribution of the parameter.

Notes
-----
The posterior distribution is only available when no bootstrap resampling was used.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianCalibrationResult::getMAPDistribution
"Accessor to the distribution of the MAP.

Returns
-------
MAPDistribution : :class:`~openturns.Distribution`
    The distribution of the MAP.

Notes
-----
The distribution of the MAP is only available when a bootstrap resampling was used.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianCalibrationResult::getParameterMAP
"Accessor to the maximum a posteriori parameter estimate.

Returns
-------
parameterPosterior : :class:`~openturns.Point`
    The maximum a posteriori parameter estimate."
