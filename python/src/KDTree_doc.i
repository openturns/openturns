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
