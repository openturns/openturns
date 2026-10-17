%feature("docstring") OT::SphereManifold
R"RAW(Unit hypersphere manifold.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The unit hypersphere :math:`\cS^{d-1}` embedded in :math:`\Rset^d` with
the metric induced by the ambient Euclidean scalar product. Points are
unit-norm ambient vectors and tangent vectors at :math:`\vect{p}` satisfy
:math:`\vect{v}^T \vect{p} = 0`.

The exponential and logarithmic maps are available in closed form along
great circles, the geodesic distance is the angle between two points and
the total volume is :math:`2 \pi^{d/2} / \Gamma(d/2)`.

Parameters
----------
dimension : int, :math:`\geq 2`
    Ambient dimension :math:`d`, default value is 3.

See also
--------
Manifold, SO3Manifold

Notes
-----
The following :class:`~openturns.ResourceMap` key is used:

- ``Manifold-Epsilon`` (``Scalar``, default: ``1.0e-10``): tolerance used by :meth:`isOnManifold` and the
  projections.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> manifold = otexp.SphereManifold(3)
>>> print(manifold.getIntrinsicDimension())
2
>>> print(manifold.getVolume())
12.5663706...
)RAW"
