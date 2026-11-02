%feature("docstring") OT::SparseGaussianProcessEvaluation
R"RAW(Sparse Gaussian process evaluation.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Refer to :any:`sparse_gaussian_process_regression` for the notations.

This class evaluates the mean of the sparse Gaussian process prediction at a given point
:math:`\vect{x} \in \Rset^{\inputDim}`:

.. math::

    \mu(\vect{x}) = \vect{a}(\vect{x})^t \vect{m}_w

where :math:`\vect{a}(\vect{x}) = \mat{L}_{uu}^{-1} \vect{k}(\vect{z}, \vect{x})`,
:math:`\mat{L}_{uu}` is the Cholesky factor of the covariance matrix discretized on the
inducing points and :math:`\vect{m}_w` is the whitened variational posterior mean.

The conditional variance can be obtained thanks to the :meth:`getConditionalVariance`
method.

Parameters
----------
covarianceModel : :class:`~openturns.CovarianceModel`
    Covariance model of the sparse Gaussian process.

inducingPoints : :class:`~openturns.Sample`
    The inducing points :math:`(\vect{z}_j)_{1 \leq j \leq m}`.

whiteningFactor : :class:`~openturns.TriangularMatrix`
    The lower Cholesky factor of the covariance matrix discretized on the inducing points.

posteriorMean : :class:`~openturns.Point`
    The mean of the whitened variational posterior distribution.

posteriorCovariance : :class:`~openturns.CovarianceMatrix`
    The covariance of the whitened variational posterior distribution.

whiteningFactorHMatrix : :class:`~openturns.HMatrix`
    The Cholesky factor in ``HMAT`` form, used when the linear algebra method is ``HMAT``.

linearAlgebraMethod : int
    The linear algebra method, either ``LAPACK`` or ``HMAT``.

See also
--------
openturns.experimental.SparseGaussianProcessRegression, openturns.experimental.SparseGaussianProcessFitter

Examples
--------
>>> import openturns as ot
>>> from openturns.experimental import SparseGaussianProcessRegression
>>> g = ot.SymbolicFunction(['x'], ['x + x * sin(x)'])
>>> inputSample = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0]])
>>> outputSample = g(inputSample)
>>> covarianceModel = ot.SquaredExponential([1.0])
>>> covarianceModel.setActiveParameter([])
>>> inducingPoints = ot.Sample([[1.0], [3.0], [5.0]])
>>> algo = SparseGaussianProcessRegression(inputSample, outputSample, covarianceModel, inducingPoints)
>>> algo.run()
>>> result = algo.getResult()
>>> metaModel = result.getMetaModel()
>>> print(metaModel([1.5]))
[0.036...]
>>> print(result.getConditionalVariance([1.5]))
0.9877...
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SparseGaussianProcessEvaluation::getConditionalVariance
R"RAW(Get the conditional variance of the sparse Gaussian process prediction.

Parameters
----------
x : sequence of float, or :class:`~openturns.Sample`
    The point(s) where to compute the conditional variance.

Returns
-------
variance : float, or :class:`~openturns.Point`
    The conditional variance :math:`\Var(Y(\vect{x}) \mid \vect{y})` at the given point(s).)RAW"
