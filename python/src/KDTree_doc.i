%feature("docstring") OT::KDTree
"Partition tree data structure.

Allows one to store and search points fast.

Parameters
----------
sample : 2-d sequence of float
    Points.

See also
--------
NearestNeighbourAlgorithm

Notes
-----
When nanoflann support is enabled, the :class:`~openturns.ResourceMap` key `KDTree-leaf_max_size`
allows one to set the tree leaf size which involves a build vs query tradeoff: large values
will tend to result in fast build and slow queries, and small values typically result in slow build and fast queries.
Also when nanoflann version is at least v1.5.0, the :class:`~openturns.ResourceMap` key `KDTree-n_thread_build`
allows one to set the number of threads used during the tree building phase. It is also decided by OPENTURNS_NUM_THREADS.

Examples
--------
>>> import openturns as ot
>>> sample = ot.Normal(2).getSample(10)
>>> tree = ot.KDTree(sample)
>>> neighbour = sample[tree.query([0.1, 0.2])]"

%feature("docstring") OT::KDTree::getOrdering
"Get a space-filling ordering of the points.

Returns
-------
ordering : :class:`~openturns.Indices`
    The indices of the points sorted along a balanced kd-tree split
    of the longest-extent axis, i.e. a permutation of ``[0, ..., n-1]``
    where ``n`` is the sample size. The first element is the index of
    the first point in the space-filling order."

%feature("docstring") OT::KDTree::setSample
"Set the sample.

Parameters
----------
sample : :class:`~openturns.Sample`
    Points."
