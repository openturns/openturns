%feature("docstring") OT::GaussianProcessFitterResult
R"RAW(Gaussian process fitter result.

Refer to :ref:`gaussian_process_regression`.

Parameters
----------
inputSample, outputSample : :class:`~openturns.Sample`
    The samples :math:`(\vect{x}_k)_{1 \leq k \leq \sampleSize} \in \Rset^{\inputDim}` and :math:`(\vect{y}_k)_{1 \leq k \leq \sampleSize}\in \Rset^{\outputDim}`.
metaModel : :class:`~openturns.Function`
    The metamodel: :math:`\metaModel`, defined in :eq:`metaModelGPF` by the trend function.
regressionMatrix : :class:`~openturns.Matrix`
    The regression matrix, e.g the evaluation of the basis functions
    upon the input design sample.
basis :  :class:`~openturns.Basis`
    Functional basis of size :math:`b` : :math:`(\varphi_j: \Rset^{\inputDim} \rightarrow \Rset)` for :math:`j \in [1, b]`.
    Its size should be equal to zero if the trend is not estimated.
trendCoef : sequence of float
    The trend coefficients vectors :math:`(\vect{\beta}^1, \dots, \vect{\beta}^{\outputDim})`.
covarianceModel : :class:`~openturns.CovarianceModel`
    The covariance model of the Gaussian process with its optimized parameters.
optimalLogLikelihood : float
    The maximum log-likelihood corresponding to the model.
linAlgMethod : int
    The used linear algebra method to fit the model:

    - ot.GaussianProcessFitterResult.LAPACK or 0: using `LAPACK` to fit the model,

    - ot.GaussianProcessFitterResult.HMAT or 1: using `HMAT` to fit the model.

Notes
-----
The structure is usually created by the method :py:meth:`~openturns.GaussianProcessFitter.run` of the class
:class:`~openturns.GaussianProcessFitter` and obtained with its method
:py:meth:`~openturns.GaussianProcessFitter.getResult()`.

Refer to :ref:`gaussian_process_regression` (Step 1) to get all the notations and the theoretical aspects. We only detail here the notions related to the class.

This class creates the metamodel
:math:`\metaModel: \Rset^{\inputDim} \rightarrow \Rset^{\outputDim}` defined by the estimated trend
function:

.. math::
    :label: metaModelGPF

    \metaModel(\vect{x}) = \vect{\mu}(\vect{x}) = \left(
      \begin{array}{l}
        \mu_1(\vect{x}) \\
        \vdots  \\
        \mu_\outputDim(\vect{x})
       \end{array}
     \right)

with :math:`\mu_\ell(\vect{x}) = \sum_{j=1}^{b} \beta_j^\ell \varphi_j(\vect{x})` and :math:`\varphi_j: \Rset^\inputDim \rightarrow \Rset` the trend functions for :math:`1 \leq j \leq b` and :math:`1 \leq \ell \leq \outputDim`.

Examples
--------
Create the model :math:`\model: \Rset \mapsto \Rset` and the samples:

>>> import openturns as ot
>>> g = ot.SymbolicFunction(['x'],  ['x * sin(x)'])
>>> sampleX = [[1.0], [2.0], [3.0], [4.0], [5.0], [6.0]]
>>> sampleY = g(sampleX)

Create the algorithm:

>>> basis = ot.Basis([ot.SymbolicFunction(['x'], ['x']), ot.SymbolicFunction(['x'], ['x^2'])])
>>> covarianceModel = ot.GeneralizedExponential([2.0], 2.0)
>>> algo = ot.GaussianProcessFitter(sampleX, sampleY, covarianceModel, basis)
>>> algo.run()

Get the result:

>>> result = algo.getResult()

Get the metamodel :math:`\metaModel(\vect{x}) = \vect{\mu}(\vect{x})`:

>>> metaModel = result.getMetaModel()
>>> graph = metaModel.draw(0.0, 7.0)
>>> cloud = ot.Cloud(sampleX, sampleY)
>>> cloud.setPointStyle('fcircle')
>>> graph = ot.Graph()
>>> graph.add(cloud)
>>> graph.add(g.draw(0.0, 7.0))
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitterResult::getBasis
R"RAW(Accessor to the basis.

Returns
-------
basis : :class:`~openturns.Basis`
    Functional basis to estimate the trend: :math:`(\varphi_j)_{1 \leq j \leq b}: \Rset^\inputDim \rightarrow \Rset`.

Notes
-----
If the trend is not estimated, the basis is empty. The same basis is used for each marginal output.)RAW"


// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitterResult::getCovarianceModel
R"RAW(Accessor to the covariance model.

Returns
-------
covModel : :class:`~openturns.CovarianceModel`
    The covariance model of the Gaussian process :math:`\vect{W}` with its optimized parameters.
)RAW"

// ---------------------------------------------------------------------
%feature("docstring") OT::GaussianProcessFitterResult::getLinearAlgebraMethod
"Accessor to the linear algebra method used to fit.

Returns
-------
linAlgMethod : int
    The used linear algebra method to fit the model:

    - ot.GaussianProcessFitterResult.LAPACK or 0: using `LAPACK` to fit the model,

    - ot.GaussianProcessFitterResult.HMAT or 1: using `HMAT` to fit the model.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitterResult::getMetaModel
R"RAW(Accessor to the trend function.

Returns
-------
metaModel : :class:`~openturns.Function`
    The metamodel :math:`\metaModel: \Rset^{\inputDim} \rightarrow \Rset^{\outputDim}`, defined by the estimated trend function.

Notes
-----
See :class:`~openturns.GaussianProcessFitter` to get the notations. This method returns the metamodel
:math:`\metaModel: \Rset^{\inputDim} \rightarrow \Rset^{\outputDim}` defined by the estimated trend
function in :eq:`metaModelGPF`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitterResult::getNoise
R"RAW(Accessor to the Gaussian process.

Returns
-------
process : :class:`~openturns.Process`
    The Gaussian process :math:`\vect{W}` its the optimized parameters.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitterResult::getOptimalLogLikelihood
"Accessor to the optimal log-likelihood of the model.

Returns
-------
optimalLogLikelihood : float
    The value of the log-likelihood corresponding to the model.
"

// ---------------------------------------------------------------------    

%feature("docstring") OT::GaussianProcessFitterResult::getRegressionMatrix
R"RAW(Accessor to the regression matrix.

Returns
-------
process : :class:`~openturns.Matrix`
    Returns the regression matrix.

Notes
-----
The regression matrix, e.g the evaluation of the basis functions
upon the input design sample. It contains :math:`\sampleSize` lines and as many column as
the size of the functional basis. The column :math:`k` is defined as:

.. math::

    \left(\varphi_k(\vect{x}_1), \dots,\varphi_k(\vect{x}_\sampleSize) \right).

)RAW"
   
// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitterResult::getTrendCoefficients
R"RAW(Accessor to the trend coefficients.

Returns
-------
trendCoef : :class:`~openturns.Point`
    The trend coefficients vectors :math:`(\vect{\beta}^1, \dots, \vect{\beta}^{\outputDim})` as a :class:`~openturns.Point`.

Notes
-----
As the same basis is used for each marginal output, each :math:`\vect{\beta}^\ell` vector is of dimension
:math:`b`, the size of the functional basis.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitterResult::getCholeskyFactor
R"RAW(Accessor to the Cholesky factor of the covariance matrix.

Returns
-------
L : :class:`~openturns.TriangularMatrix`
    Cholesky factor :math:`\mat{L}_{\vect{p}^*}` of the covariance matrix, which can thus be written :math:`\mat{L}_{\vect{p}^*} \mat{L}_{\vect{p}^*}^T`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessFitterResult::getStandardizedOutput
R"RAW(Accessor to the standardized output.

Returns
-------
rho : :class:`~openturns.Point`
    Standardized output vector.

Notes
-----
The standardized output vector is defined as :math:`\mat{L}_{\vect{p}^*}^{-1}(\vect{y} - \vect{m}_{\vect{\beta}^*(\vect{p}^*)})`.)RAW"
