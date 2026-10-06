%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter
R"RAW(Fit heteroscedastic sparse Gaussian process models.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

We consider observations :math:`\vect{y} = (y_1, \dots, y_n)^t \in \Rset^n`
of a latent function :math:`f` corrupted by an input-dependent Gaussian noise:

.. math::

    y_i = f(\vect{x}_i) + \varepsilon_i, \qquad
    \varepsilon_i \sim \cN(0, \exp(g(\vect{x}_i)))

with two independent Gaussian processes, :math:`f \sim \mathcal{GP}(0, k_f)` for the
mean and :math:`g \sim \mathcal{GP}(\mu_0, k_g)` for the log-variance, where
:math:`\mu_0` is a scalar prior mean level.

The posterior is approximated by sparse variational inference with
:math:`m` inducing inputs for :math:`f` and :math:`u` inducing inputs for
:math:`g`. In the whitened parametrisation the variational posteriors are
:math:`q(\vect{w}_f) = \cN(\vect{m}_f, \mat{S}_f)` and
:math:`q(\vect{w}_g) = \cN(\vect{m}_g, \mat{S}_g)`, giving the marginals
:math:`\mu_{f,i} = \vect{a}_i^t \vect{m}_f`,
:math:`v_{f,i} = k_f(\vect{x}_i, \vect{x}_i) - \Vert \vect{a}_i \Vert^2
+ \vect{a}_i^t \mat{S}_f \vect{a}_i` and
:math:`\mu_{g,i} = \mu_0 + \vect{b}_i^t \vect{m}_g`,
:math:`v_{g,i} = k_g(\vect{x}_i, \vect{x}_i) - \Vert \vect{b}_i \Vert^2
+ \vect{b}_i^t \mat{S}_g \vect{b}_i`, where :math:`\vect{a}_i` and
:math:`\vect{b}_i` are the whitened cross-covariance rows.

The uncollapsed evidence lower bound (ELBO) is maximized with respect to the
active covariance parameters, the prior mean :math:`\mu_0` and the variational
parameters:

.. math::

    \cL = \sum_{i=1}^n \left[-\frac{1}{2}\left(\ln(2\pi) + \mu_{g,i}
    + \left((y_i - \mu_{f,i})^2 + v_{f,i}\right)
    e^{-\mu_{g,i} + v_{g,i} / 2}\right)\right]
    - \mathrm{KL}_f - \mathrm{KL}_g

The expected log-likelihood is available in closed form through the lognormal
moments, so no quadrature is needed. The inducing points are fixed.

Input replicates (exactly repeated input points) are detected automatically and
merged into unique sites: all likelihood computations run on the sites with per-site
sufficient statistics (counts, sums and sums of squares), which is exact and reduces
the data-term cost from the number of observations to the number of unique sites.
Use :meth:`getUniqueInputSample`, :meth:`getReplicateCounts` and
:meth:`getReplicateMeanOutput` to inspect the detected structure.

The behaviour of the algorithm is controlled by the following flags:

- :meth:`setOptimizeParameters` controls the optimization of the active covariance model parameters and the prior mean (default True),
- :meth:`setOptimizeVariational` controls the optimization of the variational means and covariances (default True).

When the log-variance process is concentrated on a constant level, the model
reduces to the homoscedastic sparse model of
:class:`~openturns.experimental.SparseGaussianProcessFitter`.

Parameters
----------
inputSample, outputSample : :class:`~openturns.Sample` or 2d-array
    The samples :math:`(\vect{x}_k)_{1 \leq k \leq \sampleSize} \in \Rset^\inputDim` and
    :math:`(\vect{y}_k)_{1 \leq k \leq \sampleSize} \in \Rset`.

covarianceModelF : :class:`~openturns.CovarianceModel`
    Covariance model of the latent mean process. Only scalar outputs are supported.

covarianceModelG : :class:`~openturns.CovarianceModel`
    Covariance model of the log-variance process. Only scalar outputs are supported.

inducingPointsF : :class:`~openturns.Sample`
    The inducing points of the mean process.

inducingPointsG : :class:`~openturns.Sample`
    The inducing points of the log-variance process.

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``HeteroscedasticSparseGaussianProcessFitter-DefaultOptimizationAlgorithm`` (``String``, default: ``"TNC"``): the default optimization algorithm.
- ``HeteroscedasticSparseGaussianProcessFitter-DefaultOptimizationLowerBound`` (``Scalar``, default: ``1.0e-2``): the default lower bound for the covariance model parameters.
- ``HeteroscedasticSparseGaussianProcessFitter-DefaultOptimizationUpperBound`` (``Scalar``, default: ``1.0e2``): the default upper bound for the covariance model parameters.
- ``HeteroscedasticSparseGaussianProcessFitter-OptimizationLowerBoundScaleFactor`` (``Scalar``, default: ``1.0e-3``): the lower bound scale factor for the covariance model parameters.
- ``HeteroscedasticSparseGaussianProcessFitter-OptimizationUpperBoundScaleFactor`` (``Scalar``, default: ``2.0``): the upper bound scale factor for the covariance model parameters.
- ``HeteroscedasticSparseGaussianProcessFitter-VariationalBoundFactor`` (``Scalar``, default: ``10.0``): the range factor for the variational parameters bounds.

Examples
--------
Create the model :math:`\model: \Rset \mapsto \Rset` and the samples:

>>> import openturns as ot
>>> from openturns.experimental import HeteroscedasticSparseGaussianProcessFitter
>>> g = ot.SymbolicFunction(['x'], ['x + x * sin(x)'])
>>> inputSample = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0]])
>>> outputSample = g(inputSample)

Create the algorithm with 3 inducing points per process:

>>> covarianceModelF = ot.SquaredExponential([1.0])
>>> covarianceModelF.setActiveParameter([0])
>>> covarianceModelG = ot.SquaredExponential([1.0])
>>> covarianceModelG.setActiveParameter([0])
>>> algo = HeteroscedasticSparseGaussianProcessFitter(inputSample, outputSample, covarianceModelF, covarianceModelG, inputSample[0:3], inputSample[0:3])
>>> algo.run()

Get the resulting structure:

>>> result = algo.getResult()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getResult
"Get the results of the metamodel computation.

Returns
-------
result : :class:`~openturns.experimental.HeteroscedasticSparseGaussianProcessFitterResult`
    Structure containing all the results obtained after computation
    and created by the method :py:meth:`run`.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::run
"Compute the response surface.

Notes
-----
It computes the response surface and creates a
:class:`~openturns.experimental.HeteroscedasticSparseGaussianProcessFitterResult` structure containing all the results."

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getObjectiveFunction
R"RAW(Accessor to the objective function, i.e. the uncollapsed ELBO.

Returns
-------
elbo : :class:`~openturns.Function`
    The uncollapsed ELBO as a function of the optimized parameters (active covariance
    model parameters, prior mean and variational parameters).

Notes
-----
The objective function may be useful for some postprocessing: maximization using external
optimizers for example.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getOptimizationAlgorithm
"Accessor to the solver used to optimize the parameters.

Returns
-------
algorithm : :class:`~openturns.OptimizationAlgorithm`
    Solver used to optimize the parameters.
    Default optimizer is :class:`~openturns.TNC`"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::setOptimizationAlgorithm
"Accessor to the solver used to optimize the parameters.

Parameters
----------
algorithm : :class:`~openturns.OptimizationAlgorithm`
    Solver used to optimize the parameters."

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::setOptimizeParameters
"Accessor to the covariance models and prior mean optimization flag.

Parameters
----------
optimizeParameters : bool
    Whether to optimize the active covariance model parameters and the prior mean."

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getOptimizeParameters
"Accessor to the covariance models and prior mean optimization flag.

Returns
-------
optimizeParameters : bool
    Whether to optimize the active covariance model parameters and the prior mean."

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::setOptimizeVariational
"Accessor to the variational parameters optimization flag.

Parameters
----------
optimizeVariational : bool
    Whether to optimize the variational means and covariances."

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getOptimizeVariational
"Accessor to the variational parameters optimization flag.

Returns
-------
optimizeVariational : bool
    Whether to optimize the variational means and covariances."

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::setMu0
"Accessor to the prior mean of the log-variance process.

Parameters
----------
mu0 : float
    The prior mean level :math:`\mu_0` of the log-variance process."

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getMu0
"Accessor to the prior mean of the log-variance process.

Returns
-------
mu0 : float
    The prior mean level :math:`\mu_0` of the log-variance process."

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::setInducingPointsF
R"RAW(Accessor to the inducing points of the mean process.

Parameters
----------
inducingPointsF : :class:`~openturns.Sample`
    The inducing points of the mean process.
    The number of inducing points should not exceed the number of observations.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getInducingPointsF
R"RAW(Accessor to the inducing points of the mean process.

Returns
-------
inducingPointsF : :class:`~openturns.Sample`
    The inducing points of the mean process.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::setInducingPointsG
R"RAW(Accessor to the inducing points of the log-variance process.

Parameters
----------
inducingPointsG : :class:`~openturns.Sample`
    The inducing points of the log-variance process.
    The number of inducing points should not exceed the number of observations.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getInducingPointsG
R"RAW(Accessor to the inducing points of the log-variance process.

Returns
-------
inducingPointsG : :class:`~openturns.Sample`
    The inducing points of the log-variance process.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getUniqueInputSample
R"RAW(Get the unique input sites (replicate detection).

Returns
-------
uniqueInputSample : :class:`~openturns.Sample`
    The input sites with exact duplicates merged, in increasing lexicographic order.
    All likelihood computations run on these sites with per-site sufficient statistics,
    which is exact and reduces the data-term cost from the number of observations to
    the number of unique sites.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getReplicateCounts
R"RAW(Get the number of replicates per unique input site.

Returns
-------
counts : :class:`~openturns.Indices`
    The number of observations at each site of :meth:`getUniqueInputSample`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getReplicateMeanOutput
R"RAW(Get the mean output per unique input site.

Returns
-------
means : :class:`~openturns.Sample`
    The mean of the observations at each site of :meth:`getUniqueInputSample`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getCovarianceModelF
R"RAW(Get the covariance model of the mean process.

Returns
-------
covarianceModelF : :class:`~openturns.CovarianceModel`
    The covariance model of the latent mean process, with the parameters set to their
    optimized values.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getCovarianceModelG
R"RAW(Get the covariance model of the log-variance process.

Returns
-------
covarianceModelG : :class:`~openturns.CovarianceModel`
    The covariance model of the log-variance process, with the parameters set to their
    optimized values.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getReducedCovarianceModelF
R"RAW(Get the reduced covariance model of the mean process.

Returns
-------
covarianceModelF : :class:`~openturns.CovarianceModel`
    The covariance model of the latent mean process restricted to its active parameters.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getReducedCovarianceModelG
R"RAW(Get the reduced covariance model of the log-variance process.

Returns
-------
covarianceModelG : :class:`~openturns.CovarianceModel`
    The covariance model of the log-variance process restricted to its active parameters.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getVariationalMeanF
R"RAW(Get the variational posterior mean of the mean process.

Returns
-------
meanF : :class:`~openturns.Point`
    The mean :math:`\vect{m}_f` of the whitened variational posterior of the mean process.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getVariationalCovarianceF
R"RAW(Get the variational posterior covariance of the mean process.

Returns
-------
covarianceF : :class:`~openturns.CovarianceMatrix`
    The covariance :math:`\mat{S}_f` of the whitened variational posterior of the mean process.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getVariationalMeanG
R"RAW(Get the variational posterior mean of the log-variance process.

Returns
-------
meanG : :class:`~openturns.Point`
    The mean :math:`\vect{m}_g` of the whitened variational posterior of the log-variance process.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitter::getVariationalCovarianceG
R"RAW(Get the variational posterior covariance of the log-variance process.

Returns
-------
covarianceG : :class:`~openturns.CovarianceMatrix`
    The covariance :math:`\mat{S}_g` of the whitened variational posterior of the log-variance process.)RAW"
