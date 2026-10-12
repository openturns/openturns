%feature("docstring") OT::Grid
"Grid.

Available constructors:
    Grid(*bounds, subdivisions, legend=' '*)

Parameters
----------
bounds : :class:`~openturns.Interval` of dimension 2
    Bounds of the grid, including outer edges.
subdivisions : sequence of int of size 2
    Number of subdivisions in each direction.
legend : str
    Legend of the Grid.

Notes
-----
A Grid is a Drawable that draws uniform grid lines, including the outer
edges, over a 2D bounding box. It is typically combined with a
:class:`~openturns.Cloud` in a :class:`~openturns.Graph` to visualize
how a design of experiments fills the space, see
:meth:`~openturns.VisualTest.DrawPairsWithGrid`.

Examples
--------
>>> import openturns as ot
>>> bounds = ot.Interval([0.0, 0.0], [1.0, 1.0])
>>> grid = ot.Grid(bounds, [5, 5])
>>> cloud = ot.Cloud(ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 2).getSample(10))
>>> graph = ot.Graph()
>>> graph.add(grid)
>>> graph.add(cloud)"

%feature("docstring") OT::Grid::getBounds
"Bounds accessor.

Returns
-------
bounds : :class:`~openturns.Interval`
    Bounds of the grid."

%feature("docstring") OT::Grid::setBounds
"Bounds accessor.

Parameters
----------
bounds : :class:`~openturns.Interval`
    Bounds of the grid, of dimension 2."

%feature("docstring") OT::Grid::getSubdivisions
"Subdivisions accessor.

Returns
-------
subdivisions : :class:`~openturns.Indices`
    Number of subdivisions in each direction."

%feature("docstring") OT::Grid::setSubdivisions
"Subdivisions accessor.

Parameters
----------
subdivisions : sequence of int
    Number of subdivisions in each direction, of size 2."
