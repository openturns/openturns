%feature("docstring") OT::NaiveNearestNeighbour
"Brute force algorithm for nearest-neighbour lookup.

Parameters
----------
sample : 2-d sequence of float
    Points.

See also
--------
NearestNeighbourAlgorithm

Notes
-----

This algorithm compares distance to all points in input sample.
It can be used when sample size is very small, or in high dimension.
In other cases, KDTree is much faster.

Examples
--------
>>> import openturns as ot
>>> sample = ot.Normal(2).getSample(10)
>>> tree = ot.NaiveNearestNeighbour(sample)
>>> neighbour = sample[tree.query([0.1, 0.2])]"
