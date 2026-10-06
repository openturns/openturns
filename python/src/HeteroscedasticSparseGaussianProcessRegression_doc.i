%feature("docstring") OT::HeteroscedasticSparseGaussianProcessRegression
R"RAW(Build heteroscedastic sparse Gaussian process regression.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Refer to :class:`~openturns.experimental.HeteroscedasticSparseGaussianProcessFitter`
for the mathematical details. The metamodel predicts the posterior mean of the latent
mean process; use :meth:`getResult` then
:meth:`~openturns.experimental.HeteroscedasticSparseGaussianProcessFitterResult.getPredictiveVariance`
for prediction intervals including the heteroscedastic noise.

Parameters
----------
inputSample, outputSample : :class:`~openturns.Sample` or 2d-array
    The input and output samples (parameters constructor only).

covarianceModelF : :class:`~openturns.CovarianceModel`
    Covariance model of the latent mean process (parameters constructor only).

covarianceModelG : :class:`~openturns.CovarianceModel`
    Covariance model of the log-variance process (parameters constructor only).

inducingPointsF : :class:`~openturns.Sample`
    The inducing points of the mean process (parameters constructor only).

inducingPointsG : :class:`~openturns.Sample`
    The inducing points of the log-variance process (parameters constructor only).

result : :class:`~openturns.experimental.HeteroscedasticSparseGaussianProcessFitterResult`
    The fitter result (result constructor only).

See also
--------
openturns.experimental.HeteroscedasticSparseGaussianProcessFitter, openturns.experimental.HeteroscedasticSparseGaussianProcessFitterResult

Examples
--------
>>> import openturns as ot
>>> from openturns.experimental import HeteroscedasticSparseGaussianProcessRegression
>>> g = ot.SymbolicFunction(['x'], ['x + x * sin(x)'])
>>> inputSample = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0]])
>>> outputSample = g(inputSample)
>>> covarianceModelF = ot.SquaredExponential([1.0])
>>> covarianceModelF.setActiveParameter([0])
>>> covarianceModelG = ot.SquaredExponential([1.0])
>>> covarianceModelG.setActiveParameter([0])
>>> algo = HeteroscedasticSparseGaussianProcessRegression(inputSample, outputSample, covarianceModelF, covarianceModelG, inputSample[0:3], inputSample[0:3])
>>> algo.run()
>>> result = algo.getResult()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessRegression::run
"Compute the response surface.

Notes
-----
It computes the response surface and creates a
:class:`~openturns.experimental.HeteroscedasticSparseGaussianProcessFitterResult` structure containing all the results."

// ---------------------------------------------------------------------

%feature("docstring") OT::HeteroscedasticSparseGaussianProcessRegression::getResult
"Get the results of the metamodel computation.

Returns
-------
result : :class:`~openturns.experimental.HeteroscedasticSparseGaussianProcessFitterResult`
    Structure containing all the results obtained after computation
    and created by the method :py:meth:`run`.
"
