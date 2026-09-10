%feature("docstring") OT::SparseGaussianProcessRegression
R"RAW(Sparse Gaussian process regression.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class builds the metamodel associated to a sparse Gaussian process. Given the
optimized inducing inputs :math:`\mat{Z}`, the variational mean :math:`\vect{m}_w` and
the Cholesky factor :math:`\mat{L}_{uu}` of the inducing-point covariance matrix
:math:`\mat{K}_{uu} = k(\mat{Z}, \mat{Z})`, the mean of the prediction at a new point
:math:`\vect{x}` is:

.. math::

    \mu(\vect{x}) = \vect{a}(\vect{x})^t \vect{m}_w

where :math:`\vect{a}(\vect{x}) = \mat{L}_{uu}^{-1} \vect{k}(\mat{Z}, \vect{x})` and
:math:`\vect{k}(\mat{Z}, \vect{x}) = (k(\vect{z}_1, \vect{x}), \dots, k(\vect{z}_m, \vect{x}))^t`
is the cross-covariance vector between the inducing inputs and the prediction point.

The metamodel comes with its gradient and Hessian (the latter using covariance partial Hessians).

The conditional variance of the prediction at any point can be obtained thanks to the
:meth:`getConditionalVariance() <openturns.experimental.SparseGaussianProcessFitterResult.getConditionalVariance>`
method of the resulting
:class:`~openturns.experimental.SparseGaussianProcessFitterResult`.

See also
--------
openturns.experimental.SparseGaussianProcessFitter, openturns.GaussianProcessRegression

Parameters
----------
result : :class:`~openturns.experimental.SparseGaussianProcessFitterResult`
    A sparse Gaussian process fitter result.

    Or

inputSample, outputSample : :class:`~openturns.Sample` or 2d-array
    The samples :math:`(\vect{x}_k)_{1 \leq k \leq \sampleSize} \in \Rset^\inputDim` and
    :math:`(\vect{y}_k)_{1 \leq k \leq \sampleSize} \in \Rset`.

covarianceModel : :class:`~openturns.CovarianceModel`
    Covariance model of the Gaussian process.

inducingPoints : :class:`~openturns.Sample`
    The inducing points :math:`(\vect{z}_j)_{1 \leq j \leq m}`.

Examples
--------
Create the model :math:`\model: \Rset \mapsto \Rset` and the samples:

>>> import openturns as ot
>>> from openturns.experimental import SparseGaussianProcessRegression
>>> g = ot.SymbolicFunction(['x'], ['x + x * sin(x)'])
>>> inputSample = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0]])
>>> outputSample = g(inputSample)

Create the algorithm:

>>> covarianceModel = ot.SquaredExponential([1.0])
>>> covarianceModel.setActiveParameter([])
>>> inducingPoints = ot.Sample([[1.0], [3.0], [5.0]])
>>> algo = SparseGaussianProcessRegression(inputSample, outputSample, covarianceModel, inducingPoints)
>>> algo.run()

Get the metamodel and evaluate it:

>>> result = algo.getResult()
>>> metaModel = result.getMetaModel()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessRegression::getResult
"Get the results of the metamodel computation.

Returns
-------
result : :class:`~openturns.experimental.SparseGaussianProcessFitterResult`
    Structure containing all the results obtained after computation
    and created by the method :py:meth:`run`.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessRegression::getMethod
"Accessor to the linear algebra method.

Returns
-------
linAlgMethod : int
    The used linear algebra method to fit the model:

    - ot.experimental.SparseGaussianProcessFitterResult.LAPACK or 0: using `LAPACK` to fit the model,

    - ot.experimental.SparseGaussianProcessFitterResult.HMAT or 1: using `HMAT` to fit the model."

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessRegression::setMethod
"Set the linear algebra method.

Parameters
----------
linAlgMethod : int
    The used linear algebra method to fit the model:

    - ot.experimental.SparseGaussianProcessFitterResult.LAPACK or 0: using `LAPACK` to fit the model,

    - ot.experimental.SparseGaussianProcessFitterResult.HMAT or 1: using `HMAT` to fit the model."
