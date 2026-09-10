%feature("docstring") OT::HODLRMatrixParameters
"Parameters for HODLRMatrix class.

This class regroups the parameters used by :class:`~openturns.HODLRMatrix`.

Notes
-----
This class is experimental.

The default values of the attributes are read from the ResourceMap keys:

- 'HODLRMatrix-AssemblyEpsilon' (default 1.0e-7)
- 'HODLRMatrix-RecompressionEpsilon' (default 1.0e-7)
- 'HODLRMatrix-MinLeafSize' (default 250)
- 'HODLRMatrix-MaxRank' (default 0)
- 'HODLRMatrix-UseSpatialOrdering' (default true)"

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
    Minimum number of rows/columns for a leaf block."

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
