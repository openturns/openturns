%feature("docstring") OT::RandomDirection
"Directions sampling following the random direction strategy.

Parameters
----------
dimension : int
    The dimension of the standard space.
    
    By default, :math:`dimension = 0` but
    automatically updated by the calling class.

See also
--------
OrthogonalDirection

Notes
-----
The random direction strategy generates some directions in the standard space according to the following steps:

- we generate some points on the sphere unity in the standard space according to the uniform distribution,
- we consider both opposite directions so built.

Thus, this process generates 2 directions."

// ---------------------------------------------------------------------

%feature("docstring") OT::RandomDirection::generate
"Generate the directions.

Returns
-------
sample : :class:`~openturns.Sample`
    Both directions generated  according to the strategy.
    
Notes
-----
The sample is the collection of the points on the unit sphere in the standard space
associated to the generated directions."

