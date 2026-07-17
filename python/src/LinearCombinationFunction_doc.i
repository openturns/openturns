%feature("docstring") OT::LinearCombinationFunction
R"RAW(Linear combination of functions.

This class allows one to define a new function as a weighted sum of several existing functions.

Let :math:`q \in \Nset` be an integer representing the number of functions.
Let :math:`(f_1, \ldots, f_q)` be a collection of functions where
:math:`f_i: \Rset^{\inputDim} \to \Rset^{\outputDim}` for any
:math:`i \in \{1, \ldots, q\}`.
Let :math:`(c_1, \ldots, c_q)` be a collection of scalar weights such that
:math:`c_i \in \Rset` for any :math:`i \in \{1, \ldots, q\}`.
The linear combination is the function :math:`f: \Rset^{\inputDim} \to \Rset^{\outputDim}` defined by:

.. math::

    f(\vect{x}) = \sum_{i=1}^q c_i f_i(\vect{x})

for any :math:`\vect{x} \in \Rset^{\inputDim}`.

Parameters
----------
functionCollection : sequence of :class:`~openturns.Function`
    Collection of functions to sum.
coefficients : sequence of float
    Collection of scalar weights.

Examples
--------

Let :math:`x \in \Rset` be the input.
The three scalar functions :math:`f_1, f_2, f_3: \Rset \to \Rset` are defined as:  

.. math::

    f_1(x) = \sin(x), \quad f_2(x) = x, \quad f_3(x) = \cos(x)

for any :math:`x \in \Rset`.

With the scalar weights :math:`c_1 = 1`, :math:`c_2 = 2`, :math:`c_3 = 3`,
the resulting linear combination :math:`f: \Rset \to \Rset` is defined by:

.. math::

    f(x) = 1.0 \cdot \sin(x) + 2.0 \cdot x + 3.0 \cdot \cos(x)

for any :math:`x \in \Rset`.

>>> import openturns as ot
>>> f1 = ot.SymbolicFunction(['x'], ['sin(x)'])
>>> f2 = ot.SymbolicFunction(['x'], ['x'])
>>> f3 = ot.SymbolicFunction(['x'], ['cos(x)'])
>>> f = ot.LinearCombinationFunction([f1, f2, f3], [1.0, 2.0, 3.0])

Evaluate the function:

>>> x = [1.0]
>>> y = f(x)
)RAW"
