%feature("docstring") OT::LinearLeastSquaresCalibration
R"RAW(Linear least squares calibration algorithm.

Available constructors:
    LinearLeastSquaresCalibration(*model, inputObservations, outputObservations, startingPoint, methodName*)

    LinearLeastSquaresCalibration(*modelObservations, gradientObservations, outputObservations, startingPoint, methodName*)

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
    The reference value of the parameter, where the linearization
    of the parametric model is evaluated.
methodName : str
    The name of the least-squares method to use for the calibration.
    By default, equal to *QR*. Possible values are *SVD*, *QR*, *Cholesky*.
modelObservations : 2-d sequence of float
    The sample of output values of the model.
gradientObservations : 2-d sequence of float
    The Jacobian matrix of the model with respect to the parameter.

Notes
-----
LinearLeastSquaresCalibration is the minimum variance estimator of the parameter of a given model under the assumption that this parameter acts linearly in the model.

By default the calibration is unconstrained. When bounds are set with
:meth:`setBounds`, the estimator solves the equivalent convex quadratic problem
with the first algorithm selected by
:meth:`~openturns.OptimizationAlgorithm.Build`, which is the
:class:`~openturns.experimental.HiGHS` quadratic solver when OpenTURNS is built
with HiGHS support. When some parameters are active at a bound, the posterior
distribution is a first-order Gaussian approximation: its covariance is
computed from the inverse of the free block
:math:`\mat{J}_F^T \mat{J}_F` of the Jacobian, and the residual degrees of
freedom exclude the parameters fixed at a bound.

A parameter is considered fixed when its distance to a bound is lower than the
absolute tolerance given by the
``LinearLeastSquaresCalibration-ActiveBoundTolerance`` key of the
:class:`~openturns.ResourceMap` (1.0e-6 by default). The tolerance is absolute,
so a parameter lying inside the feasible interval of a bound with a large
magnitude is not considered fixed. A rank-deficient free Jacobian block raises
an exception.

The prior distribution of the parameter is a noninformative prior
emulated using a flat :class:`~openturns.Normal` centered on the `startingPoint` and with a variance equal to SpecFunc.Infinity.

The posterior distribution of the parameter is :class:`~openturns.Normal` and reflects the 
variability of the optimum parameter depending on the observation sample. 
The associated covariance matrix may be regularized depending on the value of the
``LinearLeastSquaresCalibration-Regularization`` key of the :class:`~openturns.ResourceMap`;
its default value is :math:`10^{-12}`.
Let :math:`\epsilon` be this value and :math:`\sigma_1` the largest singular value
of the covariance matrix. If :math:`\epsilon` is nonzero, then the singular values
of the covariance matrix are increased by :math:`\epsilon \sigma_1`.
When bounds are set, only the free directions are regularized: a parameter fixed
at a bound keeps a zero variance.
With the default value :math:`\epsilon = 10^{-12}`, this shift is negligible.

The resulting distribution of the output error is :class:`~openturns.Normal` with a zero mean
and a diagonal covariance matrix computed from the residuals.
The residuals are computed based on the linearization of the model,
where the Jacobian matrix is evaluated at the `startingPoint`.
The diagonal of the covariance matrix of the output error
is constant and is estimated with the unbiased variance estimator.

Please read :any:`code_calibration` for more details.


The following :class:`~openturns.ResourceMap` keys are used:

- ``LinearLeastSquaresCalibration-Method`` (``String``, default: ``QR``): name of the decomposition method used to solve the least squares problem of the linear least squares calibration. The possible values are ``SVD``, ``Cholesky`` and ``QR``
- ``LinearLeastSquaresCalibration-Regularization`` (``Scalar``, default: ``1e-12``): relative shift applied to the singular values of the parameter covariance matrix
- ``LinearLeastSquaresCalibration-ActiveBoundTolerance`` (``Scalar``, default: ``1e-6``): absolute distance to a bound below which a parameter is considered fixed at this bound when bounds are set

See also
--------
GaussianLinearCalibration, NonLinearLeastSquaresCalibration, GaussianNonLinearCalibration

Examples
--------
Calibrate a nonlinear model using linear least-squares:

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
>>> method = 'SVD'
>>> algo = ot.LinearLeastSquaresCalibration(modelX, x, y, startingPoint, method)
>>> algo.run()
>>> print(algo.getResult().getParameterMAP())
[8.24019,0.0768046,0.992957]

Calibrate a linear model with bounds on the parameters. The first parameter is
active at its upper bound:

>>> modelLinear = ot.SymbolicFunction(['a', 'b', 'x'], ['a * x + b'])
>>> modelXLinear = ot.ParametricFunction(modelLinear, [0, 1], [1.0, 0.0])
>>> xLinear = [[1.0], [2.0], [3.0]]
>>> yLinear = modelXLinear(xLinear)
>>> boundedAlgo = ot.LinearLeastSquaresCalibration(
...     modelXLinear, xLinear, yLinear, [1.0, 0.0], method
... )
>>> boundedAlgo.setBounds(ot.Interval([0.0, 0.0], [0.5, 10.0]))
>>> boundedAlgo.run()
>>> print([round(value, 3) for value in boundedAlgo.getResult().getParameterMAP()])
[0.5, 1.0]
>>> #
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquaresCalibration::getModelObservations
"Accessor to the model evaluation at the `startingPoint`.

Returns
-------
modelObservation : :class:`~openturns.Sample`
    Evaluation of the model at the `startingPoint`."

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquaresCalibration::getGradientObservations
"Accessor to the model gradient at the startingPoint.

Returns
-------
gradientObservation : :class:`~openturns.Matrix`
    Gradient of the model at the startingPoint point."

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquaresCalibration::getStartingPoint
"Accessor to the parameter startingPoint.

Returns
-------
startingPoint : :class:`~openturns.Point`
    Parameter startingPoint."

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquaresCalibration::getMethodName
"Accessor to the name of least-squares method used for the resolution.

Returns
-------
name : str
    Name of least-squares method used for the resolution."

