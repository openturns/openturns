%feature("docstring") OT::TruncatedOverMesh
R"RAW(Truncated distribution over a mesh.

Its probability density function is defined as:

.. math::

    f_X(x) = \frac{1}{|\cD|}, \quad x \in \cD

with :math:`\cD\subset\Rset^n` a mesh of dimension :math:`n`.

Parameters
----------
dist : :class:`~openturns.Distribution`
    Distribution to truncate
mesh : :class:`~openturns.Mesh`
    Mesh defining the domain of the distribution.

See also
--------
UniformOverMesh

Notes
-----
The sampling can be done using either a global rejection method or a local rejection method simplex by simplex. This is controlled by the entry *TtuncatedOverMesh-UseRejection* of :class:`~openturns.ResourceMap`. If it is set to *True*, the global rejection is used, otherwise the local rejection is used.
If the mesh is significantly smaller than the support of the distribution, the second method is more efficient but it needs an offline computation of cost proportionnal to the number of simplices in the mesh.

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> f = ot.SymbolicFunction(['x', 'y'], ['sin(x) * sin(y)'])
>>> levelSet = ot.LevelSet(f, ot.Less(), 0.2)
>>> box = ot.Interval([-5.0] * 2, [5.0] * 2)
>>> mesh = ot.LevelSetMesher([50]*2).build(levelSet, box, False)
>>> dist = ot.Normal([0] * 2, [2] * 2)
>>> distribution = ot.TruncatedOverMesh(dist, mesh)

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::TruncatedOverMesh::getMesh
"Accessor to the distribution's mesh.

Returns
-------
mesh : :class:`~openturns.Mesh`
    Mesh."

// ---------------------------------------------------------------------

%feature("docstring") OT::TruncatedOverMesh::setMesh
"Accessor to the distribution's mesh.

Parameters
----------
mesh : :class:`~openturns.Mesh`
    Mesh."
