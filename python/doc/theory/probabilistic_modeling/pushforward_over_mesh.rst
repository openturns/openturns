.. _pushforward_over_mesh:

Push-forward distribution over a mesh
=====================================

Let :math:`\inputRV` be an absolutely continuous random vector with values in
:math:`\Rset^n`, whose support is a domain discretized by a mesh of simplices.
Let :math:`d \leq n` be the intrinsic dimension of the reference mesh and let
:math:`\outputDim` be the output dimension, with :math:`\outputDim \geq d`.

The map :math:`f: \Rset^n \rightarrow \Rset^\outputDim` is a P1 field on the
reference mesh: its restriction to each simplex is the affine interpolation of
the images of the simplex vertices. Such a field is represented by a
:class:`~openturns.Field`, which stores the reference :class:`~openturns.Mesh`
and the image of each vertex. On a simplex :math:`s` with vertices
:math:`(\vect{v}_0, \dots, \vect{v}_d)`, a point
:math:`\inputReal \in s` has barycentric coordinates
:math:`(\lambda_0, \dots, \lambda_d)` and

.. math::

    f(\inputReal) = \sum_{k = 0}^d \lambda_k f(\vect{v}_k).

The restriction of :math:`f` to :math:`s` is therefore affine, with a constant
Jacobian matrix :math:`J_s`. Its image :math:`f(s)` is a :math:`d`-dimensional
simplex embedded in :math:`\Rset^\outputDim`.

The image mesh is the collection of the image simplices. When the map is
injective, the distribution of :math:`\outputRV = f(\inputRV)` is supported by
this image mesh. Its density with respect to the :math:`d`-dimensional measure
induced on the image is

.. math::

    f_{\outputRV}(\outputReal) = \sum_{s : \outputReal \in f(s)}
    f_{\inputRV}(\inputReal_s(\outputReal))
    \sqrt{\frac{\det\left(G^{\mathrm{ref}}_s\right)}
    {\det\left(G^{\mathrm{img}}_s\right)}},

where :math:`\inputReal_s(\outputReal)` is the preimage of
:math:`\outputReal` in the reference simplex :math:`s`. The matrices
:math:`G^{\mathrm{ref}}_s` and :math:`G^{\mathrm{img}}_s` are the Gram
matrices of the edges of the reference simplex and of its image:

.. math::

    G^{\mathrm{ref}}_s = E_s^T E_s,
    \qquad
    G^{\mathrm{img}}_s = (J_s E_s)^T (J_s E_s),

with :math:`E_s` the :math:`n \times d` matrix whose columns are the edge
vectors of the reference simplex. The ratio of the determinants is the
inverse of the volume stretching factor of the affine restriction, so that
the density integrates to one over the image mesh.

The sum covers all the simplices whose image contains
:math:`\outputReal`: folding maps and self-intersecting meshes are supported.
When the Jacobian is singular on a simplex, its image has measure zero and its
contribution is discarded, which is the behaviour of the density at a critical
point. A point outside the image mesh has zero density.

When :math:`\outputDim > d`, the image is a :math:`d`-dimensional subset of
:math:`\Rset^\outputDim`; the density above is still defined with respect to
the measure induced on the image mesh, as in the general singular case of the
:class:`~openturns.experimental.PushForwardDistribution`.

The case :math:`\outputDim < d` is the co-area case: the preimage of a point is
a fibre of dimension :math:`d - \outputDim`, and the density is the integral
of the antecedent density along this fibre divided by the corresponding
Jacobian. The general co-area formula is given in the documentation of
:class:`~openturns.experimental.PushForwardDistribution`. The
:class:`~openturns.experimental.PushForwardOverMesh` class currently implements
this case only for :math:`d = 2`, ambient dimension :math:`n = 2`,
:math:`\outputDim = 1`, and a :class:`~openturns.UniformOverMesh` antecedent.

.. topic:: API:

    - See :class:`~openturns.experimental.PushForwardOverMesh`
    - See :class:`~openturns.experimental.PushForwardDistribution`
    - See :class:`~openturns.Field`
    - See :class:`~openturns.Mesh`
