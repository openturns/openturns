%feature("docstring") OT::QuadraticEvaluation
R"RAW(Quadratic numerical math evaluation implementation.

Parameters
----------
center : sequence of float
    Vector :math:`\vect{b}`.
constant : sequence of float
    Vector :math:`\vect{c}`.
linear : :class:`~openturns.Matrix`
    Matrix :math:`\mat{A}`.
quadratic : :class:`~openturns.SymmetricTensor`
    Tensor :math:`\underline{\underline{\underline{M}}}`.

Notes
-----
This class implements the evaluation of a quadratic function :math:`f` defined as:

.. math::

    f : & \Rset^d \rightarrow \Rset^p  \\
        & \vect{X} \mapsto \Tr{\mat{A}} ( \vect{X} - \vect{b} ) + \vect{c} + \frac{1}{2} \vect{X}^T.
        \underline{\underline{\underline{M}}}. \vect{X}

where:

- :math:`\mat{A}` is a matrix with :math:`d` lines and :math:`d` columns, called *linear*,
- :math:`\vect{b}` is a vector in :math:`\Rset^d`, called *center*,
- :math:`\vect{c}` is a  vector in :math:`\Rset^p`, called *constant*
- :math:`\underline{\underline{\underline{M}}}` is a :math:`\Rset^p \times \Rset^d \times \Rset^d`
  symmetric  tensor, , called *quadratic*.

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

    \underline{\underline{\underline{M}}}_{:,:,0} = \left( \begin{array}{cc}
    5 & 6 \\
    7 & 8
    \end{array}\right),

    \underline{\underline{\underline{M}}}_{:,:,1} = \left( \begin{array}{cc}
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
>>> function = ot.QuadraticEvaluation(center, constant, linear, quadratic)
>>> inP = [1.0, 2.0]
>>> print(function(inP))
[39.5,60.5])RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticEvaluation::getCenter
R"RAW(Accessor to the center.

Returns
-------
center : sequence of float
    Vector :math:`\vect{b}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticEvaluation::getConstant
R"RAW(Accessor to the constant.

Returns
-------
constant : sequence of float
    Vector :math:`\vect{c}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticEvaluation::getLinear
R"RAW(Accessor to the matrix.

Returns
-------
linear : :class:`~openturns.Matrix`
    Matrix :math:`\mat{A}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QuadraticEvaluation::getQuadratic
R"RAW(Accessor to the symmetric tensor.

Returns
-------
quadratic : :class:`~openturns.SymmetricTensor`
    Tensor :math:`\underline{\underline{\underline{M}}}`.)RAW"
    

