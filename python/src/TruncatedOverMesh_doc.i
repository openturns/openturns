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
The sampling can be done using either a global rejection method or a local rejection method simplex by simplex. This is controlled by the entry *TruncatedOverMesh-UseRejection* of :class:`~openturns.ResourceMap`. If it is set to *True*, the global rejection is used, otherwise the local rejection is used.
If the mesh is significantly smaller than the support of the distribution, the second method is more efficient but it needs an offline computation of cost proportionnal to the number of simplices in the mesh.

This class uses the following entries of :class:`~openturns.ResourceMap`:

- *TruncatedOverMesh-MarginalIntegrationNodesNumber* (`UnsignedInteger`, default: `64`): number of
  integration nodes in each marginal direction,
- *TruncatedOverMesh-MaximumIntegrationNodesNumber* (`UnsignedInteger`, default: `262144`): maximum number
  of integration nodes over the mesh, which bounds the total computational cost,
- *TruncatedOverMesh-OptimizationAlgorithm* (`String`, default: `TNC`): name of the optimization algorithm
  used to compute the integration nodes. Other possible values are the names of the algorithms recognized
  by :class:`~openturns.OptimizationAlgorithm`,
- *TruncatedOverMesh-UseRejection* (`Bool`, default: `false`): whether the sampling uses the global rejection
  method (`true`) or the local rejection method (`false`), as described above.

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
