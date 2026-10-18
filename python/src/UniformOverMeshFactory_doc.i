%feature("docstring") OT::UniformOverMeshFactory
R"RAW(UniformOverMesh factory.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Estimates a mesh from a sample distributed on a surface of intrinsic
dimension 2 embedded in an ambient space of dimension strictly larger than
2, using the following algorithm:

1. For each point, select its :math:`k` nearest neighbors.
2. Estimate the local tangent plane by a principal component analysis of
   the neighbor offsets.
3. Delaunay triangulate the neighbors projected onto the tangent plane.
4. Merge the triangles of all the charts.
5. Prune the degenerate triangles, i.e. those of negligible area.

The sample given to :meth:`build` or :meth:`buildAsUniformOverMesh` is the
point cloud of the surface itself, not a sample of independent
realizations: its size sets the resolution of the estimated mesh.

See Also
--------
:class:`~openturns.DistributionFactory`, :class:`~openturns.UniformOverMesh`

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``UniformOverMeshFactory-NearestNeighbors`` (``UnsignedInteger``, default: ``12``): number of nearest neighbors
  used to estimate each local tangent plane. It must be adjusted to the
  sample density: the larger the sample size, the larger the value.
- ``UniformOverMeshFactory-MinimumAreaFactor`` (``Scalar``, default: ``1.0e-3``): the degenerate triangles,
  i.e. those whose area is lower than this factor times the maximum
  triangle area, are removed.

Examples
--------
Estimate the distribution from a sample:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> factory = otexp.UniformOverMeshFactory()
>>> vertices = ot.Sample(0, 3)
>>> simplices = []
>>> grid = 8
>>> for j in range(grid + 1):
...     for i in range(grid + 1):
...         vertices.add([i / grid, j / grid, 0.0])
>>> for j in range(grid):
...     for i in range(grid):
...         index = j * (grid + 1) + i
...         simplices.append([index, index + 1, index + grid + 1])
...         simplices.append([index + 1, index + grid + 2, index + grid + 1])
>>> mesh = ot.Mesh(vertices, ot.IndicesCollection(simplices))
>>> sample = ot.UniformOverMesh(mesh).getSample(500)
>>> estimated = factory.buildAsUniformOverMesh(sample)
>>> print(estimated.getDimension())
3
>>> print(estimated.getIntrinsicDimension())
2
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::UniformOverMeshFactory::build
R"RAW(Build the distribution.

The estimated mesh merges the charts of the overlapping neighborhoods, hence
it generally covers the surface more than once and overestimates its
intrinsic volume: it approximates the uniform distribution over the surface,
it does not represent it exactly.

**Available usages**:

    build()

    build(*sample*)

    build(*param*)

Parameters
----------
sample : 2-d sequence of float
    Unstructured point cloud lying on the surface, used as the data of the
    estimation. This is not a sample of independent realizations of the
    estimated distribution: the sample size drives the resolution of the
    estimated mesh and each point contributes one local chart. The
    dimension must be at least 3, the sample size at least 6.
param : sequence of float
    The parameters of the distribution. The UniformOverMesh distribution
    has no parameter, so this usage expects an empty point.

Returns
-------
dist : :class:`~openturns.Distribution`
    The estimated distribution.

    In the first usage, the default native distribution is built, in the
    second and the third the distribution whose mesh is estimated from the
    point cloud.

See Also
--------
:class:`~openturns.DistributionFactory`,
:meth:`~openturns.experimental.UniformOverMeshFactory.buildAsUniformOverMesh`
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::UniformOverMeshFactory::buildAsUniformOverMesh
R"RAW(Estimate the distribution as native distribution.

**Available usages**:

    buildAsUniformOverMesh()

    buildAsUniformOverMesh(*sample*)

Parameters
----------
sample : 2-d sequence of float
    Unstructured point cloud lying on the surface, used as the data of the
    estimation. The dimension must be at least 3, the sample size at
    least 6.

Returns
-------
dist : :class:`~openturns.UniformOverMesh`
    The estimated distribution as a UniformOverMesh.

    In the first usage, the default UniformOverMesh distribution is
    built.)RAW"
