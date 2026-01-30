%feature("docstring") OT::MetaModelValidation
"Scores a metamodel in order to perform its validation.

Parameters
----------
outputSample : 2-d sequence of float
    The output validation sample, not used during the learning step.

metamodelPredictions: 2-d sequence of float
    The output prediction sample from the metamodel.

Notes
-----
A `MetaModelValidation` object is used for the validation of a metamodel.
For that purpose, a dataset independent of the learning step, is used to score the surrogate model.
Its main functionalities are :

- compute the coefficient of determination :math:`R^2` ;
- get the residual sample and its non parametric distribution ;
- draw a validation graph presenting the metamodel predictions against
  the model observations.

More details on this topic are presented in :ref:`cross_validation`.

Examples
--------
In this example, we introduce the sinus model and approximate it with a least
squares metamodel.
Then we validate this metamodel using a test sample.

>>> import openturns as ot
>>> from math import pi
>>> dist = ot.Uniform(-pi / 2, pi / 2)
>>> # Define the model
>>> model = ot.SymbolicFunction(['x'], ['sin(x)'])
>>> # We can build several types of models (kriging, polynomial chaos expansion, ...)
>>> # We use here a least squares expansion on canonical basis and compare
>>> # the metamodel with the model
>>> # Build the metamodel using a train sample
>>> x_train = dist.getSample(25)
>>> y_train = model(x_train)
>>> total_degree = 3
>>> polynomialCollection = [f'x^{degree + 1}' for degree in range(total_degree)]
>>> basis = ot.SymbolicFunction(['x'], polynomialCollection)
>>> designMatrix = basis(x_train)
>>> myLeastSquares = ot.LinearLeastSquares(designMatrix, y_train)
>>> myLeastSquares.run()
>>> leastSquaresModel = myLeastSquares.getMetaModel()
>>> metaModel = ot.ComposedFunction(leastSquaresModel, basis)
>>> # Validate the metamodel using a test sample
>>> x_test = dist.getSample(100)
>>> y_test = model(x_test)
>>> metamodelPredictions = metaModel(x_test)
>>> val = ot.MetaModelValidation(y_test, metamodelPredictions)
>>> # Compute the R2 score
>>> r2Score = val.computeR2Score()
>>> # Get the residual
>>> residual = val.getResidualSample()
>>> # Get the histogram of residuals
>>> histoResidual = val.getResidualDistribution(False)
>>> # Draw the validation graph
>>> graph = val.drawValidation()"

// ---------------------------------------------------------------------

%feature("docstring") OT::MetaModelValidation::getOutputSample
"Accessor to the output sample.

Returns
-------
outputSample : :class:`~openturns.Sample`
    Output sample of a model evaluated apart."

// ---------------------------------------------------------------------

%feature("docstring") OT::MetaModelValidation::computeR2Score
R"RAW(Compute the R2 score.

Returns
-------
r2Score : :class:`~openturns.Point`
    The coefficient of determination R2

Notes
-----
The coefficient of determination :math:`R^2` is the fraction of the
variance of the output explained by the metamodel.
It is defined as:

.. math::
    R^2 = 1 - \operatorname{FVU}

where :math:`\operatorname{FVU}` is the fraction of unexplained variance:

.. math::
    \operatorname{FVU} = \frac{\operatorname{MSE}(\tilde{g}) }{\Var{Y}}

where :math:`Y = g(\bdX)` is the output of the physical model :math:`g`,
:math:`\Var{Y}` is the variance of the output and :math:`\operatorname{MSE}` is the 
mean squared error of the metamodel:

.. math::
    \operatorname{MSE}(\tilde{g}) = \Expect{\left(g(\bdX) - \tilde{g}(\bdX) \right)^2}.

The sample :math:`R^2` is:

.. math::
    \hat{R}^2 = 1 - \frac{\frac{1}{n} \sum_{j=1}^{n} \left(y^{(j)} - \tilde{g}\left(\bdx^{(j)}\right)\right)^2}{\hat{\sigma}^2_Y}

where :math:`n \in \Nset` is the sample size, :math:`\tilde{g}` is the metamodel,
:math:`\left\{\bdx^{(j)} \in \Rset^{n_X}\right\}_{j = 1, ..., n}` is the input experimental design,
:math:`\left\{y^{(j)} \in \Rset\right\}_{j = 1, ..., n}` is the output of the model and
:math:`\hat{\sigma}^2_Y` is the sample variance of the output:

.. math::
    \hat{\sigma}^2_Y = \frac{1}{n - 1} \sum_{j=1}^{n} \left(y^{(j)} - \overline{y}\right)^2

where :math:`\overline{y}` is the output sample mean:

.. math::
    \overline{y} = \frac{1}{n} \sum_{j=1}^{n} y^{(j)}.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MetaModelValidation::getResidualSample
R"RAW(Compute the residual sample.

Returns
-------
residual : :class:`~openturns.Sample`
    The residual sample.

Notes
-----
The residual sample is given by :

.. math::

    r^{(j)} = y^{(j)} - \tilde{g}\left(\vect{x}^{(j)}\right)

for :math:`j = 1, ..., n` where :math:`n \in \Nset` is the sample size,
:math:`y^{(j)}` is the model observation,
:math:`\tilde{g}` is the metamodel and :math:`\vect{x}^{(j)}` is the :math:`j`-th input observation.

If the output is multi-dimensional, the residual sample has dimension :math:`n_y \in \Nset`,
where :math:`n_y` is the output dimension.)RAW"


// ---------------------------------------------------------------------

%feature("docstring") OT::MetaModelValidation::getResidualDistribution
"Compute the non parametric distribution of the residual sample.

Parameters
----------
smooth : bool
    Tells if distribution is smooth (true) or not.
    Default argument is true.

Returns
-------
residualDistribution : :class:`~openturns.Distribution`
    The residual distribution.

Notes
-----
The residual distribution is built thanks to :class:`~openturns.KernelSmoothing` if `smooth` argument is true. Otherwise, an histogram distribution is returned, thanks to :class:`~openturns.HistogramFactory`."


// ---------------------------------------------------------------------

%feature("docstring") OT::MetaModelValidation::drawValidation
R"RAW(Plot a model vs metamodel graph for visual validation.

Returns
-------
graph : :class:`~openturns.GridLayout`
    The visual validation graph.

Notes
-----
The plot presents the metamodel predictions depending on the model observations. 
If the points are close to the diagonal line of the plot, then the 
metamodel validation is satisfactory. 
Points which are far away from the diagonal represent outputs 
for which the metamodel is not accurate.

If the output is multi-dimensional, the graph has 1 row and :math:`n_y \in \Nset`
columns, where :math:`n_y` is the output dimension.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MetaModelValidation::computeMeanSquaredError
R"RAW(Accessor to the mean squared error.

Returns
-------
meanSquaredError : :class:`~openturns.Point`
    The mean squared error of each marginal output dimension.

Notes
-----
The sample mean squared error is:

.. math::
    \widehat{\operatorname{MSE}} 
    = \frac{1}{n} \sum_{j=1}^{n} \left(y^{(j)} - \tilde{g}\left(\bdx^{(j)}\right)\right)^2

where :math:`n \in \Nset` is the sample size, :math:`\tilde{g}` is the metamodel,
:math:`\{\bdx^{(j)} \in \Rset^{n_X}\}_{j = 1, ..., n}` is the input experimental design and
:math:`\{y^{(j)} \in \Rset\}_{j = 1, ..., n}` is the output of the model.

If the output is multi-dimensional, the same calculations are repeated separately for
each output marginal :math:`k` for :math:`k = 1, ..., n_y` where :math:`n_y \in \Nset`
is the output dimension.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MetaModelValidation::getMetamodelPredictions
"Accessor to the output predictions from the metamodel.

Returns
-------
outputMetamodelSample : :class:`~openturns.Sample`
    Output sample of the metamodel."
