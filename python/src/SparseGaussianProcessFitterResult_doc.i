%feature("docstring") OT::SparseGaussianProcessFitterResult
R"RAW(Structure which contains the results of the sparse Gaussian process fitting.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This structure stores the results of the variational inference of a sparse Gaussian process:
the inducing points, the noise standard deviation (or the fixed per-observation noise
variances, or the log-variance function when a parametric heteroscedastic likelihood is
used), the optimal ELBO value as well as the
by-products of the collapsed variational bound, namely the whitening factor, the variational
posterior mean and the variational posterior covariance in the whitened parametrisation.
Refer to :class:`~openturns.experimental.SparseGaussianProcessFitter` for the mathematical details.

The conditional variance of the sparse Gaussian process prediction at a new point
:math:`\vect{x}` can be obtained thanks to the :meth:`getConditionalVariance` method.

See also
--------
openturns.experimental.SparseGaussianProcessFitter, openturns.experimental.SparseGaussianProcessRegression

Examples
--------
>>> import openturns as ot
>>> from openturns.experimental import SparseGaussianProcessFitter
>>> g = ot.SymbolicFunction(['x'], ['x + x * sin(x)'])
>>> inputSample = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0]])
>>> outputSample = g(inputSample)
>>> covarianceModel = ot.SquaredExponential([1.0])
>>> covarianceModel.setActiveParameter([0])
>>> algo = SparseGaussianProcessFitter(inputSample, outputSample, covarianceModel, 3)
>>> algo.run()
>>> result = algo.getResult()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::getCovarianceModel
R"RAW(Get the covariance model.

Returns
-------
covarianceModel : :class:`~openturns.CovarianceModel`
    The covariance model of the sparse Gaussian process, with the parameters set to their
    optimized values.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::getInducingPoints
R"RAW(Get the inducing points.

Returns
-------
inducingPoints : :class:`~openturns.Sample`
    The inducing points :math:`(\vect{z}_j)_{1 \leq j \leq m}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::getWhiteningFactor
R"RAW(Get the whitening factor.

Returns
-------
whiteningFactor : :class:`~openturns.TriangularMatrix`
    The lower Cholesky factor :math:`\mat{L}_{uu}` of the covariance matrix
    discretized on the inducing points.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::getPosteriorMean
R"RAW(Get the variational posterior mean.

Returns
-------
posteriorMean : :class:`~openturns.Point`
    The mean :math:`\vect{m}_w` of the whitened variational posterior distribution.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::getPosteriorCovariance
R"RAW(Get the variational posterior covariance.

Returns
-------
posteriorCovariance : :class:`~openturns.CovarianceMatrix`
    The covariance :math:`\mat{S}_{ww}` of the whitened variational posterior distribution.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::getNoiseStdDev
R"RAW(Get the noise standard deviation.

Returns
-------
noiseStdDev : float
    The noise standard deviation :math:`\sigma` of the sparse Gaussian process.
    When fixed per-observation noise variances are used (see :meth:`getNoiseVariances`),
    they take precedence over this scalar value.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::getNoiseVariances
R"RAW(Get the fixed per-observation noise variances.

Returns
-------
noiseVariances : :class:`~openturns.Point`
    The fixed noise variances :math:`(\sigma^2_1, \dots, \sigma^2_n)`, or an empty
    point when the homoscedastic likelihood or a log-variance function is used.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::getVarianceFunction
R"RAW(Get the log-variance function.

Returns
-------
varianceFunction : :class:`~openturns.Function`
    The log-variance function :math:`g`, such that the noise variance at
    :math:`\vect{x}` is :math:`\sigma^2(\vect{x}) = \exp(g(\vect{x}))`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::setVarianceFunction
R"RAW(Set the log-variance function.

Parameters
----------
varianceFunction : :class:`~openturns.Function`
    The log-variance function :math:`g: \Rset^\inputDim \mapsto \Rset`, such that the
    noise variance at :math:`\vect{x}` is :math:`\sigma^2(\vect{x}) = \exp(g(\vect{x}))`.
    The function must have output dimension 1.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::getPredictiveVariance
R"RAW(Get the predictive variance of the sparse Gaussian process.

Parameters
----------
x : sequence of float, or :class:`~openturns.Sample`
    The point(s) where to compute the predictive variance.

Returns
-------
variance : float, or :class:`~openturns.Point`
    The predictive variance :math:`\Var(Y(\vect{x}) \mid \vect{y})` of the observed output at the given point(s):
    the conditional variance of the latent process plus the noise variance, namely the
    scalar noise variance for the homoscedastic likelihood and :math:`\exp(g(\vect{x}))`
    for the log-variance function likelihood. It is undefined away from the training
    data with fixed per-observation noise variances.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::setNoiseVariances
R"RAW(Set the fixed per-observation noise variances.

Parameters
----------
noiseVariances : sequence of float
    The fixed noise variances :math:`(\sigma^2_1, \dots, \sigma^2_n)`, one positive
    value per observation. An empty sequence restores the homoscedastic likelihood.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::getOptimalELBO
R"RAW(Get the optimal ELBO value.

Returns
-------
elbo : float
    The optimal value of the collapsed variational bound (ELBO).)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::getConditionalVariance
R"RAW(Get the conditional variance of the sparse Gaussian process prediction.

Parameters
----------
x : sequence of float, or :class:`~openturns.Sample`
    The point(s) where to compute the conditional variance.

Returns
-------
variance : float, or :class:`~openturns.Point`
    The conditional variance :math:`\Var(f(\vect{x}) \mid \vect{y})` of the latent process at the given point(s).)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::getLinearAlgebraMethod
"Accessor to the linear algebra method used to fit.

Returns
-------
linAlgMethod : int
    The used linear algebra method to fit the model:

    - ot.experimental.SparseGaussianProcessFitterResult.LAPACK or 0: using ``LAPACK`` to fit the model,

    - ot.experimental.SparseGaussianProcessFitterResult.HMAT or 1: using ``HMAT`` to fit the model."

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::getWhiteningFactorHMatrix
"Accessor to the whitening factor in HMatrix form.

Returns
-------
whiteningFactor : :class:`~openturns.HMatrix`
    The Cholesky factor of the inducing points covariance matrix, when the
    ``HMAT`` linear algebra method is used."

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::setWhiteningFactorHMatrix
"Accessor to the whitening factor in HMatrix form.

Parameters
----------
whiteningFactor : :class:`~openturns.HMatrix`
    The Cholesky factor of the inducing points covariance matrix, when the
    ``HMAT`` linear algebra method is used."
