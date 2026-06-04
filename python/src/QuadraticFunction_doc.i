%feature("docstring") OT::QuadraticFunction
R"RAW(Quadratic function.

Parameters
----------
center : sequence of float
    Vector :math:`\vect{b}`.
constant : sequence of float
    Vector :math:`\vect{c}`.
linear : :class:`~openturns.Matrix`
    Matrix :math:`\mat{A}`.
quadratic : :class:`~openturns.SymmetricTensor`
    Tensor :math:`\tens{M}`.

Notes
-----
This class implements a quadratic function :math:`f` defined as:

.. math::

    f : & \Rset^\inputDim \rightarrow \Rset^\outputDim  \\
        & \vect{c} + \Tr{\mat{A}} ( \vect{x} - \vect{b} ) + \frac{1}{2} \Tr{( \vect{x} - \vect{b} )}
        \tens{M}( \vect{x} - \vect{b} )

where:

- :math:`\mat{A}` is a matrix with :math:`\inputDim` lines and :math:`\outputDim` columns,
- :math:`\vect{b}` is a vector in :math:`\Rset^\inputDim`,
- :math:`\vect{c}` is a  vector in :math:`\Rset^\outputDim`,
- :math:`\tens{M}` is a :math:`\Rset^\outputDim \times \Rset^\inputDim \times \Rset^\inputDim` symmetric
  tensor.

Examples
--------
In this example, we create the linear function :math:`f: \Rset^2 \rightarrow \Rset^2`
such that:

.. math::

    \mat{A} = \left( \begin{array}{cc}
    1 & 2 \\
    3 & 4
    \end{array}\right), \vect{b} = \Tr{(0,0)}, \vect{c} = (0)

and:

.. math::

    \tens{M}_{:,:,0} = \left( \begin{array}{cc}
    5 & 6 \\
    7 & 8
    \end{array}\right),

    \tens{M}_{:,:,1} = \left( \begin{array}{cc}
    9 & 10 \\
    11 & 12
    \end{array}\right).


>>> import openturns as ot
>>> d = 2
>>> p = 2
>>> center = [0.0] * d
>>> constant = [0.0] * p
>>> linear = ot.Matrix(d, p)
>>> linear[0,0] = 1.0
>>> linear[0,1] = 2.0
>>> linear[1,0] = 3.0
>>> linear[1,1] = 4.0
>>> quadratic = ot.SymmetricTensor(d, p)
>>> quadratic[0,0,0] = 5
>>> quadratic[0,1,0] = 6
>>> quadratic[1,0,0] = 7
>>> quadratic[1,1,0] = 8
>>> quadratic[0,0,1] = 9
>>> quadratic[0,1,1] = 10
>>> quadratic[1,0,1] = 11
>>> quadratic[1,1,1] = 12
>>> function = ot.QuadraticFunction(center, constant, linear, quadratic)
>>> inP = [1.0, 2.0]
>>> print(function(inP))
[39.5,60.5])RAW"
