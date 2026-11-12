%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult
R"RAW(Structure which contains the results of the heteroscedastic sparse Gaussian process fitting.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This structure stores the results of the variational inference of a heteroscedastic
sparse Gaussian process: the inducing points of the mean and log-variance processes,
the prior mean of the log-variance process, the optimal ELBO value as well as the
by-products of the uncollapsed variational bound, namely the whitening factors and the
variational posterior means and covariances in the whitened parametrisation.
Refer to :class:`~openturns.experimental.HeteroscedasticSparseGaussianProcessFitter`
for the mathematical details.

The conditional variance of the latent mean process at a new point
:math:`\vect{x}` can be obtained thanks to the :meth:`getConditionalVariance` method,
and the predictive variance including the heteroscedastic noise thanks to the
:meth:`getPredictiveVariance` method.

See also
--------
openturns.experimental.HeteroscedasticSparseGaussianProcessFitter, openturns.experimental.HeteroscedasticSparseGaussianProcessRegression

Examples
--------
>>> import openturns as ot
>>> from openturns.experimental import HeteroscedasticSparseGaussianProcessFitter
>>> g = ot.SymbolicFunction(['x'], ['x + x * sin(x)'])
>>> inputSample = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0]])
>>> outputSample = g(inputSample)
>>> covarianceModelF = ot.SquaredExponential([1.0])
>>> covarianceModelF.setActiveParameter([0])
>>> covarianceModelG = ot.SquaredExponential([1.0])
>>> covarianceModelG.setActiveParameter([0])
>>> algo = HeteroscedasticSparseGaussianProcessFitter(inputSample, outputSample, covarianceModelF, covarianceModelG, inputSample[0:3], inputSample[0:3])
>>> algo.run()
>>> result = algo.getResult()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult::getCovarianceModelF
R"RAW(Get the covariance model of the mean process.

Returns
-------
covarianceModelF : :class:`~openturns.CovarianceModel`
    The covariance model of the latent mean process, with the parameters set to their
    optimized values.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult::getCovarianceModelG
R"RAW(Get the covariance model of the log-variance process.

Returns
-------
covarianceModelG : :class:`~openturns.CovarianceModel`
    The covariance model of the log-variance process, with the parameters set to their
    optimized values.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult::getInducingPointsF
R"RAW(Get the inducing points of the mean process.

Returns
-------
inducingPointsF : :class:`~openturns.Sample`
    The inducing points of the mean process.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult::getInducingPointsG
R"RAW(Get the inducing points of the log-variance process.

Returns
-------
inducingPointsG : :class:`~openturns.Sample`
    The inducing points of the log-variance process.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult::getWhiteningFactorF
R"RAW(Get the whitening factor of the mean process.

Returns
-------
whiteningFactorF : :class:`~openturns.TriangularMatrix`
    The lower Cholesky factor of the covariance matrix of the mean process
    discretized on the inducing points.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult::getWhiteningFactorG
R"RAW(Get the whitening factor of the log-variance process.

Returns
-------
whiteningFactorG : :class:`~openturns.TriangularMatrix`
    The lower Cholesky factor of the covariance matrix of the log-variance process
    discretized on the inducing points.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult::getPosteriorMeanF
R"RAW(Get the variational posterior mean of the mean process.

Returns
-------
posteriorMeanF : :class:`~openturns.Point`
    The mean of the whitened variational posterior of the mean process.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult::getPosteriorCovarianceF
R"RAW(Get the variational posterior covariance of the mean process.

Returns
-------
posteriorCovarianceF : :class:`~openturns.CovarianceMatrix`
    The covariance of the whitened variational posterior of the mean process.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult::getPosteriorMeanG
R"RAW(Get the variational posterior mean of the log-variance process.

Returns
-------
posteriorMeanG : :class:`~openturns.Point`
    The mean of the whitened variational posterior of the log-variance process.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult::getPosteriorCovarianceG
R"RAW(Get the variational posterior covariance of the log-variance process.

Returns
-------
posteriorCovarianceG : :class:`~openturns.CovarianceMatrix`
    The covariance of the whitened variational posterior of the log-variance process.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult::getMu0
R"RAW(Get the prior mean of the log-variance process.

Returns
-------
mu0 : float
    The prior mean level of the log-variance process.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult::getOptimalELBO
R"RAW(Get the optimal ELBO value.

Returns
-------
elbo : float
    The optimal value of the uncollapsed variational bound (ELBO).)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult::getConditionalVariance
R"RAW(Get the conditional variance of the latent mean process prediction.

Parameters
----------
x : sequence of float, or :class:`~openturns.Sample`
    The point(s) where to compute the conditional variance.

Returns
-------
variance : float, or :class:`~openturns.Point`
    The conditional variance of the latent mean process at the given point(s).)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult::getPredictiveVariance
R"RAW(Get the predictive variance of the heteroscedastic sparse Gaussian process.

Parameters
----------
x : sequence of float, or :class:`~openturns.Sample`
    The point(s) where to compute the predictive variance.

Returns
-------
variance : float, or :class:`~openturns.Point`
    The predictive variance at the given point(s): the conditional variance of the
    latent mean process plus the lognormal noise moment
    :math:`\exp(\mu_{g} + v_{g} / 2)` of the log-variance process.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult::computeALM
R"RAW(Active Learning MacKay criterion for sequential design.

Parameters
----------
candidateSample : :class:`~openturns.Sample`
    The candidate points where to evaluate the predictive variance.

Returns
-------
index : int
    The index of the candidate point with the largest predictive variance,
    namely the point bringing the most information when added to the design.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessFitterResult::computeIMSPE
R"RAW(Integrated mean-squared prediction error over a reference sample.

Parameters
----------
referenceSample : :class:`~openturns.Sample`
    The reference points approximating the input distribution.

Returns
-------
imspe : float
    The mean predictive variance over the reference sample, namely the current
    overall prediction error level. It summarizes the fitted model; selecting a
    new observation requires a candidate-conditioned criterion, which this
    average does not provide.)RAW"
