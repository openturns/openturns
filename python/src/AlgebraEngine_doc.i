%feature("docstring") OT::AlgebraEngine
"Linear algebra operations on DataContainer objects.

This class is experimental.

AlgebraEngine provides static methods for linear algebra operations
on DataContainer objects, delegating to LAPACK/BLAS routines.
All methods operate on raw pointers from DataContainer::data(),
enabling zero-copy interoperability between different container types."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::Dot
"Compute dot product of two vectors.

Parameters
----------
a : DataContainer
    First vector.
b : DataContainer
    Second vector.

Returns
-------
result : float
    Dot product :math:`a^T b`."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::Norm
"Compute Euclidean norm.

Parameters
----------
v : DataContainer
    Input vector.

Returns
-------
result : float
    Euclidean norm :math:`\|v\|_2`."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::Norm1
"Compute l1-norm.

Parameters
----------
v : DataContainer
    Input vector.

Returns
-------
result : float
    l1-norm :math:`\|v\|_1`."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::NormInf
"Compute linf-norm.

Parameters
----------
v : DataContainer
    Input vector.

Returns
-------
result : float
    linf-norm :math:`\|v\|_\infty`."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::Scale
"Scale a vector.

Parameters
----------
x : DataContainer
    Input vector.
alpha : float
    Scalar multiplier.

Returns
-------
result : DataContainer
    :math:`\alpha x`."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::Axpy
"Compute alpha * x + y.

Parameters
----------
alpha : float
    Scalar multiplier.
x : DataContainer
    First input vector.
y : DataContainer
    Second input vector.

Returns
-------
result : DataContainer
    :math:`\alpha x + y`."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::MatrixPointProduct
"Compute matrix-vector product.

Parameters
----------
A : DataContainer
    Matrix (column-major, m x n).
x : DataContainer
    Vector of size n.

Returns
-------
result : DataContainer
    :math:`A x` of size m."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::SymMatrixPointProduct
"Compute symmetric matrix-vector product.

Parameters
----------
A : DataContainer
    Symmetric matrix (column-major, n x n).
x : DataContainer
    Vector of size n.

Returns
-------
result : DataContainer
    :math:`A x` of size n."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::MatrixProduct
"Compute matrix multiplication.

Parameters
----------
A : DataContainer
    Left matrix (column-major, m x k).
B : DataContainer
    Right matrix (column-major, k x n).

Returns
-------
result : DataContainer
    :math:`A B` of size m x n."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::SymProd
"Compute symmetric matrix product.

Parameters
----------
A : DataContainer
    Matrix (column-major).
B : DataContainer
    Matrix (column-major).
symSide : char
    'L' for :math:`A B A^T`, 'R' for :math:`A^T B A`.

Returns
-------
result : DataContainer
    Result matrix."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::TriangularProd
"Compute triangular matrix product.

Parameters
----------
A : DataContainer
    Triangular matrix.
B : DataContainer
    Matrix.
side : char, optional
    'L' (default) for left multiply, 'R' for right.
uplo : char, optional
    'L' (default) for lower triangular, 'U' for upper.

Returns
-------
result : DataContainer
    Product matrix."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeCholesky
"Compute Cholesky factorization.

Parameters
----------
A : DataContainer
    Symmetric positive definite matrix (column-major, n x n).

Returns
-------
L : DataContainer
    Lower triangular matrix such that :math:`L L^T = A`."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeQR
"Compute QR factorization.

Parameters
----------
A : DataContainer
    Input matrix (column-major, m x n).
Q : DataContainer
    Output orthogonal matrix.
R : DataContainer
    Output upper triangular matrix.
fullQR : bool, optional
    If True, Q is m x m. If False (default), Q is m x min(m,n)."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeSVD
"Compute singular value decomposition.

Parameters
----------
A : DataContainer
    Input matrix (column-major, m x n).
U : DataContainer
    Output left singular vectors.
S : DataContainer
    Output singular values.
VT : DataContainer
    Output right singular vectors transposed.
fullSVD : bool, optional
    If True, U is m x m and VT is n x n. If False (default), economy size."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeLU
"Compute LU factorization with partial pivoting.

Parameters
----------
A : DataContainer
    Input square matrix (column-major, n x n).
L : DataContainer
    Output lower triangular matrix.
U : DataContainer
    Output upper triangular matrix."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::SolveLinearSystem
"Solve linear system Ax = b.

Parameters
----------
A : DataContainer
    Square non-singular matrix (column-major, n x n).
b : DataContainer
    Right-hand side vector of size n.

Returns
-------
x : DataContainer
    Solution vector of size n."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::SolveLinearSystemSPD
"Solve linear system for symmetric positive definite A.

Parameters
----------
A : DataContainer
    Symmetric positive definite matrix.
b : DataContainer
    Right-hand side vector.

Returns
-------
x : DataContainer
    Solution vector."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::SolveLinearSystemTriangular
"Solve triangular system.

Parameters
----------
A : DataContainer
    Triangular matrix.
b : DataContainer
    Right-hand side vector.
lower : bool, optional
    If True (default), A is lower triangular.
transpose : bool, optional
    If True, solve A^T x = b.

Returns
-------
x : DataContainer
    Solution vector."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::SolveLinearSystemRectangular
"Solve rectangular system via least-squares.

Parameters
----------
A : DataContainer
    Rectangular matrix (m x n, m >= n).
b : DataContainer
    Right-hand side vector of size m.

Returns
-------
x : DataContainer
    Least-squares solution of size n."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeDeterminant
"Compute determinant.

Parameters
----------
A : DataContainer
    Square matrix.

Returns
-------
det : float
    Determinant of A."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeLogAbsoluteDeterminant
"Compute log of absolute determinant with sign.

Parameters
----------
A : DataContainer
    Square matrix.

Returns
-------
logAbsDet : float
    Log of absolute value of determinant."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeTrace
"Compute trace.

Parameters
----------
A : DataContainer
    Square matrix.

Returns
-------
trace : float
    Trace of A."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeEigenValuesSquare
"Compute eigenvalues of a square matrix.

Parameters
----------
A : DataContainer
    Square matrix (column-major, n x n).

Returns
-------
eigenValues : DataContainer
    Eigenvalues (complex, stored as real/imag pairs)."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeEigenValuesSymmetric
"Compute eigenvalues of a symmetric matrix.

Parameters
----------
A : DataContainer
    Symmetric matrix (column-major, n x n).

Returns
-------
eigenValues : DataContainer
    Real eigenvalues of size n."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeSingularValues
"Compute singular values.

Parameters
----------
A : DataContainer
    Input matrix.

Returns
-------
singularValues : DataContainer
    Singular values in descending order."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::IsSymmetric
"Check if matrix is symmetric.

Parameters
----------
A : DataContainer
    Square matrix.
tolerance : float, optional
    Tolerance for symmetry check. Default is 0.0.

Returns
-------
isSymmetric : bool
    True if A is symmetric within tolerance."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::IsPositiveDefinite
"Check if matrix is positive definite.

Parameters
----------
A : DataContainer
    Square matrix.

Returns
-------
isPD : bool
    True if A is positive definite."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::FrobeniusNorm
"Compute Frobenius norm.

Parameters
----------
A : DataContainer
    Input matrix.

Returns
-------
norm : float
    Frobenius norm :math:`\|A\|_F`."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeGram
"Compute Gram matrix.

Parameters
----------
A : DataContainer
    Input matrix (column-major, m x n).
transpose : bool, optional
    If True (default), compute :math:`A^T A` (n x n).
    If False, compute :math:`A A^T` (m x m).

Returns
-------
G : DataContainer
    Gram matrix."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::Transpose
"Compute matrix transpose.

Parameters
----------
A : DataContainer
    Input matrix (column-major, m x n).

Returns
-------
AT : DataContainer
    Transposed matrix (column-major, n x m)."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::Inverse
"Compute matrix inverse.

Parameters
----------
A : DataContainer
    Square non-singular matrix.

Returns
-------
Ainv : DataContainer
    Inverse of A."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::InverseSPD
"Compute inverse of symmetric positive definite matrix.

Parameters
----------
A : DataContainer
    Symmetric positive definite matrix.

Returns
-------
Ainv : DataContainer
    Inverse of A."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::SumElements
"Compute sum of all elements.

Parameters
----------
A : DataContainer
    Input matrix or vector.

Returns
-------
result : float
    Sum of all elements."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::Clean
"Clean small elements.

Parameters
----------
A : DataContainer
    Input matrix.
threshold : float
    Elements with absolute value below threshold are set to zero.

Returns
-------
result : DataContainer
    Cleaned matrix."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::MatrixProductBlockwise
"Block matrix multiplication.

Parameters
----------
A : DataContainer
    Left matrix.
B : DataContainer
    Right matrix.
blockSize : int, optional
    Block size. Default from ResourceMap key ``AlgebraEngine-DefaultBlockSize``.

Returns
-------
C : DataContainer
    :math:`A B`."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeCholeskyBlockwise
"Block Cholesky factorization.

Parameters
----------
A : DataContainer
    Symmetric positive definite matrix.
blockSize : int, optional
    Block size. Default from ResourceMap key ``AlgebraEngine-DefaultBlockSize``.

Returns
-------
L : DataContainer
    Lower triangular factor."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::SolveLinearSystemBlockwise
"Block linear solve.

Parameters
----------
A : DataContainer
    Square non-singular matrix.
b : DataContainer
    Right-hand side vector.
blockSize : int, optional
    Block size. Default from ResourceMap key ``AlgebraEngine-DefaultBlockSize``.

Returns
-------
x : DataContainer
    Solution vector."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeQRBlockwise
"Block QR factorization.

Parameters
----------
A : DataContainer
    Input matrix.
Q : DataContainer
    Output orthogonal matrix.
R : DataContainer
    Output upper triangular matrix.
fullQR : bool, optional
    If True, Q is m x m. Default is False.
blockSize : int, optional
    Block size. Default from ResourceMap key ``AlgebraEngine-DefaultBlockSize``."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeLUBlockwise
"Block LU factorization.

Parameters
----------
A : DataContainer
    Input square matrix.
L : DataContainer
    Output lower triangular matrix.
U : DataContainer
    Output upper triangular matrix.
blockSize : int, optional
    Block size. Default from ResourceMap key ``AlgebraEngine-DefaultBlockSize``."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::SolveLinearSystemTriangularBlockwise
"Block triangular solve.

Parameters
----------
T : DataContainer
    Triangular matrix.
b : DataContainer
    Right-hand side vector.
lower : bool, optional
    If True (default), T is lower triangular.
transpose : bool, optional
    If True, solve T^T x = b.
blockSize : int, optional
    Block size. Default from ResourceMap key ``AlgebraEngine-DefaultBlockSize``.

Returns
-------
x : DataContainer
    Solution vector."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeGramBlockwise
"Block Gram matrix computation.

Parameters
----------
A : DataContainer
    Input matrix.
transpose : bool, optional
    If True (default), compute :math:`A^T A`.
blockSize : int, optional
    Block size. Default from ResourceMap key ``AlgebraEngine-DefaultBlockSize``.

Returns
-------
G : DataContainer
    Gram matrix."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeDeterminantBlockwise
"Block determinant via block LU.

Parameters
----------
A : DataContainer
    Square matrix.
blockSize : int, optional
    Block size. Default from ResourceMap key ``AlgebraEngine-DefaultBlockSize``.

Returns
-------
det : float
    Determinant."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::ComputeLogAbsoluteDeterminantBlockwise
"Block log-absolute-determinant.

Parameters
----------
A : DataContainer
    Square matrix.
blockSize : int, optional
    Block size. Default from ResourceMap key ``AlgebraEngine-DefaultBlockSize``.

Returns
-------
logAbsDet : float
    Log of absolute determinant."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::InverseBlockwise
"Block matrix inverse.

Parameters
----------
A : DataContainer
    Square non-singular matrix.
blockSize : int, optional
    Block size. Default from ResourceMap key ``AlgebraEngine-DefaultBlockSize``.

Returns
-------
Ainv : DataContainer
    Inverse of A."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::InverseSPDBlockwise
"Block inverse of SPD matrix.

Parameters
----------
A : DataContainer
    Symmetric positive definite matrix.
blockSize : int, optional
    Block size. Default from ResourceMap key ``AlgebraEngine-DefaultBlockSize``.

Returns
-------
Ainv : DataContainer
    Inverse of A."

// ---------------------------------------------------------------------

%feature("docstring") OT::AlgebraEngine::SolveLinearSystemRectangularBlockwise
"Block least-squares solve.

Parameters
----------
A : DataContainer
    Rectangular matrix (m x n, m >= n).
b : DataContainer
    Right-hand side vector of size m.
blockSize : int, optional
    Block size. Default from ResourceMap key ``AlgebraEngine-DefaultBlockSize``.

Returns
-------
x : DataContainer
    Least-squares solution of size n."
