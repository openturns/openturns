%feature("docstring") OT::GaussianProcessRegressionResult
R"RAW(Gaussian process regression (aka kriging) result.

Refer to :ref:`gaussian_process_regression`.

Parameters
----------
gpfitResult : :class:`~openturns.GaussianProcessFitterResult`
    Structure result of a gaussian process fitter.
covarianceCoefficients : 2-d sequence of float
    The :math:`\vect{\gamma}` defined in :eq:`gammaDefinition`.

Notes
-----
The structure is usually created by the method :py:meth:`~openturns.GaussianProcessRegression.run`, and obtained thanks to the `getResult()` method.

Refer to the documentation of :class:`~openturns.GaussianProcessRegression` to get details on
the notations.

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
>>> fit_algo = ot.GaussianProcessFitter(sampleX, sampleY, covarianceModel, basis)
>>> fit_algo.run()

Create the interpolating Gaussian process approximation:

>>> algo = ot.GaussianProcessRegression(fit_algo.getResult())
>>> algo.run()

Get the resulting interpolating metamodel :math:`\metaModel`:

>>> result = algo.getResult()
>>> metaModel = result.getMetaModel()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessRegressionResult::getCovarianceCoefficients
R"RAW(Accessor to the covariance coefficients.

Returns
-------
covCoeff : :class:`~openturns.Sample`
    The :math:`\vect{\gamma}` defined in :eq:`gammaDefinition`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessRegressionResult::getBasis
R"RAW(Accessor to the collection of basis.

Returns
-------
basis : :class:`~openturns.Basis`
    Functional basis to estimate the trend: :math:`(\varphi_j)_{1 \leq j \leq b}: \Rset^\inputDim
    \rightarrow \Rset`.

Notes
-----
If the trend is not estimated, the basis is empty. The same basis is used for each marginal output.)RAW"


// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessRegressionResult::getCovarianceModel
R"RAW(Accessor to the covariance model.

Returns
-------
covModel : :class:`~openturns.CovarianceModel`
    The covariance model of the Gaussian process :math:`\vect{W}`.
)RAW"

// ---------------------------------------------------------------------
%feature("docstring") OT::GaussianProcessRegressionResult::getLinearAlgebraMethod
"Accessor to the linear algebra method used to fit.

Returns
-------
linAlgMethod : int
    The used linear algebra method to fit the model:

    - ot.GaussianProcessFitterResult.LAPACK or 0: using `LAPACK` to fit the model,

    - ot.GaussianProcessFitterResult.HMAT or 1: using `HMAT` to fit the model.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessRegressionResult::getMetaModel
R"RAW(Accessor to the metamodel.

Returns
-------
metaModel : :class:`~openturns.Function`
    The metamodel :math:`\metaModel: \Rset^{\inputDim} \rightarrow \Rset^{\outputDim}`, defined in
    :eq:`GPRmetamodel`.

Notes
-----
This metamodel is interpolating the data set.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessRegressionResult::getNoise
R"RAW(Accessor to the Gaussian process.

Returns
-------
process : :class:`~openturns.Process`
    The Gaussian process :math:`\vect{W}` its the optimized parameters.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessRegressionResult::getOptimalLogLikelihood
"Accessor to the optimal log-likelihood of the model.

Returns
-------
optimalLogLikelihood : float
    The value of the log-likelihood corresponding to the model.
"

// ---------------------------------------------------------------------    

%feature("docstring") OT::GaussianProcessRegressionResult::getRegressionMatrix
R"RAW(Accessor to the regression matrix.

Returns
-------
process : :class:`~openturns.Matrix`
    Returns the regression matrix.

Notes
-----
The regression matrix, e.g the evaluation of the basis function
upon the input design sample. It contains :math:`\sampleSize \times \outputDim` lines and as many column as
the size of the functional basis. The column :math:`k` is defined as:

.. math::

    \left(\varphi_k(\vect{x}_1), \dots,\varphi_k(\vect{x}_\sampleSize) \right).

)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessRegressionResult::getTrendCoefficients
R"RAW(Accessor to the trend coefficients.

Returns
-------
trendCoef : :class:`~openturns.Point`
    The trend coefficients vectors :math:`(\vect{\beta}^1, \dots, \vect{\beta}^{\outputDim})`

Notes
-----
As the same basis is used for each marginal output, each :math:`\vect{\beta}^\ell` vector is of dimension
:math:`b`, the size of the functional basis.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessRegressionResult::getGaussianProcessFitterResult
"Accessor to the fitting result.

Returns
-------
result : :class:`~openturns.GaussianProcessFitterResult`
    The fitting result.
"
