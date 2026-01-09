%feature("docstring") OT::GaussianProcessConditionalCovariance
R"RAW(Conditional covariance post processing of a Gaussian Process Regression result.

Parameters
----------
gprResult :  :class:`~openturns.GaussianProcessRegressionResult`
    The result class built by :class:`~openturns.GaussianProcessRegression`.

Notes
-----
Refer to :ref:`gaussian_process_regression` (step 3) to get all the notations and the theoretical aspects. We only detail here the notions related to the class.

We suppose we have a sample :math:`(\vect{x}_k, \vect{y}_k)_{1 \leq k \leq \sampleSize}` where :math:`\vect{y}_k = \model(\vect{x}_k)` for all *k*, with :math:`\model:\Rset^{\inputDim} \mapsto \Rset^{\outputDim}` the model. The  Gaussian process approximation :math:`\vect{Y}` is defined by:

.. math::

    \vect{Y}(\omega, \vect{x}) = \vect{\mu}(\vect{x}) + \vect{W}(\omega, \vect{x})

where :math:`\vect{\mu} : \Rset^\inputDim \rightarrow \Rset^outputDim` is the trend function and :math:`\vect{W}` is a Gaussian process of dimension :math:`\outputDim` with zero mean and a specified covariance function. The Gaussian process regression denoted by :math:`\vect{Z}` is defined by:

.. math::

    \vect{Z}(\omega, \vect{x}) = \vect{Y}(\omega, \vect{x})\, | \,  \cC

where :math:`\cC` is the condition :math:`\vect{Y}(\omega, \vect{x}_k) = \vect{y}_k` for
:math:`1 \leq k \leq \sampleSize`.

The class provides services related to the conditional covariance of the Gaussian process regression :math:`\vect{Z}`.

Examples
--------
Create the model :math:`g: \Rset \mapsto \Rset` and the samples:

>>> import openturns as ot
>>> trend = ot.SymbolicFunction(['x'],  ['1'])
>>> sampleX = [[1.0], [2.0], [3.0], [4.0], [5.0], [6.0]]
>>> sampleY = trend(sampleX)

Create the algorithm:

>>> covarianceModel = ot.SquaredExponential([1.0])
>>> covarianceModel.setActiveParameter([])

>>> algo = ot.GaussianProcessRegression(sampleX, sampleY, covarianceModel, trend)
>>> algo.run()
>>> result = algo.getResult()
>>> condCov = ot.GaussianProcessConditionalCovariance(result)
>>> c = condCov([1.1])
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessConditionalCovariance::getConditionalMean
R"RAW(Compute the conditional mean of the Gaussian process on a point or a sample of points.

Parameters
----------
x : sequence of float
    The point :math:`\vect{x}` where the conditional mean of the output has to be evaluated.
sampleX : 2-d sequence of float
     The sample :math:`(\vect{\xi}_1, \dots, \vect{\xi}_N)` where the conditional mean of the output has to be evaluated (*N* can be equal to 1).

Returns
-------
condMean : :class:`~openturns.Point`
    The conditional mean the Gaussian process regression :math:`\vect{Z}` defined in :eq:`GPRdef` at point :math:`\vect{x}` or on the sample :math:`(\vect{\xi}_1, \dots, \vect{\xi}_N)`:

    .. math::

        \left(
          \begin{array}{l}
            \Expect{\vect{Z}(\omega, \vect{\xi}_1)}\\
            \dots  \\
            \Expect{\vect{Z}(\omega, \vect{\xi}_N)}
          \end{array}
        \right)

    This vector is in :math:`\Rset^{N \times \outputDim}`.
)RAW"


// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessConditionalCovariance::getConditionalCovariance
R"RAW(Compute the conditional covariance of the Gaussian process on a point (or several points).

Parameters
----------
x : sequence of float
    The point :math:`\vect{x}` where the conditional covariance of the output has to be evaluated.
sampleX : 2-d sequence of float
     The sample :math:`(\vect{\xi}_1, \dots, \vect{\xi}_N)` where the conditional covariance of the output has to be evaluated (*N* can be equal to 1).

Returns
-------
condCov : :class:`~openturns.CovarianceMatrix`
    The conditional covariance of the Gaussian process regression  :math:`\vect{Z}` defined in :eq:`GPRdef` at point :math:`\vect{x}` is defined in :eq:`covarianceGPR_point`. When computed on the sample :math:`(\vect{\xi}_1, \dots, \vect{\xi}_N)`, the covariance matrix is defined in :eq:`covarianceGPR_sample`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessConditionalCovariance::getDiagonalCovariance
R"RAW(Compute the diagonal conditional covariance of the Gaussian process on a point.

Parameters
----------
x : sequence of float
    The point :math:`\vect{x}` where the conditional marginal covariance of the output has to be evaluated.

Returns
-------
condCov : :class:`~openturns.CovarianceMatrix`
    The conditional covariance :math:`\Cov{\vect{Y}(\omega, \vect{x})\, | \,  \cC}` at point :math:`\vect{x}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessConditionalCovariance::getDiagonalCovarianceCollection
R"RAW(Compute the conditional covariance of the Gaussian process on a sample.

Parameters
----------
sampleX : 2-d sequence of float
     The sample :math:`(\vect{\xi}_1, \dots, \vect{\xi}_N)` where the conditional marginal covariance of the output has to be evaluated (*N* can be equal to 1).

Returns
-------
condCov : :class:`~openturns.CovarianceMatrixCollection`
    The collection of conditional covariance matrices :math:`\Cov{\vect{Z}(\omega, \vect{\xi}_i)}
    = \mat{\Sigma_{ii}}` for :math:`1 \leq i \leq N` defined in :eq:`covarianceGPR_sample`.

Notes
-----
Each element of the collection corresponds to the conditional covariance with respect to the input learning set
(e.g. a pointwise evaluation of the `getDiagonalCovariance`). The returned collection is
of size :math:`N` and contains matrices in :math:`\cM_{\outputDim, \outputDim}(\Rset)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessConditionalCovariance::getConditionalMarginalVariance
R"RAW(Compute the conditional variance of the Gaussian process on a point (or several points).

Parameters
----------
x : sequence of float
    The point :math:`\vect{x}` where the conditional variance of the output has to be evaluated.
sampleX : 2-d sequence of float
     The sample :math:`(\vect{\xi}_1, \dots, \vect{\xi}_N)` where the conditional variance of the output has to be evaluated (*N* can be equal to 1).
marginalIndex : int
    Marginal of interest (for multiple outputs).
    
    Default value is 0 (first component).
marginalIndices : sequence of int
    Marginals of interest (for multiple outputs).

Returns
-------
var : float
      The variance of the specified marginal of the Gaussian process regression  :math:`\vect{Z}`
      defined in :eq:`GPRdef` at the specified point.

varPoint : sequence of float
    The marginal variances of each marginal of interest computed at each given point.


Notes
-----
If only one  marginal :math:`k`  of interest  and one point  :math:`\vect{x}` have been specified,
the method returns :math:`\Var{Z_k(\omega, \vect{x})}` where :math:`\vect{Z}` is the Gaussian
process regression  defined in :eq:`GPRdef`.

If several marginal of interest :math:`(k_1, \dots, k_M)` or several points :math:`(\vect{\xi}_1,
\dots, \vect{\xi}_N)` have been specified, the method returns the concatenation of
sequence of variances :math:`(\Var{Z_{k_1}(\omega, \vect{\xi}_j)}, \dots, \Var{Z_{k_M}(\omega, \vect{\xi}_j)})` for each :math:`1 \leq j \leq N`.)RAW"

