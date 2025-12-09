%feature("docstring") OT::ConvexHullMesher
"Convex hull mesher from a set of points.

Examples
--------
Compute the convex hull of a sample of random points:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> points = ot.Normal(2).getSample(15)
>>> mesher = otexp.ConvexHullMesher()
>>> convexHull = mesher.build(points)  # doctest: +SKIP"

// ---------------------------------------------------------------------

%feature("docstring") OT::ConvexHullMesher::build
"Build the convex hull from a set of points.

Parameters
----------
points : :class:`~openturns.Sample`
    A set of points.

Returns
-------
mesh : :class:`~openturns.Mesh`
    The mesh built."
