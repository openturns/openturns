%feature("docstring") OT::NonLinearLeastSquaresCalibration
"Non-linear least-squares calibration algorithm.

Parameters
----------
model : :class:`~openturns.Function`
    The parametric function to be calibrated.
inputObservations : 2-d sequence of float
    The sample of input observations.
    Can have dimension 0 to specify no observations.
outputObservations : 2-d sequence of float
    The sample of output observations.
startingPoint : sequence of float
    Initial parameter value used as the starting point of the optimization.

Notes
-----

NonLinearLeastSquaresCalibration estimates the parameter of a model by
minimizing the sum of squared residuals between the model predictions and
the observed outputs.
Under standard assumptions on the observation errors, the resulting
estimator coincides with the maximum likelihood estimator for Gaussian
errors.
Unlike :class:`~openturns.LinearLeastSquaresCalibration`, this algorithm
does not require the model to depend linearly on the parameter.

The prior distribution of the parameter is approximated by a Normal
distribution centered on the `startingPoint` with a very large variance,
so that the influence of the prior is negligible compared with that of
the observations.

The posterior distribution of the parameter reflects the sampling
variability of the estimated parameter.
If the key `NonLinearLeastSquaresCalibration-BootstrapSize` in the :class:`~openturns.ResourceMap` 
is set to zero, the posterior distribution is evaluated based on a linear approximation 
of the model at the optimum. 
This corresponds to using the :class:`~openturns.LinearLeastSquaresCalibration` at the optimum, 
and is named *Laplace approximation* in the Bayesian context. 
However, if the key `NonLinearLeastSquaresCalibration-BootstrapSize` in the :class:`~openturns.ResourceMap` 
is set to a nonzero positive integer, then a bootstrap resampling of the observations 
is performed and the posterior distribution is based on a :class:`~openturns.KernelSmoothing` 
of the sample of parameter estimates obtained from the bootstrap resamples.
By default, a bootstrap algorithm is used.

The resulting distribution of the output error is a :class:`~openturns.Normal` and is 
computed from the residuals. 

If least squares optimization algorithms are enabled, then the algorithm used is the
first found by `Build` of :class:`~openturns.OptimizationAlgorithm`.
Otherwise, the algorithm :class:`~openturns.TNC` is used.

Please read :any:`code_calibration` for more details.

See also
--------
GaussianLinearCalibration, LinearLeastSquaresCalibration, GaussianNonLinearCalibration

Examples
--------
Calibrate a nonlinear model using non-linear least-squares:

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
>>> startingPoint = [1.0]*3
>>> algo = ot.NonLinearLeastSquaresCalibration(modelX, x, y, startingPoint)
>>> algo.run()
>>> print(algo.getResult().getParameterMAP())
[2.773...,1.203...,0.499...]"

// ---------------------------------------------------------------------

%feature("docstring") OT::NonLinearLeastSquaresCalibration::getStartingPoint
"Return the starting point used for the optimization.

Returns
-------
startingPoint : :class:`~openturns.Point`
    Starting point used for the optimization."

// ---------------------------------------------------------------------

%feature("docstring") OT::NonLinearLeastSquaresCalibration::getOptimizationAlgorithm
"Return the optimization algorithm used for the computation.

Returns
-------
algo : :class:`~openturns.OptimizationAlgorithm`
    Optimization algorithm used for the computation."

// ---------------------------------------------------------------------

%feature("docstring") OT::NonLinearLeastSquaresCalibration::setOptimizationAlgorithm
"Set the optimization algorithm used for the computation.

Parameters
----------
algo : :class:`~openturns.OptimizationAlgorithm`
    Optimization algorithm to use for the computation."

// ---------------------------------------------------------------------

%feature("docstring") OT::NonLinearLeastSquaresCalibration::getBootstrapSize
"Return the bootstrap size used to sample the posterior distribution.

Returns
-------
size : int
    Bootstrap size used to sample the posterior distribution.
    A value of 0 means that no bootstrap is performed and that the posterior
    distribution is approximated using a linearization of the model around
    the optimum, equivalent to applying :class:`~openturns.LinearLeastSquaresCalibration`."

// ---------------------------------------------------------------------

%feature("docstring") OT::NonLinearLeastSquaresCalibration::setBootstrapSize
"Set the bootstrap size used to sample the posterior distribution.

Parameters
----------
size : int
    Bootstrap size used to sample the posterior distribution.
    A value of 0 means that no bootstrap is performed and that the posterior
    distribution is approximated using a linearization of the model around
    the optimum, equivalent to applying :class:`~openturns.LinearLeastSquaresCalibration`."

// ---------------------------------------------------------------------

%feature("docstring") OT::NonLinearLeastSquaresCalibration::BuildResidualFunction
R"RAW(Build a residual function given a parametric model, input and output observations.

Parameters
----------
model : :class:`~openturns.Function`
    Parametric model.
inputObservations : 2-d sequence of float
    Input observations associated to the output observations.
outputObservations : 2-d sequence of float
    Output observations.

Returns
-------
residual : :class:`~openturns.Function`
    Residual function.

Notes
-----
Given a parametric model :math:`F_{\theta}:\Rset^\inputDim \rightarrow \Rset^p` with
parameter :math:`\theta \in \Rset^m`, a sample of input points
:math:`(\vect{x}_i)_{i=1, \dots, \sampleSize}` and the associated output
:math:`(\vect{y}_i)_{i=1, \dots, \sampleSize}`,
the residual function :math:`f : \Rset^m \rightarrow \Rset^{p \sampleSize}` is defined by:

.. math::

    f(\theta)=
    \begin{pmatrix}
    F_{\theta}(\vect{x}_1) - \vect{y}_1 \\
    \vdots \\
    F_{\theta}(\vect{x}_\sampleSize) - \vect{y}_\sampleSize
    \end{pmatrix}

for all :math:`\theta\in\Rset^m`.
The residual vector is obtained by stacking the residuals associated with
all observations.

)RAW"
