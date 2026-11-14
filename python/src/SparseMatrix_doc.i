%feature("docstring") OT::SparseMatrix
R"RAW(Compressed sparse column matrix.

This class stores a matrix in the compressed sparse column (CSC) format, which
is the format expected by sparse solvers: the non-zero coefficients are stored
column by column, together with the row index of each coefficient and the
offset of the first coefficient of each column.

The constructor from triplets builds the canonical form: the row indices are
sorted within each column and the values of duplicate (row, column) coordinates
are summed. Row and column indices are checked against the dimensions.

Parameters
----------
nbRows : int, optional
    Number of rows. Default is 0.
nbColumns : int, optional
    Number of columns. Default is 0.
rowIndices : sequence of int, optional
    Row index of each triplet, used by the constructor from triplets.
columnIndices : sequence of int, optional
    Column index of each triplet, used by the constructor from triplets.
values : sequence of float, optional
    Value of each triplet, used by the constructor from triplets.

Notes
-----
The compressed sparse column layout is available through
:meth:`getColumnPointer`, :meth:`getRowIndices` and :meth:`getValues`.
The column pointer has size :math:`n + 1` and starts at 0, so that column
:math:`j` holds the coefficients of indices ``getColumnPointer()[j]`` to
``getColumnPointer()[j + 1] - 1``.

The stored representation is canonical: the row indices are sorted within
each column and each (row, column) coordinate is stored at most once. The
coefficients can be read and written by index, and a coefficient which is
not stored yet is inserted at its place when it is assigned.

Explicit zeros may be stored: they are kept in the sparse layout, and skipped
when the matrix is converted to a dense matrix or handed over to a solver.
Use :meth:`FromDenseMatrix` to build a sparse matrix from a dense one, which
drops the zero coefficients.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> matrix = otexp.SparseMatrix(3, 2, [0, 2, 0], [0, 1, 0], [1.0, 2.0, 3.0])
>>> print(matrix.getNbNonZeros())
2
>>> print(matrix.getColumnPointer())
[0,1,2]
>>> print(matrix.asDenseMatrix())
[[ 4 0 ]
 [ 0 0 ]
 [ 0 2 ]]
>>> matrix[1, 1] = 7.0
>>> print(matrix.getNbNonZeros())
3
>>> print(matrix.getRowIndices())
[0,1,2]
>>> print(matrix.asDenseMatrix())
[[ 4 0 ]
 [ 0 7 ]
 [ 0 2 ]]
)RAW"

// ---------------------------------------------------------------------------

%feature("docstring") OT::SparseMatrix::FromDenseMatrix
"Conversion from a dense matrix.

The zero coefficients of the dense matrix are skipped.

Parameters
----------
matrix : :class:`~openturns.Matrix`
    The dense matrix to convert.

Returns
-------
sparseMatrix : :class:`~openturns.experimental.SparseMatrix`
    The sparse matrix with the same coefficients."

// ---------------------------------------------------------------------------

%feature("docstring") OT::SparseMatrix::getColumnPointer
"Accessor to the column pointer of the compressed sparse column layout.

Returns
-------
columnPointer : :class:`~openturns.Indices`
    Offset of the first coefficient of each column, of size the number of
    columns plus one and starting at 0."

// ---------------------------------------------------------------------------

%feature("docstring") OT::SparseMatrix::getRowIndices
"Accessor to the row indices of the compressed sparse column layout.

Returns
-------
rowIndices : :class:`~openturns.Indices`
    Row index of each stored coefficient, sorted within each column."

// ---------------------------------------------------------------------------

%feature("docstring") OT::SparseMatrix::getValues
"Accessor to the values of the compressed sparse column layout.

Returns
-------
values : :class:`~openturns.Point`
    Value of each stored coefficient, in the same order as
    :meth:`getRowIndices`."

// ---------------------------------------------------------------------------

%feature("docstring") OT::SparseMatrix::asDenseMatrix
"Conversion to a dense matrix.

Returns
-------
matrix : :class:`~openturns.Matrix`
    The dense matrix with the same coefficients."

// ---------------------------------------------------------------------------

%feature("docstring") OT::SparseMatrix::getNbRows
"Accessor to the number of rows.

Returns
-------
nbRows : int
    Number of rows."

// ---------------------------------------------------------------------------

%feature("docstring") OT::SparseMatrix::getNbColumns
"Accessor to the number of columns.

Returns
-------
nbColumns : int
    Number of columns."

// ---------------------------------------------------------------------------

%feature("docstring") OT::SparseMatrix::getNbNonZeros
"Accessor to the number of stored coefficients.

Returns
-------
nbNonZeros : int
    Number of coefficients stored in the compressed sparse column layout."

// ---------------------------------------------------------------------------

%feature("docstring") OT::SparseMatrix::multiplyByDenseOnLeft
"Multiplication by a dense matrix given as left operand.

The sparse matrix is not densified: the product iterates over its stored
coefficients only.

Parameters
----------
matrix : :class:`~openturns.Matrix`
    The dense left operand, with as many columns as the sparse matrix has rows.

Returns
-------
result : :class:`~openturns.Matrix`
    The dense product."

// ---------------------------------------------------------------------------

%feature("docstring") OT::SparseMatrix::multiplyBySymmetricOnLeft
"Multiplication by a symmetric matrix given as left operand.

Same as :meth:`multiplyByDenseOnLeft`, except that the left operand is read
through the symmetric accessor as only its lower triangle is stored.

Parameters
----------
matrix : :class:`~openturns.SymmetricMatrix`
    The symmetric left operand, with as many columns as the sparse matrix has
    rows.

Returns
-------
result : :class:`~openturns.Matrix`
    The dense product."

// ---------------------------------------------------------------------------

%feature("docstring") OT::SparseMatrix::transpose
"Transposition.

Returns
-------
transpose : :class:`~openturns.experimental.SparseMatrix`
    The transposed matrix."
