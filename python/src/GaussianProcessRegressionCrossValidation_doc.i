%feature("docstring") OT::GaussianProcessRegressionCrossValidation
"Validate a Gaussian Process Regression surrogate model.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Parameters
----------
result : :class:`~openturns.GaussianProcessRegressionResult`
    A Gaussian Process Regression result.

splitter : :class:`~openturns.SplitterImplementation`, optional
    The cross-validation method.
    For now, only the default :class:`~openturns.LeaveOneOutSplitter` can be used.

See also
--------
openturns.GaussianProcessRegression, openturns.GaussianProcessRegressionResult

Notes
-----
A `GaussianProcessRegressionCrossValidation` object is used for the validation of a Gaussian Process Regression.
It is based on the fast (analytical) leave-one-out
cross-validation method presented in [ginsbourger2025]_ (Equation 23).

Note that this method relies on linear algebra, and therefore uses the covariance model parameters fitted on the whole data set.
It is therefore not strictly equivalent to the naive cross-validation method,
which consists in re-fitting the Gaussian Process Regression model on each training subset.

Examples
--------
Create a Gaussian Process Regression surroagate for the Ishigami function.

>>> import openturns as ot
>>> from openturns.experimental import GaussianProcessRegressionCrossValidation
>>> from openturns.usecases import ishigami_function
>>> im = ishigami_function.IshigamiModel()
>>> sampleSize = 500 
>>> inputTrain = im.distribution.getSample(sampleSize)
>>> outputTrain = im.model(inputTrain)
>>> covariance_kernel = ot.SquaredExponential(inputTrain.getDimension())
>>> basis = ot.ConstantBasisFactory(inputTrain.getDimension()).build()
>>> gpf = ot.GaussianProcessFitter(
...     inputTrain, outputTrain, covariance_kernel, basis
... )
>>> gpf.run()
>>> gpf_result = gpf.getResult()
>>> gpr = ot.GaussianProcessRegression(gpf_result)
>>> gpr.run()
>>> gpr_result = gpr.getResult()

Validate the Gaussian Process Regression surrogate model
using leave-one-out cross-validation.

>>> validation = GaussianProcessRegressionCrossValidation(gpr_result)
>>> r2Score = validation.computeR2Score()
>>> print('R2 = ', r2Score[0])
R2 =  0.99...

Draw the validation graph.

>>> graph = validation.drawValidation()"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessRegressionCrossValidation::getSplitter
"Get the cross-validation method.

Returns
-------
splitter : :class:`~openturns.SplitterImplementation`
    The cross-validation method."

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessRegressionCrossValidation::computeR2Score
"Compute the R2 score.

Returns
-------
r2Score : :class:`~openturns.Point`
    The coefficient of determination R2"

// ---------------------------------------------------------------------

%feature("docstring") OT::GaussianProcessRegressionCrossValidation::getGaussianProcessRegressionResult
"Result accessor.

Returns
-------
result : :class:`~openturns.GaussianProcessRegressionResult`
    The result provided."

