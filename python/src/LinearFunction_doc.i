%feature("docstring") OT::LinearFunction
R"RAW(Linear numerical math function.

Parameters
----------
center : sequence of float
    Vector :math:`\vect{b}`.
constant : sequence of float
    Vector :math:`\vect{c}`.
linear : :class:`~openturns.Matrix`
    Matrix :math:`\mat{A}`.

Notes
-----
This implements a linear :class:`~openturns.Function` :math:`f` 
defined as :

.. math::

    f : & \Rset^\inputDim \rightarrow \Rset^\outputDim  \\
        & \vect{x} \mapsto \vect{c} + \mat{A} ( \vect{x} - \vect{b} )

where:

- :math:`\mat{A}` is a matrix with :math:`\outputDim` lines and :math:`\inputDim` columns,
- :math:`\vect{b}` is a vector in :math:`\Rset^\inputDim`,
- :math:`\vect{c}` is a vector in :math:`\Rset^\outputDim`.

See also
--------
Function

Examples
--------
In this example, we create the linear function :math:`f: \Rset^2 \rightarrow \Rset` such that
:math:`f(x,y) = 2x + 3y - 1`. Then we have:

.. math::

    \mat{A} = \left(2,3 \right), \vect{b} = \Tr{(0,0)}, \vect{c} = (-1)

>>> import openturns as ot
>>> center = [0.0, 0.0]
>>> constant = [-1.0]
>>> linear = ot.Matrix(1,2)
>>> linear[0,0] = 2.0
>>> linear[0,1] = 3.0
>>> f = ot.LinearFunction(center, constant, linear)
>>> print(f([1.0, 1.0]))
[4]

Get the center, the constant and the matrix :math:`\mat{A}`:

>>> print(f.getCenter())
[0,0]
>>> print(f.getConstant())
[-1]
>>> print(f.getLinear())
[[ 2 3 ]])RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearFunction::getCenter
"Accessor to the center.

Returns
-------
center : :class:`~openturns.Point`
    The center, as given to the constructor."

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearFunction::getConstant
"Accessor to the constant.

Returns
-------
constant : :class:`~openturns.Point`
    The constant, as given to the constructor."

// ---------------------------------------------------------------------

%feature("docstring") OT::LinearFunction::getLinear
"Accessor to the linear term.

Returns
-------
linear : :class:`~openturns.Matrix`
    The matrix, as given to the constructor."
