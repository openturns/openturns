%feature("docstring") OT::GaussianNonLinearCalibration
"Gaussian non linear calibration algorithm (3DVAR).

Parameters
----------
model : :class:`~openturns.Function`
    The parametric function to be calibrated.
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

Notes
-----
This class implements the Maximum A Posteriori (MAP) estimator for a nonlinear model
under the Bayesian hypothesis that the prior and the observation errors have Gaussian distributions. 
This algorithm is also known as *3DVAR*. 

The given observation error covariance can be either *local*,
i.e. the same matrix applies for each observation and is of dimension the output
dimension of the model, or *global*, i.e. the matrix describes the full set of
observation errors, in which case its dimension is the product of the output
dimension of the model and the number of observations.

The prior distribution of the parameter is :class:`~openturns.Normal`.  

The posterior distribution of the parameter is :class:`~openturns.Normal` when
no bounds are set, or when the bounds are handled with the default active-set
approach.

Bounds can be set with :meth:`setBounds`. Their handling is selected by the
``GaussianNonLinearCalibration-BoundsHandling`` key of the
:class:`~openturns.ResourceMap`:

- ``ActiveSet`` (default): the optimization is constrained, and the posterior
  covariance is computed from the free block of the extended design matrix at
  the MAP. Parameters fixed at a bound have a zero variance. This is a
  first-order active-set approximation.
- ``Truncated``: the Gaussian surrogate of the Gauss-Newton Laplace
  approximation at the MAP is truncated to the bounds, which returns a
  :class:`~openturns.TruncatedDistribution`. Its mean is shifted by the
  first-order gradient term at the constrained MAP, so that an affine model
  reproduces the :class:`~openturns.GaussianLinearCalibration` truncated
  posterior. This is the exact posterior of the Gaussian (Laplace) surrogate,
  not of the nonlinear model. The MAP is the mode of the truncated
  distribution, not its mean. The current
  :class:`~openturns.TruncatedDistribution` only computes its moments in
  reasonable time for dimensions up to about three, so this option is not the
  default. Moreover, the truncated distribution is numerically representable
  only if the bounds leave a non-negligible probability under the Gaussian
  surrogate; when the MAP lies far at a bound, the ``ActiveSet`` handling is
  more robust.

The ``Truncated`` handling is ignored when
``GaussianNonLinearCalibration-BootstrapSize`` is nonzero, because the
posterior is then estimated from the bootstrap sample.

When the linearization at the MAP is ill-conditioned, the MAP and the active set
are determined only up to the resolution of the residual sum of squares: a
direction whose contribution is below the machine precision is not identified,
the returned MAP depends on the optimization algorithm and on its tolerances,
and the posterior variance in that direction is large. Rescaling the
parameterization or relying on the prior distribution resolves the direction.
See :any:`ill_conditioned_calibration` for details.

Depending on the value of the `GaussianNonLinearCalibration-BootstrapSize` key 
in the :class:`~openturns.ResourceMap`, the parameters are estimated differently.

- If `GaussianNonLinearCalibration-BootstrapSize`
  is set to a nonzero positive integer, then a bootstrap resampling of the observations
  is performed and the posterior distribution is based on a :class:`~openturns.KernelSmoothing`
  of the sample of bootstrap optimum parameters.
- If `GaussianNonLinearCalibration-BootstrapSize`
  is set to zero, then the posterior distribution is
  evaluated based on a linear approximation of the model at the optimum. 
  This corresponds to using :class:`~openturns.GaussianLinearCalibration` at the optimum,
  and is named *Laplace approximation* in the Bayesian context.

The default value of `GaussianNonLinearCalibration-BootstrapSize` is nonzero, 
meaning that a bootstrap is used by default.
This can be costly in some practical use cases, because it requires
several runs of the optimization algorithm.
In this case, please set the bootstrap size to zero.

The following :class:`~openturns.ResourceMap` keys are used:

- ``GaussianNonLinearCalibration-BootstrapSize`` (``UnsignedInteger``, default: ``100``): number of bootstrap resamples used to estimate the posterior distribution; zero selects the Laplace approximation
- ``GaussianNonLinearCalibration-BoundsHandling`` (``String``, default: ``ActiveSet``): handling of the posterior distribution when bounds are set and the bootstrap size is zero. The possible values are ``ActiveSet`` and ``Truncated``
- ``GaussianLinearCalibration-Method`` (``String``, default: ``QR``): name of the decomposition method used for the Laplace approximation
- ``LinearLeastSquaresCalibration-ActiveBoundTolerance`` (``Scalar``, default: ``1e-6``): absolute distance to a bound below which a parameter is considered fixed at this bound when bounds are set

The resulting error distribution is :class:`~openturns.Normal`
with a mean estimated from the sample mean of the residuals and with a
covariance matrix equal to the `errorCovariance` input argument.

Please read read :any:`gaussian_calibration` for more details
on Gaussian calibration.

See also
--------
GaussianLinearCalibration, LinearLeastSquaresCalibration, NonLinearLeastSquaresCalibration

Examples
--------
Calibrate a nonlinear model using 3DVAR:

>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> m = 10
>>> x = [[0.5 + i] for i in range(m)]
>>> inVars = ['a', 'b', 'c', 'x']
>>> formulas = ['a + b * exp(c * x)']
>>> model = ot.SymbolicFunction(inVars, formulas)
>>> p_ref = [2.8, 1.2, 0.5]
>>> params = [0, 1, 2]
>>> modelX = ot.ParametricFunction(model, params, p_ref)
>>> y = modelX(x)
>>> y += ot.Normal(0.0, 0.05).getSample(m)
>>> parameterMean = [1.0]*3
>>> priorCovariance = ot.CovarianceMatrix(3)
>>> errorCovariance = ot.CovarianceMatrix(1, [0.1])
>>> algo = ot.GaussianNonLinearCalibration(modelX, x, y, parameterMean, priorCovariance, errorCovariance)
>>> algo.run()  # doctest: +SKIP
>>> pmap = algo.getResult().getParameterMAP()  # doctest: +SKIP"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianNonLinearCalibration::getParameterMean
"Accessor to the parameter `parameterMean`.

Returns
-------
parameterMean : :class:`~openturns.Point`
    Parameter `parameterMean`."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianNonLinearCalibration::getParameterCovariance
"Accessor to the parameter prior covariance.

Returns
-------
prior : :class:`~openturns.CovarianceMatrix`
    Parameter prior covariance."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianNonLinearCalibration::getErrorCovariance
"Accessor to the observation error covariance.

Returns
-------
error : :class:`~openturns.CovarianceMatrix`
    Observation error covariance."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianNonLinearCalibration::getGlobalErrorCovariance
"Accessor to the flag for a global observation error covariance.

Returns
-------
flag : bool
    Flag telling if the given observation error covariance is global or not."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianNonLinearCalibration::getOptimizationAlgorithm
"Accessor to the optimization algorithm used for the computation.

Returns
-------
algo : :class:`~openturns.OptimizationAlgorithm`
    Optimization algorithm used for the computation."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianNonLinearCalibration::setOptimizationAlgorithm
"Accessor to the optimization algorithm used for the computation.

Parameters
----------
algo : :class:`~openturns.OptimizationAlgorithm`
    Optimization algorithm to use for the computation."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianNonLinearCalibration::getBootstrapSize
"Accessor to the bootstrap size used to sample the posterior distribution.

Returns
-------
size : int
    Bootstrap size used to sample the posterior distribution. A value of 0
    means that no bootstrap has been done but a linear approximation has
    been used to get the posterior distribution, using the :class:`~openturns.GaussianLinearCalibration`
    algorithm at the maximum a posteriori estimate."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianNonLinearCalibration::setBootstrapSize
"Accessor to the bootstrap size used to sample the posterior distribution.

Parameters
----------
size : int
    Bootstrap size used to sample the posterior distribution. A value of 0
    means that no bootstrap has to be done but a linear approximation has
    been used to get the posterior distribution, using the :class:`~openturns.GaussianLinearCalibration`
    algorithm at the maximum a posteriori estimate."

