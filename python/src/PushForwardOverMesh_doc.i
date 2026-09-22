%feature("docstring") OT::PushForwardOverMesh
R"RAW(Push-forward distribution over an embedded mesh.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Its probability density function is the density of :math:`Y = f(X)`
where :math:`X` is an :math:`n`-dimensional absolutely continuous random
vector whose support is a domain discretized by a mesh of simplices, and
:math:`f : \Rset^n \rightarrow \Rset^p` is the piecewise-affine map
defined by a P1 field on this reference mesh, with :math:`p \geq n`. The
distribution :math:`Y` is supported on the image mesh, which may be
embedded in an ambient space of dimension larger than :math:`n`.

The map :math:`f` is affine on each simplex, with a constant Jacobian
matrix :math:`J_{f}`, so that the volume stretching factor
:math:`\sqrt{\det(J_{f}(u)^T J_{f}(u))}` is constant on each simplex:
given a point :math:`y` on the image, its preimage :math:`u` in the
reference domain is obtained exactly by inverting the affine restriction
of :math:`f`, which boils down to a small linear system of size
:math:`n`, instead of a general non-linear solver. The intrinsic density
of :math:`Y` with respect to the measure induced on the image mesh is

.. math::

    f_Y(y) = \sum_{s : y \in f(s)}
    f_X(u_s(y)) \, \sqrt{\frac{\det(G^{\mathrm{ref}}_s)}{\det(G^{\mathrm{img}}_s)}}

where :math:`u_s(y)` is the preimage of :math:`y` in the reference simplex
:math:`s`, and :math:`G^{\mathrm{ref}}_s`, :math:`G^{\mathrm{img}}_s` are
the Gram matrices of the edges of the reference and of the image simplex.
The sum covers all the simplices whose image contains :math:`y`, so both
folding maps and self-intersecting meshes are supported. When the
Jacobian of the map is singular on a simplex (degenerate image), its
contribution is discarded: this is the behaviour of the density at a
critical point.

Realizations are sampled by mapping the realizations of :math:`X` through
the P1 map, so the distribution is exactly normalized regardless of the
mesh refinement.

When :math:`f` is the identity map and :math:`X` is uniform over the
reference mesh, this distribution reduces to the uniform distribution over
the mesh.

The case :math:`p < n` (co-area formula) is not implemented yet.

Parameters
----------
antecedent : :class:`~openturns.Distribution`, absolutely continuous :math:`n`-dimensional, optional
    The distribution of :math:`X`, whose support must be the reference domain.
    When only the field is given, the antecedent defaults to the uniform
    distribution over the mesh of the field.
values : :class:`~openturns.Field`, from :math:`\Rset^n` to :math:`\Rset^p`
    The P1 field defining the piecewise-affine map.

Notes
-----
The tolerance used when checking whether a point belongs to the image mesh
is stored in the `PushForwardOverMesh-OnSimplexEpsilon`
:class:`~openturns.ResourceMap` key.

The number of Monte-Carlo realizations used to estimate the mean and the
covariance are stored in the `PushForwardOverMesh-SampleSize`
:class:`~openturns.ResourceMap` key.

See also
--------
openturns.UniformOverMesh, PushForwardDistribution

Examples
--------
Create the distribution of a point following the uniform distribution over
a reference triangle, mapped by a piecewise-affine map onto the plane
:math:`x + y = z` embedded in :math:`\Rset^3`:

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
"Accessor to the antecedent distribution.

Returns
-------
antecedent : :class:`~openturns.Distribution`, :math:`n`-dimensional
    The distribution of :math:`X`."

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardOverMesh::setAntecedent
"Accessor to the antecedent distribution.

Parameters
----------
antecedent : :class:`~openturns.Distribution`, :math:`n`-dimensional
    The distribution of :math:`X`, whose support must be the reference domain."

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardOverMesh::getValues
"Accessor to the P1 field.

Returns
-------
values : :class:`~openturns.Field`, from :math:`\Rset^n` to :math:`\Rset^p`
    The P1 field defining the piecewise-affine map: the reference mesh and
    the image of each reference vertex."

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardOverMesh::setValues
"Accessor to the P1 field.

Parameters
----------
values : :class:`~openturns.Field`, from :math:`\Rset^n` to :math:`\Rset^p`
    The P1 field defining the piecewise-affine map: the reference mesh and
    the image of each reference vertex."

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardOverMesh::getMesh
"Accessor to the image mesh.

Returns
-------
mesh : :class:`~openturns.Mesh`, embedded in an ambient space of dimension :math:`p`
    The mesh supporting the distribution: the image of the reference mesh
    through the P1 map."

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardOverMesh::getIntrinsicDimension
"Accessor to the intrinsic dimension of the image mesh.

Returns
-------
dim : int
    The dimension of the reference domain, which is the dimension of the
    image mesh."