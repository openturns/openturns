%feature("docstring") OT::SparseGaussianProcessFitterResult
R"RAW(Structure which contains the results of the sparse Gaussian process fitting.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This structure stores the results of the variational inference of a sparse Gaussian process:
the inducing points, the (fixed) noise standard deviation, the optimal ELBO value as well as the
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
    The noise standard deviation :math:`\sigma` of the sparse Gaussian process.)RAW"

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
    The conditional variance :math:`\Var(Y(\vect{x}) \mid \vect{y})` at the given point(s).)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::getLinearAlgebraMethod
"Accessor to the linear algebra method used to fit.

Returns
-------
linAlgMethod : int
    The used linear algebra method to fit the model:

    - ot.SparseGaussianProcessFitterResult.LAPACK or 0: using `LAPACK` to fit the model,

    - ot.SparseGaussianProcessFitterResult.HMAT or 1: using `HMAT` to fit the model."

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::getWhiteningFactorHMatrix
"Accessor to the whitening factor in HMatrix form.

Returns
-------
whiteningFactor : :class:`~openturns.HMatrix`
    The Cholesky factor of the inducing points covariance matrix, when the
    `HMAT` linear algebra method is used."

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessFitterResult::setWhiteningFactorHMatrix
"Accessor to the whitening factor in HMatrix form.

Parameters
----------
whiteningFactor : :class:`~openturns.HMatrix`
    The Cholesky factor of the inducing points covariance matrix, when the
    `HMAT` linear algebra method is used."
