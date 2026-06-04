%feature("docstring") OT::DomainDisjunctiveUnion
"A Domain representing the disjunctive union of several subdomains.

A point is inside this domain if only in exactly one subdomain.

Parameters
----------
collection : sequence of :class:`~openturns.Domain`
    The domains"

// ---------------------------------------------------------------------

%feature("docstring") OT::DomainDisjunctiveUnion::contains
"Check if the given point is inside of the domain.

Parameters
----------
point : sequence of float
    Point with the same dimension as the current domain's dimension.

Returns
-------
isInside : bool
    Flag telling whether the point is inside this domain, which means
    that is is either inside left or right domains, but not both.

Examples
--------
>>> import openturns as ot
>>> # First domain
>>> interval2d = ot.Interval([-1.0, -1.0], [0.5, 0.5])
>>> # Second domain
>>> vertices = [[0.0, 0.0], [1.0, 0.0], [1.0, 1.0], [1.5, 1.0]]
>>> simplices = [[0, 1, 2], [1, 2, 3]]
>>> mesh2d = ot.MeshDomain(ot.Mesh(vertices, simplices))
>>> disjunctive_union = ot.DomainDisjunctiveUnion([interval2d, mesh2d])
>>> [0.2, 0.1] in disjunctive_union
False
>>> [0.1, 0.2] in disjunctive_union
True"
