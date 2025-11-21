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

    f : & \Rset^d \rightarrow \Rset^p  \\
        & \vect{X} \mapsto \mat{A} ( \vect{X} - \vect{b} ) + \vect{c}

where:

- :math:`\mat{A}` is a matrix with :math:`p` lines and :math:`d` columns,
- :math:`\vect{b}` is a vector in :math:`\Rset^d`,
- :math:`\vect{c}` is a vector in :math:`\Rset^p`.

See also
--------
Function

Examples
--------
In this example, we create the linear function :math:`f: \Rset^2 \rightarrow \Rset` such that
:math:`f(x,y) = 2x + 3y-1`. Then we have:

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

Get the matrix :math:`\Tr{\mat{A}}`:

>>> A_transpose = f.getEvaluation().getImplementation().getLinear()
)RAW"
