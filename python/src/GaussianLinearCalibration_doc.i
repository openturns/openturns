%feature("docstring") OT::GaussianLinearCalibration
"Gaussian linear calibration algorithm.

Available constructors:
    GaussianLinearCalibration(*model, inputObservations, outputObservations, parameterMean, parameterCovariance, errorCovariance, methodName*)

    GaussianLinearCalibration(*modelObservations, gradientObservations, outputObservations, parameterMean, parameterCovariance, errorCovariance, methodName*)

Parameters
----------
model : :class:`~openturns.Function`
    The parametric function to be calibrated. Its inputs should correspond to the calibrated parameters
    and its parameters should correspond to the input observations.
inputObservations : 2-d sequence of float
    The sample of input observations.
    Can have dimension 0 to specify no observations. 
outputObservations : 2-d sequence of float
    The sample of output observations.
parameterMean : sequence of float
    The mean of the Gaussian prior distribution of the parameter.
parameterCovariance : 2-d sequence of float
    The covariance matrix of the Gaussian prior distribution of the parameter.
errorCovariance : 2-d sequence of float
    The covariance matrix of the Gaussian distribution of the observations error.
methodName : str
    The name of the least-squares method to use for the calibration.
    By default, equal to *QR*. Possible values are *SVD*, *QR*, *Cholesky*.
modelObservations : 2-d sequence of float
    The sample of output values of the model.
gradientObservations : 2-d sequence of float
    The Jacobian matrix of the model with respect to the parameter.

Notes
-----
This class implements the Maximum A Posteriori (MAP) estimator for a linear model
under the Bayesian hypothesis that the prior and the observation errors have Gaussian distributions. 
This is also known as the Kalman filter. 

The given observation error covariance can be either *local*, ie the same matrix applies for each observation and is of
dimension the output dimension of the model, or *global*, ie the matrix describes
the full set of observation errors, in which case its dimension is the product of
the output dimension of the model and the number of observations.

The prior distribution of the parameter is a :class:`~openturns.Normal`.

The posterior distribution of the parameter is :class:`~openturns.Normal`
when no bounds are set, or when the bounds are handled with the default
active-set approach.

Bounds can be set with :meth:`setBounds`. Their handling is selected by the
``GaussianLinearCalibration-BoundsHandling`` key of the
:class:`~openturns.ResourceMap`:

- ``ActiveSet`` (default): the optimization is constrained, and the posterior
  covariance is computed from the free block of the extended design matrix at
  the MAP. Parameters fixed at a bound have a zero variance. This is a
  first-order active-set approximation.
- ``Truncated``: the unconstrained Gaussian posterior is truncated to the
  bounds, which returns a :class:`~openturns.TruncatedDistribution`. This is
  the exact posterior of the linear-Gaussian model with bounds. The MAP is the
  mode of the truncated distribution, not its mean. The current
  :class:`~openturns.TruncatedDistribution` computes its moments by numerical
  integration, which is practical only up to about three dimensions, so this
  option is not the default; the calibration result computes them only when
  they are requested. Moreover, the truncated distribution is numerically
  representable only if the bounds leave a non-negligible probability under
  the unconstrained posterior; when the data are very informative and the MAP
  lies far outside the bounds, the ``ActiveSet`` handling is more robust.

The following :class:`~openturns.ResourceMap` keys are used:

- ``GaussianLinearCalibration-Method`` (``String``, default: ``QR``): name of the decomposition method used to solve the least squares problem of the Gaussian linear calibration. The possible values are ``SVD``, ``Cholesky`` and ``QR``
- ``GaussianLinearCalibration-BoundsHandling`` (``String``, default: ``ActiveSet``): handling of the posterior distribution when bounds are set. The possible values are ``ActiveSet`` and ``Truncated``
- ``LinearLeastSquaresCalibration-ActiveBoundTolerance`` (``Scalar``, default: ``1e-6``): absolute distance to a bound below which a parameter is considered fixed at this bound when bounds are set

The resulting error distribution is :class:`~openturns.Normal`
with a zero mean and with a covariance matrix equal to the
`errorCovariance` input argument.

Please read :any:`gaussian_calibration` for more details.

See also
--------
LinearLeastSquaresCalibration, NonLinearLeastSquaresCalibration, GaussianNonLinearCalibration

Examples
--------
Calibrate a non-linear model using GaussianLinearCalibration:

>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> m = 10
>>> x = [[0.5 + i] for i in range(m)]
>>> inVars = ['a', 'b', 'c', 'x']
>>> formulas = ['a + b * exp(c * x)']
>>> g = ot.SymbolicFunction(inVars, formulas)
>>> paramRef = [2.8, 1.2, 0.5]
>>> paramIndices = [0, 1, 2]
>>> model = ot.ParametricFunction(g, paramIndices, paramRef)
>>> y = model(x)
>>> y += ot.Normal(0.0, 0.05).getSample(m)
>>> inputIndices = [3]
>>> calibrationModel = ot.ParametricFunction(g, inputIndices, [0.0] * len(inputIndices))
>>> parameterMean = [1.0] * 3
>>> priorCovariance = ot.CovarianceMatrix(3)
>>> errorCovariance = ot.CovarianceMatrix(1, [0.1])
>>> method = 'SVD'
>>> algo = ot.GaussianLinearCalibration(calibrationModel, x, y, parameterMean, priorCovariance, errorCovariance, method)
>>> algo.run()
>>> print(algo.getResult().getParameterMAP())
[8.11483,0.0770992,0.992927]"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianLinearCalibration::getModelObservations
"Accessor to the model evaluation at the `parameterMean`.

Returns
-------
modelObservation : :class:`~openturns.Sample`
    Evaluation of the model at the `parameterMean` point."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianLinearCalibration::getGradientObservations
"Accessor to the model gradient at the `parameterMean`.

Returns
-------
gradientObservation : :class:`~openturns.Matrix`
    Gradient of the model at the `parameterMean` point."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianLinearCalibration::getParameterMean
"Accessor to the parameter `parameterMean`.

Returns
-------
parameterMean : :class:`~openturns.Point`
    Parameter parameterMean."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianLinearCalibration::getParameterCovariance
"Accessor to the parameter prior covariance.

Returns
-------
prior : :class:`~openturns.CovarianceMatrix`
    Parameter prior covariance."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianLinearCalibration::getErrorCovariance
"Accessor to the observation error covariance.

Returns
-------
error : :class:`~openturns.CovarianceMatrix`
    Observation error covariance."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianLinearCalibration::getGlobalErrorCovariance
"Accessor to the flag for a global observation error covariance.

Returns
-------
flag : bool
    Flag telling if the given observation error covariance is global or not."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianLinearCalibration::getMethodName
"Accessor to the name of least-squares method used for the resolution.

Returns
-------
name : str

Notes
-----
The following :class:`~openturns.ResourceMap` key is used:

- ``GaussianLinearCalibration-Method`` (``String``, default: ``QR``): name of the decomposition method used to solve the least squares problem of the Gaussian linear calibration. The possible values are ``SVD``, ``Cholesky`` and ``QR``

    Name of least-squares method used for the resolution."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianLinearCalibration::getResult
"Accessor to the result object.

Returns
-------
result : :class:`~openturns.experimental.GaussianCalibrationResult`
    Result object."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianLinearCalibration::run
"Launch the algorithm.

Notes
-----
It launches the algorithm and creates a :class:`~openturns.experimental.GaussianCalibrationResult`
structure containing all the results."
