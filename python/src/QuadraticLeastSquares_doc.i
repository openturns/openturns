%feature("docstring") OT::QuadraticLeastSquares
R"RAW(Second order polynomial response surface by least squares.

Parameters
----------
dataIn : 2-d sequence of float
    Input data.
dataOut : 2-d sequence of float
    Output data.

Notes
-----
A least squares meta model provides an approximation of the model which is valid over its whole domain of definition.

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

..  math::

    \vect{y}^{(k)} = \model\left(\vect{x}^{(k)}\right)

for :math:`1 \leq k \leq \sampleSize`.

This class estimates the parameters of the meta model
:math:`\metaModel: \Rset^\inputDim \rightarrow \Rset^\outputDim` defined by: 

.. math::
    :label: QuadraticLeastSquaresMMOpenTURNSAPI

    \metaModel(\vect{x}) = \vect{c} + \Tr{\mat{L}} ( \vect{x} - \vect{b} )
      + \frac{1}{2} \Tr{( \vect{x} - \vect{b} )} \tens{M} ( \vect{x} - \vect{b} )

for any :math:\vect{x} \in \Rset^\inputDim`.

Refer to :ref:`least_squares` to get details on general least squares meta models and
to get information on the estimation of
the symmetric tensor :math:`\tens{M} \in \Rset^{\outputDim \times \inputDim \times \inputDim}`,
the matrix :math:`\mat{L} \in \cM_{\inputDim, \outputDim}`,
the center vector :math:`\vect{b}\in \Rset^\inputDim` and
the constant vector :math:`\vect{c} \in \Rset^\outputDim`.

See also
--------
LinearLeastSquares, LinearTaylor, QuadraticTaylor

Examples
--------
>>> import openturns as ot
>>> formulas = ['x1 * sin(x2)', 'cos(x1 + x2)', '(x2 + 1) * exp(x1 - 2 * x2)']
>>> f = ot.SymbolicFunction(['x1', 'x2'], formulas)
>>> X  = [[0.5,0.5], [-0.5,-0.5], [-0.5,0.5], [0.5,-0.5]]
>>> X += [[0.25,0.25], [-0.25,-0.25], [-0.25,0.25], [0.25,-0.25]]
>>> Y = f(X)
>>> algo = ot.QuadraticLeastSquares(X, Y)
>>> algo.run()
>>> mm = algo.getResult().getMetaModel()
>>> x = [0.1, 0.1]
>>> y = mm(x)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticLeastSquares::getInputSample
"Get the input data.

Returns
-------
dataIn : :class:`~openturns.Sample`
    Input data.
    
Notes
-----
The input experimental design is defined in :eq:`inputDataQLS`."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticLeastSquares::getCenter
R"RAW(Get the centering vector of the approximation.

Returns
-------
centerVector : :class:`~openturns.Point`
    Center vector :math:`\vect{b}`.
    
Notes
-----
The constant vector :math:`\vect{b}` is defined in :eq:`QuadraticLeastSquaresMMOpenTURNSAPI`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticLeastSquares::getCenter
R"RAW(Get the centering vector of the approximation.

Returns
-------
centerVector : :class:`~openturns.Point`
    Centering vector of the approximation, equal to :math:`c`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticLeastSquares::getConstant
R"RAW(Get the constant vector of the approximation.

Returns
-------
constantVector : :class:`~openturns.Point`
    Constant  vector :math:`\vect{c}`.
    
Notes
-----
The constant vector :math:`\vect{c}` is defined in :eq:`QuadraticLeastSquaresMMOpenTURNSAPI`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticLeastSquares::getOutputSample
"Get the output data.

Returns
-------
dataOut : :class:`~openturns.Sample`
    Output data.
    
Notes
-----
The output experimental design is defined in :eq:`outputDataQLS`."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticLeastSquares::setDataOut
"Set the output data.

Parameters
----------
dataOut : 2-d sequence of float
    Output data."

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticLeastSquares::getLinear
R"RAW(Get the linear matrix of the approximation.

Returns
-------
linearMatrix : :class:`~openturns.Matrix`
    Linear matrix  :math:`\mat{L}`.
    
Notes
-----
The linear matrix :math:`\mat{L}` is defined in :eq:`QuadraticLeastSquaresMMOpenTURNSAPI`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticLeastSquares::getQuadratic
R"RAW(Get the quadratic term of the approximation.

Returns
-------
tensor : :class:`~openturns.SymmetricTensor`
    Symmetric tensor :math:`\tens{M}`.

Notes
-----
The symmetric tensor :math:`\tens{M}` is defined in :eq:`QuadraticLeastSquaresMMOpenTURNSAPI`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticLeastSquares::getResult
R"RAW(Get the meta model result class.

Returns
-------
result : :class:`~openturns.MetaModelResult`
    The class that contains all the results on the meta model. 
    
Notes
-----
To get the meta model :math:`\metaModel` defined in :eq:`QuadraticLeastSquaresMMOpenTURNSAPI` as a :class:`~openturns.Function`, use the :meth:`~openturns.MetaModelResult.getMetaModel` method.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticLeastSquares::run
"Perform the least squares approximation."
