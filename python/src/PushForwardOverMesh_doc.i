%feature("docstring") OT::PushForwardOverMesh
R"RAW(Push-forward distribution over an embedded mesh.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This distribution is the piecewise-affine special case of
:class:`~openturns.experimental.PushForwardDistribution`, where the map
:math:`f` is defined by a P1 (piecewise-affine) field on a mesh of simplices.
The field is represented by a :class:`~openturns.Field`: its values are the
images of the vertices of the reference :class:`~openturns.Mesh`, and its
restriction to each simplex is the affine interpolation of these values.
The general definition of the push-forward distribution, of its density with
respect to the measure induced on the image, and of the co-area case are given
in the :class:`~openturns.experimental.PushForwardDistribution` documentation.
The mesh-specific construction and the resulting density formula are detailed
in :ref:`pushforward_over_mesh`.

Here, :math:`\inputRV` is an :math:`n`-dimensional absolutely continuous
random vector whose support is a domain discretized by a mesh of simplices, and
:math:`f: \Rset^n \rightarrow \Rset^\outputDim` is the P1 map defined on this
reference mesh. Its restriction to each simplex is affine, so the preimage of a
point of the image is obtained by solving a small linear system instead of a
general non-linear problem. The image mesh is a :math:`d`-dimensional object,
where :math:`d` is the intrinsic dimension of the reference mesh, and it may
be embedded in an ambient space of dimension larger than :math:`d`.

Realizations are sampled by mapping the realizations of :math:`\inputRV` through
the P1 map, so the distribution is exactly normalized regardless of the
mesh refinement.

The mean and the covariance are computed deterministically, not estimated by
Monte-Carlo: they are integrated over the reference mesh with
:class:`~openturns.SimplicialCubature`, which applies a cubature rule to the
P1 map and the antecedent density on each simplex. This also covers the
co-area case, where the integration is performed over the reference domain
rather than over the collapsed image mesh.

When :math:`f` is the identity map and :math:`\inputRV` is uniform over the
reference mesh, this distribution reduces to the uniform distribution over
the mesh.

The case :math:`\outputDim < d` (co-area formula) is implemented only for
:math:`d = 2`, ambient reference dimension :math:`n = 2`, and
:math:`\outputDim = 1`, with a ``UniformOverMesh`` antecedent: the density
integrates the antecedent over the fiber segments with the local projection
Jacobian. Intrinsic-preserving maps with :math:`\outputDim \geq d`, including
embedded meshes such as a polyline in :math:`\Rset^2` mapped to scalars, use
the weighted piecewise-affine path above. Other co-area cases are not
implemented yet.

Parameters
----------
antecedent : :class:`~openturns.Distribution`, absolutely continuous :math:`n`-dimensional, optional
    The distribution of :math:`\inputRV`, whose support must be the reference domain.
    When only the field is given, the antecedent defaults to the uniform
    distribution over the mesh of the field.
values : :class:`~openturns.Field`, from :math:`\Rset^n` to :math:`\Rset^\outputDim`
    The P1 field defining the piecewise-affine map.

See also
--------
:class:`~openturns.UniformOverMesh`, :class:`~openturns.experimental.PushForwardDistribution`

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``PushForwardOverMesh-OnSimplexEpsilon`` (``Scalar``, default: ``1.0e-10``): tolerance used when checking
  whether a point belongs to the image mesh.

Examples
--------
Create the distribution of a point following the uniform distribution over
a reference triangle, mapped by a piecewise-affine map onto the plane
:math:`\outputReal_0 + \outputReal_1 = \outputReal_2` embedded in :math:`\Rset^3`:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> mesh = ot.Mesh(ot.Sample([[0.0, 0.0], [1.0, 0.0], [0.0, 1.0]]),
...                ot.IndicesCollection([[0, 1, 2]]))
>>> values = ot.Sample([[0.0, 0.0, 0.0], [1.0, 0.0, 1.0], [0.0, 1.0, 1.0]])
>>> dist = otexp.PushForwardOverMesh(ot.Field(mesh, values))
>>> dist.computePDF([0.25, 0.25, 0.5])
1.1547005...

Draw a sample:

>>> sample = dist.getSample(5)
>>> print(sample.getDimension())
3
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardOverMesh::getAntecedent
R"RAW(Accessor to the antecedent distribution.

Returns
-------
antecedent : :class:`~openturns.Distribution`, :math:`n`-dimensional
    The distribution of :math:`\inputRV`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardOverMesh::setAntecedent
R"RAW(Accessor to the antecedent distribution.

Parameters
----------
antecedent : :class:`~openturns.Distribution`, :math:`n`-dimensional
    The distribution of :math:`\inputRV`, whose support must be the reference domain.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardOverMesh::getValues
R"RAW(Accessor to the P1 field.

Returns
-------
values : :class:`~openturns.Field`, from :math:`\Rset^n` to :math:`\Rset^\outputDim`
    The P1 field defining the piecewise-affine map: the reference mesh and
    the image of each reference vertex.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardOverMesh::setValues
R"RAW(Accessor to the P1 field.

Parameters
----------
values : :class:`~openturns.Field`, from :math:`\Rset^n` to :math:`\Rset^\outputDim`
    The P1 field defining the piecewise-affine map: the reference mesh and
    the image of each reference vertex.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardOverMesh::getMesh
R"RAW(Accessor to the image mesh.

Returns
-------
mesh : :class:`~openturns.Mesh`, embedded in an ambient space of dimension :math:`\outputDim`
    The mesh supporting the distribution: the image of the reference mesh
    through the P1 map.

Notes
-----
In the co-area case :math:`\outputDim < d`, where :math:`d` is the intrinsic
dimension of the reference mesh, the image simplices are collapsed and cannot
be reconstructed as a mesh: this accessor raises a
*NotYetImplementedException*.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardOverMesh::getIntrinsicDimension
R"RAW(Accessor to the intrinsic dimension of the image mesh.

Returns
-------
dim : int
    The intrinsic dimension of the image mesh.  In the supported co-area case
    :math:`\outputDim < d`, the image is assumed to be full-dimensional of
    dimension :math:`\outputDim`, which is returned.)RAW"
