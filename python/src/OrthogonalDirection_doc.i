%feature("docstring") OT::OrthogonalDirection
R"RAW(Directions sampling following the orthogonal direction strategy.

Parameters
----------
dimension : int
    The dimension of the standard space.
    
    By default, :math:`dimension = 0` but automatically updated by the calling class.
k : int
    The number of elements in the linear combinations.
    
    By default, :math:`k = 1` but automatically updated by the calling class.

See also
--------
RandomDirection

Notes
-----
The orthogonal direction strategy is parameterized by :math:`k \in \{1, \ldots, \inputDim\}`, where :math:`\inputDim` is
the dimension of the standard space. We generate some directions in the standard space according to the following steps:

- one direct orthonormalized basis :math:`(\vect{e}_1, \ldots, \vect{e}_\inputDim)` uniformly distributed in
  the set of direct orthonormal bases;
- we consider all the normalized linear combinations of :math:`k` vectors chosen within the :math:`\inputDim` vectors
  of the basis, where the coefficients of the linear combinations are in :math:`\{+1, -1\}`.

Thus, this process generates :math:`\binom{k}{\inputDim} 2^k` directions.

If :math:`k = 1`, we consider all the axes of the space, i.e. :math:`\inputDim` directions.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::OrthogonalDirection::generate
R"RAW(Generate the directions.

Returns
-------
sample : :class:`~openturns.Sample`
    The :math:`\binom{k}{\inputDim} 2^k` directions generated according to the strategy.
    
Notes
-----
The sample is the collection of the :math:`\binom{k}{\inputDim} 2^k` points on the unit sphere in the standard space
associated to the generated directions.)RAW"
