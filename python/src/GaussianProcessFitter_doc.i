%feature("docstring") OT::GaussianProcessFitter
R"RAW(Fit gaussian process models

Refer to :ref:`gaussian_process_regression`.

Parameters
----------
inputSample, outputSample : :class:`~openturns.Sample` or 2d-array
    The samples :math:`(\vect{x}_k)_{1 \leq k \leq \sampleSize} \in \Rset^\inputDim` and :math:`(\vect{y}_k)_{1 \leq k \leq \sampleSize}\in \Rset^{\outputDim}`.

covarianceModel : :class:`~openturns.CovarianceModel`
    Covariance model of the Gaussian process. See notes for the details.

basis : :class:`~openturns.Basis`
    Functional basis to estimate the trend: :math:`(\varphi_j)_{1 \leq j \leq b}: \Rset^\inputDim \rightarrow \Rset`.

    The same basis is used for each marginal output.

    Default value is `Basis(0)`, i.e. no trend to estimate.

Notes
-----

Refer to :ref:`gaussian_process_regression` (Step 1) to get all the notations and the theoretical aspects.
We only detail here the notions related to the class.

We suppose we have a sample :math:`(\vect{x}_k, \vect{y}_k)_{1 \leq k \leq \sampleSize}`
where :math:`\vect{y}_k = \model(\vect{x}_k)` for all :math:`k`, with :math:`\model:\Rset^\inputDim \mapsto
\Rset^{\outputDim}` a given function.

The class creates the Gaussian process  :math:`\vect{Y}(\omega, \vect{x})` such that the sample
:math:`(\vect{y}_k)_{1 \leq k \leq \sampleSize}` is considered as its restriction  on
:math:`(\vect{x}_k)_{1 \leq k \leq \sampleSize}`. It is defined by:

.. math::

    \vect{Y}(\omega, \vect{x}) = \vect{\mu}(\vect{x}) + \vect{W}(\omega, \vect{x})

where :math:`\vect{\mu} = (\mu_1, \dots, \mu_\outputDim)` with :math:`\mu_\ell(\vect{x}) = \sum_{j=1}^{b}
\beta_j^\ell \varphi_j(\vect{x})` and :math:`\varphi_j: \Rset^\inputDim \rightarrow \Rset` the trend function
basis for :math:`1 \leq j \leq b` and :math:`1 \leq \ell \leq \outputDim`.

Furthermore, :math:`\vect{W}` is a Gaussian process of dimension :math:`\outputDim` with zero mean and a specified
covariance model.

The *GaussianProcessFitter* class estimates the coefficients :math:`\beta_j^\ell` and :math:`\vect{p}` 
where :math:`\vect{p}` is the vector of the parameters of the covariance model  that has been declared as
*active*: see
:class:`openturns.CovarianceModel` to get details on the activation of the estimation of the other
parameters.

The estimation is done by maximizing the *reduced* log-likelihood of the mode, defined in :eq:`logLikelihoodGP`.

The default optimizer is :class:`~openturns.Cobyla` and can be changed thanks to the
:meth:`setOptimizationAlgorithm` method.
User could also change the default optimization solver by setting the entry 
`GaussianProcessFitter-DefaultOptimizationAlgorithm` of :class:`~openturns.ResourceMap` to one of the :class:`~openturns.NLopt`
solver names.

It is also possible to proceed as follows:

* ask for the reduced log-likelihood function thanks to the :meth:`getReducedLogLikelihoodFunction` method,
* optimize it with respect to the parameters :math:`\vect{\theta}` and :math:`\vect{\sigma}` using any optimization algorithms
  (that can take into account some additional constraints if needed),
* set the optimal parameter value into the covariance model used in the *GaussianProcessFitter*,
* tell the algorithm not to optimize the parameter using the :meth:`setOptimizeParameters` method.

The behaviour of the reduction is controlled by the following keys in :class:`~openturns.ResourceMap`:

- The boolean entry *GaussianProcessFitter-UseAnalyticalAmplitudeEstimate* to use the reduction associated to
  :math:`\sigma`.
  It has no effect if :math:`\outputDim > 1` or if :math:`\outputDim=1` and :math:`\sigma` is not part of :math:`\vect{p}`,
- The boolean entry *GaussianProcessFitter-UnbiasedVariance* allows one to use the *unbiased* estimate of
  :math:`\sigma` where :math:`\dfrac{1}{\sampleSize}` is replaced by :math:`\dfrac{1}{\sampleSize-\outputDim}`
  in the optimality condition for :math:`\sigma`.
- Like other machine learning techniques, heteregeneous data (i.e., data defined with different orders of magnitude)
  can have an impact on the training process of Gaussian Process Fitter.
  The boolean entry *GaussianProcessFitter-OptimizationNormalization* decides whether
  to scale input data during the hyperparameters optimization, see :doc:`/auto_surrogate_modeling/gaussian_process_regression/plot_gpr_normalization`

With huge samples, the `hierarchical matrix <http://en.wikipedia.org/wiki/Hierarchical_matrix>`_
implementation could be used if `hmat-oss` support has been enabled.

This implementation, which is based on a compressed representation of an approximated covariance matrix
(and its Cholesky factor), has a better complexity both in terms of memory requirements
and floating point operations. To use it, the entry `GaussianProcessFitter-LinearAlgebra` of the :class:`openturns.ResourceMap` class should be
instancied to `HMAT`. Default value of the key is `LAPACK`.

Examples
--------
Create the model :math:`\model: \Rset \mapsto \Rset` and the samples:

>>> import openturns as ot
>>> g = ot.SymbolicFunction(['x'], ['x + x * sin(x)'])
>>> inputSample = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0]])
>>> outputSample = g(inputSample)

Create the algorithm:

>>> g1 = ot.SymbolicFunction(['x'], ['sin(x)'])
>>> g2 = ot.SymbolicFunction(['x'], ['x'])
>>> g3 = ot.SymbolicFunction(['x'], ['cos(x)'])
>>> basis = ot.Basis([g1, g2, g3])
>>> covarianceModel = ot.SquaredExponential([1.0])
>>> covarianceModel.setActiveParameter([])
>>> algo = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
>>> algo.run()

Get the resulting  metamodel which is the trend function of the Gaussian process:

>>> result = algo.getResult()
>>> metamodel = result.getMetaModel()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitter::getResult
"Get the results of the metamodel computation.

Returns
-------
result : :class:`~openturns.GaussianProcessFitterResult`
    Structure containing all the results obtained after computation
    and created by the method :py:meth:`run`.
"

//-----------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitter::getInputSample
R"RAW(Accessor to the input sample.

Returns
-------
inputSample : :class:`~openturns.Sample`
    The input sample :math:`(\vect{x}_k)_{1 \leq k \leq \sampleSize}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitter::getOutputSample
R"RAW(Accessor to the output sample.

Returns
-------
outputSample : :class:`~openturns.Sample`
    The output sample :math:`(\vect{y}_k)_{1 \leq k \leq \sampleSize}` .)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitter::getReducedLogLikelihoodFunction()
R"RAW(Accessor to the log-likelihood function that writes as argument of the covariance's model parameters.

Returns
-------
logLikelihood : :class:`~openturns.Function`
    The log-likelihood function degined in :eq:`logLikelihoodGP` as a function of the active parameters of the covariance model.

Notes
-----
The log-likelihood function may be useful for some postprocessing: maximization using external optimizers for example.


Examples
--------
Create the model :math:`\model: \Rset \mapsto \Rset` and the samples:

>>> import openturns as ot
>>> g = ot.SymbolicFunction(['x0'], ['x0 * sin(x0)'])
>>> inputSample = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0]])
>>> outputSample = g(inputSample)

Create the algorithm:

>>> basis = ot.ConstantBasisFactory().build()
>>> covarianceModel = ot.SquaredExponential(1)
>>> algo = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
>>> algo.run()

Get the log-likelihood function:

>>> likelihoodFunction = algo.getReducedLogLikelihoodFunction()
)RAW"


// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitter::run
"Compute the response surface.

Notes
-----
It computes the response surface and creates a
:class:`~openturns.GaussianProcessFitterResult` structure containing all the results."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitter::getOptimizationAlgorithm
"Accessor to solver used to optimize the covariance model parameters.

Returns
-------
algorithm : :class:`~openturns.OptimizationAlgorithm`
    Solver used to optimize the covariance model parameters.
    Default optimizer is :class:`~openturns.Cobyla`"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitter::setOptimizationAlgorithm
"Accessor to the solver used to optimize the covariance model parameters.

Parameters
----------
algorithm : :class:`~openturns.OptimizationAlgorithm`
    Solver used to optimize the covariance model parameters."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitter::setOptimizeParameters
"Accessor to the covariance model parameters optimization flag.

Parameters
----------
optimizeParameters : bool
    Whether to optimize the covariance model parameters."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitter::getOptimizeParameters
"Accessor to the covariance model parameters optimization flag.

Returns
-------
optimizeParameters : bool
    Whether to optimize the covariance model parameters."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitter::setOptimizationBounds
"Optimization bounds accessor.

Parameters
----------
bounds : :class:`~openturns.Interval`
    Bounds for covariance model parameter optimization.

Notes
-----
Parameters involved by this method are:

 - Scale parameters,
 - Amplitude parameters if output dimension is greater than one or
   analytical sigma disabled,
 - Additional parameters.

Lower & upper bounds are defined in resource map.
Default lower upper bounds value for all parameters is
:math:`10^{-2}` and defined thanks to the
`GaussianProcessFitter-DefaultOptimizationLowerBound`
resource map key.

For scale parameters, default bounds are set as a factor of the difference between the max and min values of `X`
for each coordinate of the input sample `X` thanks to the `GaussianProcessFitter-OptimizationLowerBoundScaleFactor`
and `GaussianProcessFitter-OptimizationUpperBoundScaleFactor` :class:`~openturns.ResourceMap` entries.

Finally for other parameters (amplitude,...), default upper bound is set
to :math:`100` (corresponding resource map key is
`GaussianProcessFitter-DefaultOptimizationUpperBound`)
"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitter::getOptimizationBounds
"Optimization bounds accessor.

Returns
-------
bounds : :class:`~openturns.Interval`
    Bounds for covariance model parameter optimization."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitter::getKeepCholeskyFactor
"Keep Cholesky factor accessor.

Returns
-------
keepCholesky : bool
    Tells whether we keep or not the final Cholesky factor."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitter::setKeepCholeskyFactor
"Keep Cholesky factor setter.

Parameters
----------
keepCholesky : bool
    Tells whether we keep or not the final Cholesky factor."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitter::getMethod
" Accessor to the linear algebra method.

Returns
-------
linAlgMethod : int
    The used linear algebra method to fit the model:

    - ot.GaussianProcessFitterResult.LAPACK or 0: using `LAPACK` to fit the model,

    - ot.GaussianProcessFitterResult.HMAT or 1: using `HMAT` to fit the model."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitter::setMethod
" Accessor to the linear algebra method.

Parameters
----------
linAlgMethod : int
    The used linear algebra method to fit the model:

    - ot.GaussianProcessFitterResult.LAPACK or 0: using `LAPACK` to fit the model,

    - ot.GaussianProcessFitterResult.HMAT or 1: using `HMAT` to fit the model."
