%feature("docstring") OT::LinearLeastSquares
R"RAW(First order polynomial response surface by least squares.

Parameters
----------
dataIn : 2-d sequence of float
    Input data.
dataOut : 2-d sequence of float
    Output data.

Notes
-----
A least squares surrogate model provides an approximation of the model which is valid over its whole domain of definition.

Let :math:`\model: \Rset^\inputDim \rightarrow \Rset^\outputDim` be a model.
Consider the experimental design :math:`\cX` of size :math:`\sampleSize`:

.. math::
    :label: inputDataLLS

    \cX = \left\{ \vect{x}^{(1)}, \dots, \vect{x}^{(\sampleSize)} \in \Rset^\inputDim \right\}.

Consider the corresponding output experimental design:

.. math::
    :label: outputDataLLS

    \cY = \left\{ \vect{y}^{(1)}, \dots, \vect{y}^{(\sampleSize)} \in \Rset^\outputDim \right\}

where:

.. math::

    \vect{y}^{(k)} = \model\left(\vect{x}^{(k)}\right)

for :math:`1 \leq k \leq \sampleSize`.

This class estimates the parameters of the surrogate model
:math:`\metaModel: \Rset^\inputDim \rightarrow \Rset^\outputDim` defined by:

.. math::
    :label: LinearLeastSquaresMMOpenTURNSAPI

    \metaModel(\vect{x}) = \vect{c} + \Tr{\mat{L}}(\vect{x} - \vect{b})

for any :math:`\vect{x} \in \Rset^\inputDim`, where:

- :math:`\mat{L}` is a matrix with :math:`\inputDim` lines and :math:`\outputDim` columns,
- :math:`\vect{b}` is a vector in :math:`\Rset^\inputDim`,
- :math:`\vect{c}` is a vector in :math:`\Rset^\outputDim`.

Refer to :ref:`least_squares` to get details on general least squares surrogate models
and to get information on the estimation of
the matrix :math:`\mat{L}`,
the center vector :math:`\vect{b}` and
the constant vector :math:`\vect{c}`.

See also
--------
QuadraticLeastSquares, LinearTaylor, QuadraticTaylor

Examples
--------
>>> import openturns as ot
>>> formulas = ['cos(x1 + x2)', '(x2 + 1) * exp(x1 - 2 * x2)']
>>> f = ot.SymbolicFunction(['x1', 'x2'], formulas)
>>> X  = [[0.5,0.5], [-0.5,-0.5], [-0.5,0.5], [0.5,-0.5]]
>>> X += [[0.25,0.25], [-0.25,-0.25], [-0.25,0.25], [0.25,-0.25]]
>>> Y = f(X)
>>> algo = ot.LinearLeastSquares(X, Y)
>>> algo.run()
>>> mm = algo.getResult().getMetaModel()
>>> x = [0.1, 0.1]
>>> y = mm(x)
>>> print(y)
[0.854471,1.06031]
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquares::getInputSample
"Get the input data.

Returns
-------
dataIn : :class:`~openturns.Sample`
    Input data.
    
Notes
-----
The input experimental design is defined in :eq:`inputDataLLS`."

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquares::getCenter
R"RAW(Get the centering vector of the approximation.

Returns
-------
centerVector : :class:`~openturns.Point`
    Center vector :math:`\vect{b}`.
    
Notes
-----
The center vector :math:`\vect{b}` is defined in :eq:`LinearLeastSquaresMMOpenTURNSAPI`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquares::getConstant
R"RAW(Get the constant vector of the approximation.

Returns
-------
constantVector : :class:`~openturns.Point`
    Constant vector :math:`\vect{c}`.
    
Notes
-----
The constant vector :math:`\vect{c}` is defined in :eq:`LinearLeastSquaresMMOpenTURNSAPI`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquares::getOutputSample
"Get the output data.

Returns
-------
dataOut : :class:`~openturns.Sample`
    Output data.
    
Notes
-----
The output experimental design is defined in :eq:`outputDataLLS`."

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquares::setDataOut
"Set the output data.

Parameters
----------
dataOut : 2-d sequence of float
    Output data."

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquares::getLinear
R"RAW(Get the linear matrix of the approximation.

Returns
-------
linearMatrix : :class:`~openturns.Matrix`
    Linear matrix  :math:`\mat{L}`.
    
Notes
-----
The linear matrix :math:`\mat{L}` is defined in :eq:`LinearLeastSquaresMMOpenTURNSAPI`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquares::getResult
R"RAW(Get the surrogate model result class.

Returns
-------
result : :class:`~openturns.MetaModelResult`
    The class that contains all the results on the surrogate model. 
    
Notes
-----
To get the surrogate model :math:`\metaModel` defined in :eq:`LinearLeastSquaresMMOpenTURNSAPI`
as a :class:`~openturns.Function`, use the :meth:`~openturns.MetaModelResult.getMetaModel` method.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquares::run
"Perform the least squares approximation."
