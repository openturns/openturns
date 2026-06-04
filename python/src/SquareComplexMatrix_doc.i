%feature("docstring") OT::SquareComplexMatrix
"Complex square matrix.

Parameters
----------
size : int, :math:`n > 0`, optional
    Matrix size.
    Default is 1.
values : sequence of complex with size :math:`n^2`, optional
    Values. OpenTURNS uses **column-major** ordering (like Fortran) for
    reshaping the flat list of values.
    Default creates a zero matrix.

Examples
--------
Create a matrix

>>> import openturns as ot
>>> M = ot.SquareComplexMatrix(2, range(2 * 2))
>>> print(M)
[[ (0,0) (2,0) ]
 [ (1,0) (3,0) ]]

Get or set terms

>>> print(M[0, 0])
0j
>>> M[0, 0] = 1.0
>>> print(M[0, 0])
(1+0j)
>>> print(M[:, 0])
[[ (1,0) ]
 [ (1,0) ]]


Create an openturns matrix from a **square** numpy 2d-array (or matrix, or
2d-list)...

>>> import numpy as np
>>> np_2d_array = np.array([[1.0, 2.0], [3.0, 4.0]])
>>> ot_matrix = ot.SquareComplexMatrix(np_2d_array)

and back

>>> np_matrix = np.matrix(ot_matrix)"

// ---------------------------------------------------------------------

%feature("docstring") OT::SquareComplexMatrix::getDimension
"Accessor to the dimension (the number of rows).

Returns
-------
dimension : int"
