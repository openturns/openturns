%feature("docstring") OT::PushForwardOverMesh
R"RAW(Push-forward distribution over an embedded mesh.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Its probability density function is the density of :math:`\outputRV = f(\inputRV)`
where :math:`\inputRV` is an :math:`\inputDim`-dimensional absolutely continuous random
vector whose support is a domain discretized by a mesh of simplices, and
:math:`f: \Rset^\inputDim \rightarrow \Rset^\outputDim` is the piecewise-affine map
defined by a P1 field on this reference mesh, with :math:`\outputDim \geq \inputDim`. The
distribution :math:`\outputRV` is supported on the image mesh, which may be
embedded in an ambient space of dimension larger than :math:`\inputDim`.

The map :math:`f` is affine on each simplex, with a constant Jacobian
matrix :math:`J_{f}`, so that the volume stretching factor
:math:`\sqrt{\det(J_{f}(\inputReal)^T J_{f}(\inputReal))}` is constant on each simplex:
given a point :math:`\outputReal` on the image, its preimage :math:`\inputReal` in the
reference domain is obtained exactly by inverting the affine restriction
of :math:`f`, which boils down to a small linear system of size
:math:`\inputDim`, instead of a general non-linear solver. The intrinsic density
of :math:`\outputRV` with respect to the measure induced on the image mesh is

.. math::

    f_{\outputRV}(\outputReal) = \sum_{s : \outputReal \in f(s)}
    f_{\inputRV}(\inputReal_s(\outputReal)) \, \sqrt{\frac{\det(G^{\mathrm{ref}}_s)}{\det(G^{\mathrm{img}}_s)}}

where :math:`\inputReal_s(\outputReal)` is the preimage of :math:`\outputReal` in the reference simplex
:math:`s`, and :math:`G^{\mathrm{ref}}_s`, :math:`G^{\mathrm{img}}_s` are
the Gram matrices of the edges of the reference and of the image simplex.
The sum covers all the simplices whose image contains :math:`\outputReal`, so both
folding maps and self-intersecting meshes are supported. When the
Jacobian of the map is singular on a simplex (degenerate image), its
contribution is discarded: this is the behaviour of the density at a
critical point.

Realizations are sampled by mapping the realizations of :math:`\inputRV` through
the P1 map, so the distribution is exactly normalized regardless of the
mesh refinement.

When :math:`f` is the identity map and :math:`\inputRV` is uniform over the
reference mesh, this distribution reduces to the uniform distribution over
the mesh.

The case :math:`\outputDim < \inputDim` (co-area formula) is implemented for
the codimension 1 uniform case (reference dimension minus image dimension
equal to 1 with ``UniformOverMesh`` antecedent): the density integrates the
antecedent over the fiber segments with the local projection Jacobian.
Other co-area cases (higher codimension or non-uniform antecedent) are not
implemented yet.

Parameters
----------
antecedent : :class:`~openturns.Distribution`, absolutely continuous :math:`\inputDim`-dimensional, optional
    The distribution of :math:`\inputRV`, whose support must be the reference domain.
    When only the field is given, the antecedent defaults to the uniform
    distribution over the mesh of the field.
values : :class:`~openturns.Field`, from :math:`\Rset^\inputDim` to :math:`\Rset^\outputDim`
    The P1 field defining the piecewise-affine map.

See also
--------
:class:`~openturns.UniformOverMesh`, :class:`~openturns.experimental.PushForwardDistribution`

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``PushForwardOverMesh-OnSimplexEpsilon`` (``Scalar``, default: ``1.0e-10``): tolerance used when checking
  whether a point belongs to the image mesh.
- ``PushForwardOverMesh-SampleSize`` (``UnsignedInteger``, default: ``10000``): number of Monte-Carlo
  realizations used to estimate the mean and the covariance.

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
antecedent : :class:`~openturns.Distribution`, :math:`\inputDim`-dimensional
    The distribution of :math:`\inputRV`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardOverMesh::setAntecedent
R"RAW(Accessor to the antecedent distribution.

Parameters
----------
antecedent : :class:`~openturns.Distribution`, :math:`\inputDim`-dimensional
    The distribution of :math:`\inputRV`, whose support must be the reference domain.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardOverMesh::getValues
R"RAW(Accessor to the P1 field.

Returns
-------
values : :class:`~openturns.Field`, from :math:`\Rset^\inputDim` to :math:`\Rset^\outputDim`
    The P1 field defining the piecewise-affine map: the reference mesh and
    the image of each reference vertex.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardOverMesh::setValues
R"RAW(Accessor to the P1 field.

Parameters
----------
values : :class:`~openturns.Field`, from :math:`\Rset^\inputDim` to :math:`\Rset^\outputDim`
    The P1 field defining the piecewise-affine map: the reference mesh and
    the image of each reference vertex.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardOverMesh::getMesh
R"RAW(Accessor to the image mesh.

Returns
-------
mesh : :class:`~openturns.Mesh`, embedded in an ambient space of dimension :math:`\outputDim`
    The mesh supporting the distribution: the image of the reference mesh
    through the P1 map.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardOverMesh::getIntrinsicDimension
R"RAW(Accessor to the intrinsic dimension of the image mesh.

Returns
-------
dim : int
    The dimension of the reference domain, which is the dimension of the
    image mesh.)RAW"