%feature("docstring") OT::SO3Manifold
R"RAW(Rotation group manifold.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The special orthogonal group :math:`SO(3)` of 3D rotations, embedded in
:math:`\Rset^{3 \times 3}` as flattened row-major 3x3 matrices (ambient
dimension 9, intrinsic dimension 3). This layout matches the points of
the :class:`~openturns.experimental.MatrixFisher` distribution.

The Riemannian metric is the bi-invariant metric induced by the ambient
Frobenius product on :math:`\Rset^{3 \times 3}`: the tangent basis is
Frobenius-orthonormal, the geodesic distance between two rotations is
:math:`\sqrt{2}` times the angle of the relative rotation and the total
volume is :math:`16 \sqrt{2} \pi^2`.
The exponential and logarithmic maps are available in closed form
through the Rodrigues formulas (Rodrigues, O. (1840). Des lois geometriques
qui regissent les deplacements d'un systeme solide dans l'espace, et de la
variation des coordonnees provenant de ces deplacements consideres
independamment des causes qui peuvent les produire. Journal de
Mathematiques Pures et Appliquees, 5, 380-440: rotation of a vector about an
axis by an angle expressed with the skew-symmetric matrix exponential. Murray,
R. M., Li, Z., Sastry, S. S. (1994). A Mathematical Introduction to Robotic
Manipulation. CRC Press, Chapter 2: exponential and logarithmic maps on SO(3)
and their axis-angle parametrization used here.

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
223.3236543...
)RAW"
