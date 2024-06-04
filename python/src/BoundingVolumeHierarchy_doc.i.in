%feature("docstring") OT::BoundingVolumeHierarchy
"Bounding Volume Hierarchy to speed-up point location.

This spatial data structure helps to find the simplex
containing a given point.

Parameters
----------
points : 2-d sequence of float
    Points.

simplices : :class:`~openturns.IndicesCollection`
    Simplices.

binNumber : int, optional
    Maximum number of simplices stored in tree leaves.
    By default, it is equal to the value defined through the key
    BoundingVolumeHierarchy-BinNumber of the
    :class:`~openturns.ResourceMap`.

strategy : str, optional
    Node splitting strategy.  Valid values are: `Mean` and `Median`.
    By default, it is equal to the value defined through the key
    BoundingVolumeHierarchy-Strategy of the
    :class:`~openturns.ResourceMap` (`Mean`).

See also
--------
EnclosingSimplexAlgorithm

Examples
--------
>>> import openturns as ot
>>> mesher = ot.IntervalMesher([5, 10])
>>> lowerbound = [0.0, 0.0]
>>> upperBound = [2.0, 4.0]
>>> interval = ot.Interval(lowerbound, upperBound)
>>> mesh = mesher.build(interval)
>>> locator = ot.BoundingVolumeHierarchy(mesh.getVertices(), mesh.getSimplices())
>>> simplex = locator.query([0.1, 0.2])"

