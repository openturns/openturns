%feature("docstring") OT::DomainUnion
"A Domain representing the union of two Domain.

A point is inside this domain if it is contained in at least one domain.

Parameters
----------
collection : sequence of :class:`~openturns.Domain`
    The domains"

// ---------------------------------------------------------------------

%feature("docstring") OT::DomainUnion::contains
"Check if the given point is inside of the domain.

Parameters
----------
point : sequence of float
    Point with the same dimension as the current domain's dimension.

Returns
-------
isInside : bool
    Flag telling whether the point is inside this domain, which means
    that is at least in one domain.

Examples
--------
>>> import openturns as ot
>>> # First domain
>>> interval2d = ot.Interval([-1.0, -1.0], [0.5, 0.5])
>>> # Second domain
>>> vertices = [[0.0, 0.0], [1.0, 0.0], [1.0, 1.0], [1.5, 1.0]]
>>> simplices = [[0, 1, 2], [1, 2, 3]]
>>> mesh2d = ot.MeshDomain(ot.Mesh(vertices, simplices))
>>> intersection = ot.DomainUnion([interval2d, mesh2d])
>>> [0.2, 0.1] in intersection
True
>>> [0.1, 0.2] in intersection
True"
