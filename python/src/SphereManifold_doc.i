%feature("docstring") OT::SphereManifold
R"RAW(Unit hypersphere manifold.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The unit hypersphere :math:`\cS^{n-1}` embedded in :math:`\Rset^n` with
the metric induced by the ambient Euclidean scalar product. Points are
unit-norm ambient vectors and tangent vectors at :math:`p` satisfy
:math:`v^T p = 0`.

The exponential and logarithmic maps are available in closed form along
great circles, the geodesic distance is the angle between two points and
the total volume is :math:`2 \pi^{n/2} / \Gamma(n/2)`.

Parameters
----------
dimension : int, :math:`\geq 2`
    Ambient dimension :math:`n`, default value is 3.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> manifold = otexp.SphereManifold(3)
>>> print(manifold.getIntrinsicDimension())
2
>>> print(manifold.getVolume())
12.566370614359172...

See also
--------
Manifold, SO3Manifold

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``Manifold-Epsilon``: tolerance used by :meth:`isOnManifold` and the
  projections, default value ``1e-10``.
)RAW"
