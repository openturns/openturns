%feature("docstring") OT::GaussianProcessRegression
R"RAW(Gaussian process regression algorithm.

Available constructors:
    GaussianProcessRegression(*gprFitterResult*)

    GaussianProcessRegression(*inputSample, outputSample, covarianceModel, trendFunction*)

Parameters
----------
gprFitterResult : :class:`~openturns.GaussianProcessFitterResult`
    The result class built by :class:`~openturns.GaussianProcessFitter`.
inputSample, outputSample : 2-d sequence of float
    The samples :math:`(\vect{x}_k)_{1 \leq k \leq \sampleSize} \in \Rset^{\inputDim}` and :math:`(\vect{y}_k)_{1 \leq k \leq \sampleSize} \in \Rset^{\outputDim}` upon which the meta-model is built.
covarianceModel : :class:`~openturns.CovarianceModel`
    The covariance model used for the underlying Gaussian process approximation.
trendFunction : :class:`~openturns.Function`
    The trend function.

Notes
-----

Refer to :ref:`gaussian_process_regression` (step 2) to get all the notations and the theoretical aspects. We only detail here the notions related to the class.

We suppose we have a sample :math:`(\vect{x}_k, \vect{y}_k)_{1 \leq k \leq \sampleSize}` where :math:`\vect{y}_k = \model(\vect{x}_k)` for all *k*, with :math:`\model:\Rset^{\inputDim} \mapsto \Rset^{\outputDim}` the model.

The underlying Gaussian process approximation :math:`\vect{Y}` can be fulfilled with two different ways: 

* Within the first constructor: we suppose that the Gaussian process approximation has already been
  calibrated using the class :class:`~openturns.GaussianProcessFitter`;
* Within the second constructor: all the elements defining the Gaussian process approximation are specified
  separately: the data set, the covariance model and the trend function.

The objective of the *GaussianProcessRegression* is to condition the Gaussian process approximation :math:`\vect{Y}` to the data set: thus, we make the Gaussian process approximation become interpolating over the dataset.

In all cases, no estimation of the underlying Gaussian process approximation :math:`\vect{Y}` is performed. Refer to :class:`~openturns.GaussianProcessFitter` to get more details on the notation. The Gaussian process :math:`\vect{Y}` is defined by:

.. math::

    \vect{Y}(\omega, \vect{x}) = \vect{\mu}(\vect{x}) + \vect{W}(\omega, \vect{x})

where :math:`\vect{\mu} : \Rset^\inputDim \rightarrow \Rset^outputDim` is the trend function and :math:`\vect{W}` is a Gaussian process of dimension :math:`\outputDim` with zero mean and a specified covariance function.

The Gaussian Process Regression denoted by :math:`\vect{Z}` is the Gaussian process :math:`\vect{Y}` conditioned to the data
set:

.. math::

    \vect{Z}(\omega, \vect{x}) = \vect{Y}(\omega, \vect{x})\, | \,  \cC

where :math:`\cC` is the condition :math:`\vect{Y}(\omega, \vect{x}_k) = \vect{y}_k` for
:math:`1 \leq k \leq \sampleSize`.

Then, :math:`\vect{Z}` is a Gaussian process, which mean and covariance function are defined in :eq:`expectationGPR` and
:eq:`covarianceGPR_point`.

The Gaussian Process Regression metamodel :math:`\metaModel` is defined by the mean of :math:`\vect{Z}`. Its expression is
detailed in :eq:`GPRmetamodel`.

In order to get services related to the conditional covariance, use the class :class:`~openturns.GaussianProcessConditionalCovariance`.

Examples
--------
Create the model :math:`\model: \Rset \mapsto \Rset` and the samples:

>>> import openturns as ot
>>> g = ot.SymbolicFunction(['x'],  ['x * sin(x)'])
>>> sampleX = [[1.0], [2.0], [3.0], [4.0], [5.0], [6.0], [7.0], [8.0]]
>>> sampleY = g(sampleX)

Create the algorithm:

>>> basis = ot.Basis([ot.SymbolicFunction(['x'], ['x']), ot.SymbolicFunction(['x'], ['x^2'])])
>>> covarianceModel = ot.SquaredExponential([1.0])
>>> covarianceModel.setActiveParameter([])
>>> fit_algo = ot.GaussianProcessFitter(sampleX, sampleY, covarianceModel, basis)
>>> fit_algo.run()

Get the resulting interpolating metamodel :math:`\metaModel`:

>>> fit_result = fit_algo.getResult()
>>> algo = ot.GaussianProcessRegression(fit_result)
>>> algo.run()
>>> result = algo.getResult()
>>> metamodel = result.getMetaModel()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessRegression::getResult
"Get the results of the metamodel computation.

Returns
-------
result : :class:`~openturns.GaussianProcessRegressionResult`
    Structure containing all the results obtained, created by the method :py:meth:`run`.
"
// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessRegression::run
"Compute the response surface.

Notes
-----
It computes the Gaussian Process interpolating approximation and creates a
:class:`~openturns.GaussianProcessRegressionResult` structure containing all the results."

