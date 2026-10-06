%feature("docstring") OT::HODLRMatrixParameters
"Parameters for HODLRMatrix class.

This class regroups the parameters used by :class:`~openturns.experimental.HODLRMatrix`.

Notes
-----
This class is experimental.

The following :class:`~openturns.ResourceMap` keys are used:

- ``HODLRMatrix-AssemblyEpsilon``: assembly epsilon for ACA compression (default ``1e-10``).
- ``HODLRMatrix-RecompressionEpsilon``: truncation tolerance for the factor-stage SVD recompression of the Schur-complement corrections, never applied tighter than the assembly epsilon (default ``1.0e-6``).
- ``HODLRMatrix-MinLeafSize``: minimum leaf size (default ``250``).
- ``HODLRMatrix-MaxRank``: maximum rank, adaptive when ``0`` (default ``0``).
- ``HODLRMatrix-UseSpatialOrdering``: use spatial ordering (default ``True``)."

%feature("docstring") OT::HODLRMatrixParameters::getAssemblyEpsilon
"Return the assembly epsilon.

Returns
-------
epsilon : float
    Assembly epsilon for ACA compression."

%feature("docstring") OT::HODLRMatrixParameters::setAssemblyEpsilon
"Set the assembly epsilon.

Parameters
----------
epsilon : float
    Assembly epsilon for ACA compression."

%feature("docstring") OT::HODLRMatrixParameters::getRecompressionEpsilon
"Return the recompression epsilon.

Returns
-------
epsilon : float
    Recompression epsilon."

%feature("docstring") OT::HODLRMatrixParameters::setRecompressionEpsilon
"Set the recompression epsilon.

Parameters
----------
epsilon : float
    Recompression epsilon."

%feature("docstring") OT::HODLRMatrixParameters::getMinLeafSize
"Return the minimum leaf size.

Returns
-------
size : int
    Minimum number of rows/columns for a leaf block."

%feature("docstring") OT::HODLRMatrixParameters::setMinLeafSize
"Set the minimum leaf size.

Parameters
----------
size : int
    Minimum number of rows/columns for a leaf block.

Notes
-----
This is the dominant tuning parameter of the class, and the right value depends
on the kernel rather than on the problem size. A larger leaf means a shallower
tree, hence fewer off-diagonal blocks to compress, and the total rank is a sum
over all levels: on Matern kernels with correlation length 0.1 over the unit
square, going from 64 to 512 divides the factorization time by 1.2 to 2.0 and
the total rank by a third to a half (2D n=4096: total rank 2233 at leaf 64,
1480 at 250, 500 at 1024).

The rule of thumb is that the leaf should be large compared to the correlation
structure and small compared to the mesh. When the correlation length is long
compared to the mesh spacing a large leaf wins on both time and accuracy: over
80 reference cells (dimensions 1 to 4, both kernels, both correlation lengths,
n up to 4096) the default 250 is never less accurate than 64, and is up to 100
times more accurate on 21 of them. When the correlation length falls below the
mesh spacing the kernel is nearly banded, its off-diagonal blocks carry almost
no rank, and a large leaf is paid for as a large dense Cholesky: leaf 512 was
then up to 12 times slower than 64 on the factorization, on cells where that
factorization takes a few milliseconds.

Accuracy and time also move in opposite directions with the problem size at
fixed leaf, so a value suited to one grid is not automatically right for
another: at n=4096 (2D Matern, correlation length 0.1) leaf 1024 brings the
solve error from 2.2e-4 to 1.2e-5, while at n=10000 leaf 250 beats 64, 1024 and
2048 by a factor of two in both directions."

%feature("docstring") OT::HODLRMatrixParameters::getMaxRank
"Return the maximum rank for low-rank blocks.

Returns
-------
rank : int
    Maximum rank of the low-rank blocks. Zero means the rank is
    adaptive, i.e. driven by the assembly epsilon."

%feature("docstring") OT::HODLRMatrixParameters::setMaxRank
"Set the maximum rank for low-rank blocks.

Parameters
----------
rank : int
    Maximum rank of the low-rank blocks. Zero (the default) means the
    rank is adaptive, i.e. each block is compressed up to the assembly
    epsilon. A positive value caps the rank of every block; blocks that
    hit the cap before reaching the assembly epsilon are reported by a
    warning as rank-starved."

%feature("docstring") OT::HODLRMatrixParameters::getUseSpatialOrdering
"Return whether the spatial ordering is used.

Returns
-------
use : bool
    True if the vertices are reordered along a space-filling curve
    before assembly."

%feature("docstring") OT::HODLRMatrixParameters::setUseSpatialOrdering
"Set whether the spatial ordering is used.

Parameters
----------
use : bool
    If True (the default), the vertices are reordered along a
    space-filling curve before assembly, so that the recursive split
    of the HODLR tree separates spatially close points at the leaves."
