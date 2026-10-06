%define OT_HODLRMatrixImplementation_doc
"HODLR compressed matrix implementation.

This class implements the compressed HODLR (Hierarchical Off-Diagonal Low-Rank)
matrix representation with a max-element (partial-pivoting) adaptive cross
approximation and a recursive LLt factorization.

Notes
-----
This class is experimental.

The assembly, factorization and regularization behavior is controlled by the
following :class:`~openturns.ResourceMap` keys:

.. list-table::
   :widths: 50 50
   :header-rows: 1

   * - Key
     - Description
   * - ``HODLRMatrix-DenseThreshold`` (``UnsignedInteger``, default: ``256``)
     - Leaf size below which the dense fallback is used.
   * - ``HODLRMatrix-RegularizationEpsilon`` (``Scalar``, default: ``1.0e-7``)
     - Initial regularization shift.
   * - ``HODLRMatrix-FactorizationIterations`` (``UnsignedInteger``, default: ``20``)
     - Maximum outer regularization attempts.
   * - ``HODLRMatrix-Nugget`` (``Scalar``, default: ``0.0``)
     - Extra nugget added at assembly on top of the model nugget, biasing every solve by its ratio to the smallest eigenvalue; keep at ``0.0`` unless the leaves need stabilization.
   * - ``HODLRMatrix-MaxRegularization`` (``Scalar``, default: ``1.0``)
     - Soft cap for the geometric shift escalation.
   * - ``HODLRMatrix-RecompressCorrections`` (``Bool``, default: ``True``)
     - Recompress the Schur-complement corrections.
   * - ``HODLRMatrix-CompressionMethod`` (``String``, default: ``AcaPartial``)
     - ACA variant for the off-diagonal blocks, ``AcaPartial`` (max-element pivots) or ``AcaRandom`` (hmat-oss random-pivot scheme, usually lower ranks and smaller constants).
   * - ``HODLRMatrix-ParallelAssembly`` (``Bool``, default: ``False``)
     - Build independent child subtrees concurrently (results deterministic except with ``AcaRandom``, whose RNG draws then follow the task schedule). It currently buys almost nothing: with the process pinned to eight performance cores, 1, 2, 4 and 8 threads give 15.9 s to 16.2 s of assembly on a 4D cell, that is no speedup at all. The residual update of the ACA is a single matrix-vector product per pivot and cannot be split, and the tree parallelism is two children per node, so the only parallel share left is the kernel evaluations. It is kept because it is correct and because a future change to the assembly may make it pay, not because it currently does.
   * - ``HODLRMatrix-ProfileFactorization`` (``Bool``, default: ``False``)
     - Print per-block factorization timings.
   * - ``HODLRMatrix-RegularizationFactor`` (``Scalar``, default: ``2.0``)
     - Geometric factor of the shift escalation.
   * - ``HODLRMatrix-RegularizationAttempts`` (``UnsignedInteger``, default: ``60``)
     - Maximum inner regularization attempts per node.
   * - ``HODLRMatrix-PivotFloorFactor`` (``Scalar``, default: ``1.0e-12``)
     - Relative pivot floor for the leaf heal loop, scaled to the raw kernel diagonal; only roundoff-degenerate pivots must trip it.
   * - ``HODLRMatrix-RegularizationWarnThreshold`` (``Scalar``, default: ``1.0e-4``)
     - Warn when the shift reaches this fraction of the diagonal.
   * - ``HODLRMatrix-StackTruncation`` (``Bool``, default: ``True``)
     - Pre-truncate the stacked correction columns.
   * - ``HODLRMatrix-StackTruncationFactor`` (``Scalar``, default: ``0.1``)
     - Fraction of the tolerance budget for the pre-truncation.
   * - ``HODLRMatrix-CorruptionCapFactor`` (``Scalar``, default: ``1000.0``)
     - Factor applied to the raw kernel diagonal: a heal shift beyond this multiple means a numerically corrupted Schur complement and fails loudly.
   * - ``HODLRMatrix-InitialLambda`` (``Scalar``, default: ``1e-4``)
     - Starting shift of the node Schur-complement retry loop, jumped to on first failure.
   * - ``HODLRMatrix-LeafSeedCap`` (``Scalar``, default: ``0.05``)
     - Cap on the outer-loop seed taken from the worst leaf-heal shift, so a corrupted leaf cannot derail the search."
%enddef

%feature("docstring") OT::HODLRMatrixImplementation
OT_HODLRMatrixImplementation_doc

%feature("docstring") OT::HODLRMatrixImplementation::getNbRows
"Return the number of rows.

Returns
-------
n : int
    Number of rows."

%feature("docstring") OT::HODLRMatrixImplementation::getNbColumns
"Return the number of columns.

Returns
-------
n : int
    Number of columns."

%feature("docstring") OT::HODLRMatrixImplementation::getParameters
"Return the HODLR matrix parameters.

Returns
-------
parameters : :class:`~openturns.experimental.HODLRMatrixParameters`
    Parameters of the HODLR matrix."

%feature("docstring") OT::HODLRMatrixImplementation::assemble
"Assemble the matrix from an evaluator.

Parameters
----------
f : callable
    Entry evaluator, such that f(i, j) returns the entry (i, j) of the matrix.
symmetry : str
    Symmetry flag, one of 'L' (lower part only, symmetric matrix), 'U'
    (upper part only, symmetric matrix), 'N' (no symmetry) or 'F'
    (full matrix)."

%feature("docstring") OT::HODLRMatrixImplementation::setPermutation
"Set the spatial permutation.

The matrix is assembled in the permuted order, so this method must be
called before :meth:`assemble`. The permutation is a bijection of
``[0, ..., n-1]`` mapping the permuted index to the original one; an
empty permutation restores the original order.

Parameters
----------
permutation : sequence of int
    Permutation of ``[0, ..., n-1]``, or an empty sequence for the
    identity."

%feature("docstring") OT::HODLRMatrixImplementation::getPermutation
"Return the spatial permutation.

Returns
-------
permutation : :class:`~openturns.Indices`
    The permutation used at assembly time, empty for the identity."

%feature("docstring") OT::HODLRMatrixImplementation::factorize
"Factorize the matrix using the LLt factorization."

%feature("docstring") OT::HODLRMatrixImplementation::scale
"Scale the matrix by a scalar.

The matrix is multiplied by the scalar alpha. The scaling factor is folded
into the evaluator and the compressed representation is rebuilt, which
invalidates any previous factorization. A call to :meth:`factorize` is
required before factor-dependent operations such as :meth:`solve` or
:meth:`logDeterminant`.

Parameters
----------
alpha : float
    Scaling factor."

%feature("docstring") OT::HODLRMatrixImplementation::gemv
"Compute a matrix-vector product.

Computes y = alpha * A * x + beta * y, where A is the matrix. Only the
non-transposed product is supported.

Parameters
----------
trans : str
    Transpose flag, only 'N' is supported.
alpha : float
    Scalar multiplier for A.
x : sequence of float
    Input vector.
beta : float
    Scalar multiplier for the output vector.
y : sequence of float
    Output vector, updated in place."

%feature("docstring") OT::HODLRMatrixImplementation::addIdentity
"Add a multiple of the identity to the matrix.

The matrix A is replaced by A + alpha * I. The matrix must be factorized
again after this call.

Parameters
----------
alpha : float
    Shift to add to the diagonal."

%feature("docstring") OT::HODLRMatrixImplementation::applyFactor
"Compute the product with the Cholesky factor.

Computes y = L * x, where L is the lower-triangular Cholesky factor of
the matrix A, with A = L * L^T. The matrix must have been factorized
with the 'LLt' or 'LLT' method.

The main use is sampling a centered Gaussian random vector of
covariance A: if x is a vector of independent standard normal variates,
then L * x follows the centered normal distribution of covariance A.
Contrary to :meth:`gemv`, which computes the full product A * x (and
would hence sample a random vector of covariance A^2 from unit noise),
this method applies the factor only once.

Parameters
----------
y : sequence of float
    Output vector, updated in place.
x : sequence of float
    Input vector."

%feature("docstring") OT::HODLRMatrixImplementation::applyNugget
"Add a relative nugget to the diagonal.

Adds a small multiple of the mean diagonal to the matrix, following
the formulation C = sigma^2 I + K used in the HODLR Gaussian-process
literature. The relative factor is read from the resource key
``HODLRMatrix-Nugget`` (default ``0.0``); a nonzero value biases every solve
by its ratio to the smallest eigenvalue, so keep it at zero unless the
leaves need stabilization. The nugget bounds the condition number of
ill-conditioned covariance matrices (e.g. long-correlation kernels) and is
applied through the same mechanism as :meth:`addIdentity`.

Notes
-----
This method must be called after assembly and before factorization."

%feature("docstring") OT::HODLRMatrixImplementation::norm
"Return the matrix norm.

This method is not implemented yet."

%feature("docstring") OT::HODLRMatrixImplementation::getDiagonal
"Return the diagonal of the matrix.

Returns
-------
diagonal : :class:`~openturns.Point`
    Diagonal of the matrix, of dimension n."

%feature("docstring") OT::HODLRMatrixImplementation::getRegularizationShift
"Return the regularization shift used by the last factorization.

When the unregularized Cholesky factorization fails (e.g. on a
near-singular covariance matrix), the factorization is retried on
A + shift * I with an increasing shift, following the same scheme as
:class:`~openturns.HMatrix`. This method returns the shift that was
actually applied. A positive value means that the solve() and
logDeterminant() results correspond to the regularized matrix rather
than to A, and the factorization emits a warning whenever the shift is
non-negligible relative to the diagonal (see the
``HODLRMatrix-RegularizationWarnThreshold`` resource map key).

Returns
-------
shift : float
    Accumulated regularization shift (0 if the factorization required
    no regularization)."

%feature("docstring") OT::HODLRMatrixImplementation::solve
"Solve a linear system.

Parameters
----------
b : 1-d or 2-d sequence of float
    Right-hand side.
trans : bool, optional
    Whether to solve with the transpose. The transposed solve is not
    implemented: only False is accepted, and True raises an error.
    Default is False.

Returns
-------
x : 1-d or 2-d sequence of float
    Solution."

%feature("docstring") OT::HODLRMatrixImplementation::solveLower
"Solve a linear system with the Cholesky factor.

Solves L * x = b (or L^T * x = b if trans is True), where L is the lower
Cholesky factor of the matrix. The matrix must have been factorized with
the 'LLt' or 'LLT' method.

Parameters
----------
b : 1-d or 2-d sequence of float
    Right-hand side.
trans : bool, optional
    Whether to solve with the transpose of the factor. Default is False.

Returns
-------
x : 1-d or 2-d sequence of float
    Solution."

%feature("docstring") OT::HODLRMatrixImplementation::logDeterminant
"Return the log-determinant.

The matrix must be factorized first.

Returns
-------
logdet : float
    Logarithm of the absolute value of the determinant."

%feature("docstring") OT::HODLRMatrixImplementation::compressionRatio
"Return the compression ratio of the matrix.

Returns
-------
ratio : sequence of int
    Pair (compressedSize, uncompressedSize), where compressedSize is the
    number of stored coefficients and uncompressedSize is the number of
    entries of the dense matrix."

%feature("docstring") OT::HODLRMatrixImplementation::fullrkRatio
"Return the full-rank ratio of the matrix.

Returns
-------
ratio : sequence of int
    Pair (fullRankSize, lowRankSize), where fullRankSize is the number of
    entries of the dense matrix and lowRankSize is the total storage of
    the low-rank blocks."

%feature("docstring") OT::HODLRMatrixImplementation::dump
"Write a debugging dump of the matrix.

Parameters
----------
name : str
    Name of the dump."
