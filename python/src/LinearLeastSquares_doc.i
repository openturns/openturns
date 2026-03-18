%feature("docstring") OT::LinearLeastSquares
R"RAW(First order polynomial response surface by least squares.

Parameters
----------
dataIn : 2-d sequence of float
    Input data.
dataOut : 2-d sequence of float
    Output data.`.

Notes
-----
A least squares meta model provides an approximation of the model which is valid over its whole domain of definition.

This class estimates the meta model :math:`\metaModel: \Rset^\inputDim \rightarrow \Rset^\outputDim` defined by: 

  .. math::
    :label: LinearLeastSquaresMMOpenTURNSAPI
    \metaModel(\vect{x})  =  \vect{c} + \Tr{\mat{L}}(\vect{x} - \vect{b}}

from an experimental design :math:`\cX` of size :math:`\sampleSize`, that is, a set of observations of
the input vector defined by:

.. math::
    :label: inputData
    \cX = \left\{ \vect{x}^{(1)}, \dots, \vect{x}^{(\sampleSize)} \right\},

and the corresponding output vectors:

.. math::
    :label: outputData
    \cY = \left\{ \vect{y}^{(1)}, \dots, \vect{y}^{(\sampleSize)} \right\}.

where :math:`\vect{y}^{(k)} = \model{ \vect{x}^{(k)}}`. 

Refer to :ref:`least_squares` to get details on general least squares meta models and to get information on the 
estimation of the matrix :math:`\mat{M} \in \cM_{\inputDim, \outputDim}`, the center vector
:math:`\vect{b}\in \Rset^\inputDim` and the constant vector :math:`\vect{c} \in \Rset^\outputDim`.


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
The input experimental design is defined in :eq:`inputData`."

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquares::getCenter
"Get the centering vector of the approximation.

Returns
-------
centerVector : :class:`~openturns.Point`
    Center vector :math:`\vect{b}`.
    
Notes
-----
The constant vector :math:`\vect{b}` is defined in :eq:`LinearLeastSquaresMMOpenTURNSAPI`.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquares::getConstant
"Get the constant vector of the approximation.

Returns
-------
constantVector : :class:`~openturns.Point`
    Constant vector :math:`\vect{c}`.
    
Notes
-----
The constant vector :math:`\vect{c}` is defined in :eq:`LinearLeastSquaresMMOpenTURNSAPI`.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquares::getOutputSample
"Get the output data.

Returns
-------
dataOut : :class:`~openturns.Sample`
    Output data.
    
Notes
-----
The output experimental design is defined in :eq:`outputData`."

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquares::setDataOut
"Set the output data.

Parameters
----------
dataOut : 2-d sequence of float
    Output data."

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquares::getLinear
"Get the linear matrix of the approximation.

Returns
-------
linearMatrix : :class:`~openturns.Matrix`
    Linear matrix  :math:`\mat{L}`.
    
Notes
-----
The linear matrix :math:`\mat{L}` is defined in :eq:`LinearLeastSquaresMMOpenTURNSAPI`."

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquares::getResult
"Get the meta model result class.

Returns
-------
result : :class:`~openturns.MetaModelResult`
    The class that contains all the results on the meta model. 
    
Notes
-----
To get the meta model :math:`\metaModel` defined in :eq:`LinearLeastSquaresMMOpenTURNSAPI` as a :class:`~openturns.Function`, use the :meth:``~openturns.MetaModelResult.getMetaModel` method."

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearLeastSquares::run
"Perform the least squares approximation."
