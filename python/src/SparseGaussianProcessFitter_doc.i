%feature("docstring") OT::SparseGaussianProcessFitter
R"RAW(Fit sparse Gaussian process models.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Refer to the theory of the sparse Gaussian process, based on the variational inference of
Titsias [titsias2009]_ (see also [wenliang2021]_ for the whitened parametrisation).

We consider a Gaussian process :math:`Y` with covariance model :math:`k` and an independent
centered Gaussian noise of variance :math:`\sigma^2` on the observations
:math:`\vect{y} = (y_1, \dots, y_n)^t \in \Rset^n`.
Let :math:`p(y \mid f) = \cN(y \mid f, \sigma^2 \mat{I}_n)` denote the likelihood of
the observations given the latent function values :math:`\vect{f}`.

The model is approximated by introducing a set of :math:`m` inducing inputs
:math:`\mat{Z} = (\vect{z}_1, \dots, \vect{z}_m)^t \in \Rset^{\inputDim \times m}` with
:math:`m \leq n`. The inducing inputs lie in the same input space as the training data.
They are associated with inducing variables
:math:`\vect{u} = (u_1, \dots, u_m)^t` corresponding to the evaluation of the Gaussian
process at the inducing inputs, so that
:math:`p(\vect{u} \mid \mat{Z}) = \cN(\vect{u} \mid \vect{0}, \mat{K}_{uu})` with
:math:`\mat{K}_{uu} = k(\mat{Z}, \mat{Z})`. The joint distribution of the latent
function values :math:`\vect{f}` at the training inputs :math:`\mat{X}` and the inducing
variables :math:`\vect{u}` is:

.. math::

    p(\vect{f}, \vect{u} \mid \mat{X}, \mat{Z}) = \cN\left(
    \vect{0}, \begin{pmatrix} \mat{K}_{ff} & \mat{K}_{fu} \\ \mat{K}_{uf} & \mat{K}_{uu}
    \end{pmatrix}\right)

where :math:`\mat{K}_{ff} = k(\mat{X}, \mat{X})` and :math:`\mat{K}_{fu} = k(\mat{X}, \mat{Z})`.
The conditional distribution of :math:`\vect{f}` given :math:`\vect{u}` is:

.. math::

    p(\vect{f} \mid \vect{u}, \mat{X}, \mat{Z}) = \cN(\vect{f} \mid
    \mat{K}_{fu} \mat{K}_{uu}^{-1} \vect{u},\, \mat{K}_{ff} - \mat{K}_{fu}
    \mat{K}_{uu}^{-1} \mat{K}_{fu}^t)

The model is approximated by introducing a variational posterior
in the whitened parametrisation:

.. math::

    q(\vect{w}) = \cN(\vect{m}_w, \mat{S}_{ww})

The optimal variational parameters are available in closed form given the covariance model
parameters, the inducing inputs and the noise standard deviation:

.. math::

    \vect{m}_w = \mat{B}^{-1} \mat{A}^t \vect{y}, \qquad
    \mat{S}_{ww} = \sigma^2 \mat{B}^{-1}

with :math:`\mat{A} = \mat{K}_{fu} \mat{L}_{uu}^{-t}`,
:math:`\mat{B} = \sigma^2 \mat{I}_m + \mat{A}^t \mat{A}`, :math:`\mat{K}_{uu} = k(\mat{Z}, \mat{Z})`
and :math:`\mat{L}_{uu}` its Cholesky factor.

The objective function, namely the collapsed Evidence Lower Bound (ELBO), is maximized with
respect to the active parameters of the covariance model, the logarithm of the noise
standard deviation and the inducing inputs:

.. math::

    \cL = -\frac{1}{2}\left(n\ln(2\pi) + (2n - m)\ln(\sigma^2) + \ln\det(\mat{B})
    + \frac{\Vert \vect{y}\Vert^2 - \Vert \vect{c}\Vert^2}{\sigma^2}\right)
    - \frac{\operatorname{tr}(\mat{K}_{ff}) - \operatorname{tr}(\mat{A}^t\mat{A})}{2\sigma^2}

where :math:`\vect{c} = \mat{L}_B^{-1} \mat{A}^t \vect{y}` and
:math:`\mat{L}_B` is the Cholesky factor of :math:`\mat{B}`.

The behaviour of the algorithm is controlled by the following flags:

- :meth:`setOptimizeParameters` controls the optimization of the active covariance model parameters (default True),
- :meth:`setOptimizeNoiseStdDev` controls the optimization of the noise standard deviation (default True),
- :meth:`setOptimizeInducingPoints` controls the optimization of the inducing points (default False).

When the number of inducing points :math:`m` equals the number of observations :math:`n` and
the inducing points equal the observations, the ELBO reduces to the exact marginal
log-likelihood of the Gaussian process model and the predictions coincide with the ones of a
:class:`~openturns.GaussianProcessRegression`.

The optimization of the hyperparameters relies on a local optimizer
(:class:`~openturns.TNC` by default) and can converge to a degenerate optimum when the
initial inducing points are not informative enough, e.g. when they are too few or clustered:
the amplitude of the covariance model is then driven to its lower bound and the noise
standard deviation to a large value, which leads to a metamodel predicting a constant value. This is a
genuine maximum of the ELBO for such a loose approximation. Increasing the number of
inducing points, spreading them over the input domain or restarting the optimization from
different initial parameters usually avoids this configuration.

Parameters
----------
inputSample, outputSample : :class:`~openturns.Sample` or 2d-array
    The samples :math:`(\vect{x}_k)_{1 \leq k \leq \sampleSize} \in \Rset^\inputDim` and
    :math:`(\vect{y}_k)_{1 \leq k \leq \sampleSize} \in \Rset`.

covarianceModel : :class:`~openturns.CovarianceModel`
    Covariance model of the Gaussian process. Only scalar outputs are supported.

inducingPoints : :class:`~openturns.Sample` or int
    The inducing points :math:`(\vect{z}_j)_{1 \leq j \leq m}`. If an integer is given,
    the inducing points are uniformly subsampled from the input sample at approximately
    regular index intervals.

Notes
-----
This class is controlled by the following :class:`~openturns.ResourceMap` entries:

- `SparseGaussianProcessFitter-DefaultOptimizationAlgorithm` (default ``"TNC"``): the default optimization algorithm,
- `SparseGaussianProcessFitter-DefaultOptimizationLowerBound` (default 1.0e-2): the default lower bound for the covariance model parameters,
- `SparseGaussianProcessFitter-DefaultOptimizationUpperBound` (default 1.0e2): the default upper bound for the covariance model parameters,
- `SparseGaussianProcessFitter-OptimizationLowerBoundScaleFactor` (default 1.0e-3): the lower bound scale factor for the covariance model parameters,
- `SparseGaussianProcessFitter-OptimizationUpperBoundScaleFactor` (default 2.0): the upper bound scale factor for the covariance model parameters,
- `SparseGaussianProcessFitter-DefaultNoiseStdDev` (default 1.0e-3): the default noise standard deviation,
- `SparseGaussianProcessFitter-DefaultNoiseStdDevLowerBound` (default 1.0e-12): the default lower bound for the noise standard deviation,
- `SparseGaussianProcessFitter-DefaultNoiseStdDevUpperBound` (default 1.0e8): the default upper bound for the noise standard deviation,
- `SparseGaussianProcessFitter-OptimizationNormalization` (default ``True``): whether to internally scale the hyperparameters during the optimization using a min-max transformation,
- `SparseGaussianProcessFitter-LinearAlgebra` (default ``"LAPACK"``): the default linear algebra method.

Examples
--------
Create the model :math:`\model: \Rset \mapsto \Rset` and the samples:

>>> import openturns as ot
>>> from openturns.experimental import SparseGaussianProcessFitter
>>> g = ot.SymbolicFunction(['x'], ['x + x * sin(x)'])
>>> inputSample = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0]])
>>> outputSample = g(inputSample)

Create the algorithm with 3 inducing points:

>>> covarianceModel = ot.SquaredExponential([1.0])
>>> covarianceModel.setActiveParameter([0])
>>> numberOfInducingPoints = 3
>>> algo = SparseGaussianProcessFitter(inputSample, outputSample, covarianceModel, numberOfInducingPoints)
>>> algo.run()

Get the resulting structure:

>>> result = algo.getResult()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::getResult
"Get the results of the metamodel computation.

Returns
-------
result : :class:`~openturns.experimental.SparseGaussianProcessFitterResult`
    Structure containing all the results obtained after computation
    and created by the method :py:meth:`run`.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::run
"Compute the response surface.

Notes
-----
It computes the response surface and creates a
:class:`~openturns.experimental.SparseGaussianProcessFitterResult` structure containing all the results."

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::getObjectiveFunction
R"RAW(Accessor to the objective function, i.e. the collapsed ELBO.

Returns
-------
elbo : :class:`~openturns.Function`
    The collapsed ELBO as a function of the optimized parameters (active covariance model
    parameters, logarithm of the noise standard deviation and inducing points).

Notes
-----
The objective function may be useful for some postprocessing: maximization using external
optimizers for example.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::getOptimizationAlgorithm
"Accessor to the solver used to optimize the parameters.

Returns
-------
algorithm : :class:`~openturns.OptimizationAlgorithm`
    Solver used to optimize the parameters.
    Default optimizer is :class:`~openturns.TNC`"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::setOptimizationAlgorithm
"Accessor to the solver used to optimize the parameters.

Parameters
----------
algorithm : :class:`~openturns.OptimizationAlgorithm`
    Solver used to optimize the parameters."

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::setOptimizeParameters
"Accessor to the covariance model parameters optimization flag.

Parameters
----------
optimizeParameters : bool
    Whether to optimize the active covariance model parameters."

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::getOptimizeParameters
"Accessor to the covariance model parameters optimization flag.

Returns
-------
optimizeParameters : bool
    Whether to optimize the active covariance model parameters."

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::setOptimizeInducingPoints
"Accessor to the inducing points optimization flag.

Parameters
----------
optimizeInducingPoints : bool
    Whether to optimize the inducing points."

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::getOptimizeInducingPoints
"Accessor to the inducing points optimization flag.

Returns
-------
optimizeInducingPoints : bool
    Whether to optimize the inducing points."

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::setOptimizeNoiseStdDev
"Accessor to the noise standard deviation optimization flag.

Parameters
----------
optimizeNoiseStdDev : bool
    Whether to optimize the noise standard deviation."

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::getOptimizeNoiseStdDev
"Accessor to the noise standard deviation optimization flag.

Returns
-------
optimizeNoiseStdDev : bool
    Whether to optimize the noise standard deviation."

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::setNoiseStdDev
"Accessor to the noise standard deviation.

Parameters
----------
noiseStdDev : float
    The noise standard deviation :math:`\\sigma` of the sparse Gaussian process."

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::getNoiseStdDev
"Accessor to the noise standard deviation.

Returns
-------
noiseStdDev : float
    The noise standard deviation :math:`\\sigma` of the sparse Gaussian process."

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::setInducingPoints
R"RAW(Accessor to the inducing points.

Parameters
----------
inducingPoints : :class:`~openturns.Sample`
    The inducing points :math:`(\vect{z}_j)_{1 \leq j \leq m}`.
    The number of inducing points should not exceed the number of observations.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::getInducingPoints
R"RAW(Accessor to the inducing points.

Returns
-------
inducingPoints : :class:`~openturns.Sample`
    The inducing points :math:`(\vect{z}_j)_{1 \leq j \leq m}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::getMethod
"Accessor to the linear algebra method.

Returns
-------
linAlgMethod : int
    The used linear algebra method to fit the model:

    - ot.SparseGaussianProcessFitterResult.LAPACK or 0: using `LAPACK` to fit the model,

    - ot.SparseGaussianProcessFitterResult.HMAT or 1: using `HMAT` to fit the model."

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitter::setMethod
"Accessor to the linear algebra method.

Parameters
----------
linAlgMethod : int
    The used linear algebra method to fit the model:

    - ot.SparseGaussianProcessFitterResult.LAPACK or 0: using `LAPACK` to fit the model,

    - ot.SparseGaussianProcessFitterResult.HMAT or 1: using `HMAT` to fit the model."
