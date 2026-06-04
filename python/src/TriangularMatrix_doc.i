%feature("docstring") OT::TriangularMatrix
"Triangular Matrix.

Parameters
----------
dim : int
    The dimension of the triangular matrix (square matrix with *dim* rows and 
    *dim* columns).
isLower : bool, optional
    Flag telling if the matrix is triangular lower (*True*) or upper (*False*).
    Default is *True*.

See also
--------
Matrix

Notes
-----
The triangular matrix is filled with 0."

// ---------------------------------------------------------------------

%feature("docstring") OT::TriangularMatrix::isLowerTriangular
"Test whether the matrix is lower triangular or upper triangular.

Returns
-------
isLower : bool
    Flag telling if the matrix is triangular lower (*True*) or upper (*False*)."

// ---------------------------------------------------------------------

%feature("docstring") OT::TriangularMatrix::checkTriangularity
"Check if the internal representation is really triangular."
