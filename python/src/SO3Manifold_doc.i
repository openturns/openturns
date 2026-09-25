%feature("docstring") OT::SO3Manifold
R"RAW(Rotation group manifold.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The special orthogonal group :math:`SO(3)` of 3D rotations, embedded in
:math:`\Rset^{3 \times 3}` as flattened row-major 3x3 matrices (ambient
dimension 9, intrinsic dimension 3). This layout matches the points of
the :class:`~openturns.experimental.MatrixFisher` distribution.

The Riemannian metric is the bi-invariant metric induced by the
axis-angle identification of the Lie algebra :math:`\mathfrak{so}(3)`
with :math:`\Rset^3`: the geodesic distance between two rotations is the
angle of the relative rotation and the total volume is :math:`8 \pi^2`.
The exponential and logarithmic maps are available in closed form
through the Rodrigues formulas.

See also
--------
Manifold, SphereManifold

Notes
-----
The following :class:`~openturns.ResourceMap` key is used:

- ``Manifold-Epsilon`` (``Scalar``, default: ``1e-10``): tolerance used by :meth:`isOnManifold` and the
  projections.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> manifold = otexp.SO3Manifold()
>>> print(manifold.getIntrinsicDimension())
3
>>> print(manifold.getVolume())
78.9568352...
)RAW"
