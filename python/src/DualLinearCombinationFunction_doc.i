%feature("docstring") OT::DualLinearCombinationFunction
R"RAW(Linear combination of functions with vectorial weights.

Allows one to create a function which is the linear combination
of scalar functions with vectorial weights.

Let :math:`q \in \Nset` be an integer representing the number of functions
in the set.
Let :math:`\left(f_1, \ldots, f_q\right)` be a collection of functions
where :math:`f_i: \Rset^{\inputDim} \to \Rset`
for any :math:`i \in \{1, \ldots, q\}`.
Let :math:`\left(\vect{c}_1, \ldots, \vect{c}_q\right)` be a collection
of vectors such that :math:`\vect{c}_i \in \Rset^{\outputDim}`
for any :math:`i \in \{1, \ldots, q\}`.
The linear combination is the function :math:`f : \Rset^{\inputDim} \to \Rset^{\outputDim}`
defined by the equation:

.. math::

    f(\vect{x}) = \sum_{i=1}^q \vect{c}_i f_i(\vect{x})

for any :math:`\vect{x} \in \Rset^{\inputDim}`.

Parameters
----------
functionCollection : sequence of :class:`~openturns.Function`
    Collection of functions.
coefficients : 2-d sequence of float
    Collection of vector coefficients.

Examples
--------

Let :math:`\vect{x} = (x_1, x_2, x_3)^T` be the input vector in :math:`\Rset^3`.

The two scalar functions :math:`f_1, f_2: \Rset^3 \to \Rset` are defined as:

.. math::

    f_1(\vect{x}) = x_1 + 2 x_2 + x_3

    f_2(\vect{x}) = x_1^2 + x_2

for any :math:`\vect{x} \in \Rset^3`.

The corresponding vectorial weights :math:`\vect{c}_1, \vect{c}_2 \in \Rset^2` are:

.. math::

    \vect{c}_1 = \begin{pmatrix} 2 \\ 4 \end{pmatrix}, \quad
    \vect{c}_2 = \begin{pmatrix} 3 \\ 1 \end{pmatrix}.

The resulting linear combination :math:`f: \Rset^3 \to \Rset^2` is defined by:

.. math::

    f(\vect{x}) = \vect{c}_1 f_1(\vect{x}) + \vect{c}_2 f_2(\vect{x})

for any :math:`\vect{x} \in \Rset^3`.

>>> import openturns as ot
>>> f1 = ot.SymbolicFunction(['x1', 'x2', 'x3'],
...                          ['x1 + 2 * x2 + x3'])
>>> f2 = ot.SymbolicFunction(['x1', 'x2', 'x3'],
...                          ['x1^2 + x2'])
>>> functionCollection = [f1, f2]
>>> coefficients = [[2.0, 4.0], [3.0, 1.0]]
>>> function = ot.DualLinearCombinationFunction(functionCollection, coefficients)
>>> print(function([1, 2, 3]))
[25,35]
)RAW"
