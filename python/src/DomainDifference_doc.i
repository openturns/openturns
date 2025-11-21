%feature("docstring") OT::DomainDifference
"A Domain representing the difference of two Domain.

A point is inside this domain if it is contained in
left domain but not in right domain.

Parameters
----------
left : :class:`~openturns.Domain`
    The first domain
right : :class:`~openturns.Domain`
    The second domain"

// ---------------------------------------------------------------------

%feature("docstring") OT::DomainDifference::contains
"Check if the given point is inside of the domain.

Parameters
----------
point : sequence of float
    Point with the same dimension as the current domain's dimension.

Returns
-------
isInside : bool
    Flag telling whether the point is inside this domain, which means
    that is is inside left domain, but not inside right domain.

Examples
--------
>>> import openturns as ot
>>> # First domain
>>> interval2d = ot.Interval([-1.0, -1.0], [0.5, 0.5])
>>> # Second domain
>>> vertices = [[0.0, 0.0], [1.0, 0.0], [1.0, 1.0], [1.5, 1.0]]
>>> simplices = [[0, 1, 2], [1, 2, 3]]
>>> mesh2d = ot.MeshDomain(ot.Mesh(vertices, simplices))
>>> difference = ot.DomainDifference(interval2d, mesh2d)
>>> [0.2, 0.1] in difference
False
>>> [0.1, 0.2] in difference
True"
