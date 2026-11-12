%feature("docstring") OT::SparseGaussianProcessHessian
R"RAW(Sparse Gaussian process hessian.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Refer to :any:`sparse_gaussian_process_regression` for the notations.

This class computes the hessian of the mean of the sparse Gaussian process prediction with
respect to the input point :math:`\vect{x}`:

.. math::

    \frac{\partial^2 \mu}{\partial x_j \partial x_k}(\vect{x}) = \sum_{m=1}^{M}
    \alpha_m \frac{\partial^2 k(\vect{z}_m, \vect{x})}{\partial x_j \partial x_k}

where :math:`\vect{\alpha} = \mat{L}_{uu}^{-t} \vect{m}_w`.

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

whiteningFactorHMatrix : :class:`~openturns.HMatrix`
    The Cholesky factor in ``HMAT`` form, used when the linear algebra method is ``HMAT``.

linearAlgebraMethod : int
    The linear algebra method, either ``LAPACK`` or ``HMAT``.

See also
--------
openturns.experimental.SparseGaussianProcessRegression, openturns.experimental.SparseGaussianProcessEvaluation, openturns.experimental.SparseGaussianProcessGradient

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
>>> metaModel = algo.getResult().getMetaModel()
>>> h = metaModel.hessian([1.5])
)RAW"
