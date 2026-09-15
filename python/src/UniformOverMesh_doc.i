%feature("docstring") OT::UniformOverMesh
R"RAW(UniformOverMesh distribution.

Its probability density function is defined as:

.. math::

    f_X(x) = \frac{1}{|\cD|}, \quad x \in \cD

with :math:`\cD\subset\Rset^n` the mesh defining the domain, seen as a
subset of :math:`\Rset^n`.

If the intrinsic dimension of the mesh, i.e. the dimension of its simplices,
is equal to :math:`n` (the dimension of its vertices), the density is with
respect to the Lebesgue measure of :math:`\Rset^n`. This is the case of
domains meshed by :class:`~openturns.LevelSetMesher`.

If the mesh is embedded in an ambient space of dimension :math:`n` strictly
larger than its intrinsic dimension, the distribution is singular with
respect to the Lebesgue measure of :math:`\Rset^n` and the density is with
respect to the measure induced on the mesh by the intrinsic Lebesgue
measure of its affine hull. For example a square meshed by triangles lying
in the plane :math:`z=0` of :math:`\Rset^3` has intrinsic dimension 2 and
:math:`|\cD|` is its surface area.

Parameters
----------
mesh : :class:`~openturns.Mesh`
    Mesh defining the domain of the distribution.

See also
--------
TruncatedOverMesh

Examples
--------
Create a distribution over a two dimensional domain:

>>> import openturns as ot
>>> f = ot.SymbolicFunction(['x', 'y'], ['sin(x)*sin(y)'])
>>> levelSet = ot.LevelSet(f, ot.Less(), 0.2)
>>> box = ot.Interval([-5.0]*2, [5.0]*2)
>>> mesh = ot.LevelSetMesher([50]*2).build(levelSet, box, False)
>>> distribution = ot.UniformOverMesh(mesh)

Draw a sample:

>>> sample = distribution.getSample(5)

Explore some of the attributes:

>>> mesh = distribution.getMesh()
>>> algo = distribution.getIntegrationAlgorithm()
>>> distribution.setIntegrationAlgorithm(ot.GaussLegendre([10] * 2))

Create a distribution over a triangular surface embedded in :math:`\Rset^3`:

>>> vertices = ot.Sample([[0.0, 0.0, 0.0], [1.0, 0.0, 0.0], [0.0, 1.0, 0.0]])
>>> simplices = ot.IndicesCollection([[0, 1, 2, 2]])
>>> mesh = ot.Mesh(vertices, simplices)
>>> distribution = ot.UniformOverMesh(mesh)
>>> print(distribution.getDimension())
3
>>> print(distribution.getIntrinsicDimension())
2

The simplices follow the :class:`~openturns.Mesh` convention: a simplex of
intrinsic dimension :math:`d` embedded in an ambient space of dimension
:math:`n` has :math:`d+1` distinct leading vertex indices followed by
trailing entries equal to the last distinct vertex index, so that the number
of vertex indices per simplex is :math:`n+1`.

Notes
-----
The tolerance used by :meth:`computePDF` and :meth:`computeLogPDF` to decide
whether a point lies on the mesh is stored in the
`UniformOverMesh-OnManifoldEpsilon` :class:`~openturns.ResourceMap` key.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::UniformOverMesh::getMesh
"Accessor to the distribution's mesh.

Returns
-------
mesh : ;class:`~openturns.Mesh`
    Mesh."

// ---------------------------------------------------------------------

%feature("docstring") OT::UniformOverMesh::getIntrinsicDimension
"Accessor to the distribution's intrinsic dimension.

Returns
-------
dimension : int
    The intrinsic dimension of the mesh, i.e. the dimension of its
    simplices."

// ---------------------------------------------------------------------

%feature("docstring") OT::UniformOverMesh::getVolume
"Accessor to the distribution's volume.

Returns
-------
volume : float
    The intrinsic volume of the mesh, i.e. the sum of the volumes of its
    simplices."

// ---------------------------------------------------------------------

%feature("docstring") OT::UniformOverMesh::getIntegrationAlgorithm
"Accessor to the integration algorithm used to compute the CDF.

Returns
-------
algo : :class:`~openturns.IntegrationAlgorithm`
    Integration algorithm used to compute the CDF. Default value is
    :class:`~openturns.GaussLegendre` with a marginal integration node number
    specified by the `UniformOverMesh-MarginalIntegrationNodesNumber` key in
    :class:`~openturns.ResourceMap` if the total number of nodes doesn't exceed
    a value specified by the `UniformOverMesh-MaximumIntegrationNodesNumber` key
    in :class:`~openturns.ResourceMap`, otherwise the marginal integration node
    number is decreased.:class:`~openturns.IteratedQuadrature`."

// ---------------------------------------------------------------------

%feature("docstring") OT::UniformOverMesh::setMesh
"Accessor to the distribution's mesh.

Parameters
----------
mesh : :class:`~openturns.Mesh`
    Mesh."

// ---------------------------------------------------------------------

%feature("docstring") OT::UniformOverMesh::setIntegrationAlgorithm
"Accessor to the integration algorithm used to compute the CDF.

Parameters
----------
algo : :class:`~openturns.IntegrationAlgorithm`
    Integration algorithm used to compute the CDF. Default value is
    :class:`~openturns.GaussLegendre` with a marginal integration node number
    specified by the `UniformOverMesh-MarginalIntegrationNodesNumber` key in
    :class:`~openturns.ResourceMap` if the total number of nodes doesn't exceed
    a value specified by the `UniformOverMesh-MaximumIntegrationNodesNumber` key
    in :class:`~openturns.ResourceMap`, otherwise the marginal integration node
    number is decreased."