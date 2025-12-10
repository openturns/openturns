%feature("docstring") OT::PolygonMesher
"2-d Polygon meshing algorithm.

Examples
--------
Triangulate a simple polygon:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> mesher = otexp.PolygonMesher()
>>> polyline = [[3.0, 0.0], [2.0, 0.0], [2.0, 0.75], [2.5, 0.75], [3.0, 0.2]]
>>> triangulation = mesher.build(polyline)"

// ---------------------------------------------------------------------

%feature("docstring") OT::PolygonMesher::build
"Generate a mesh from polygon coordinates.

Parameters
----------
polyline : :class:`~openturns.Sample`
    An ordered set of points defining a 2-d polygon (possibly non-convex).

Returns
-------
mesh : :class:`~openturns.Mesh`
    The triangulation generated."
