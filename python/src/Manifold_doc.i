%feature("docstring") OT::Manifold
R"RAW(Interface object for Riemannian manifolds.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

A Manifold object is built from a manifold implementation and forwards
all the geometric operations to it:

>>> import openturns.experimental as otexp
>>> manifold = otexp.Manifold(otexp.SphereManifold(3))
>>> print(manifold.getIntrinsicDimension())
2

See also
--------
SphereManifold, SO3Manifold

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``Manifold-Epsilon``: tolerance used by :meth:`isOnManifold` and the
  projections, default value ``1e-10``.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Manifold::getAmbientDimension
R"RAW(Accessor to the ambient dimension.

Returns
-------
ambientDimension : int
    Dimension of the ambient Euclidean space.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Manifold::getIntrinsicDimension
R"RAW(Accessor to the intrinsic dimension.

Returns
-------
intrinsicDimension : int
    Dimension of the manifold as a Riemannian manifold.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Manifold::getEpsilon
R"RAW(Accessor to the tolerance.

Returns
-------
epsilon : float
    Tolerance used by :meth:`isOnManifold` and the projections.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Manifold::setEpsilon
R"RAW(Accessor to the tolerance.

Parameters
----------
epsilon : float, :math:`> 0`
    Tolerance used by :meth:`isOnManifold` and the projections.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Manifold::expMap
R"RAW(Exponential map at a base point.

Parameters
----------
basePoint : sequence of float
    Base point on the manifold.
tangentVector : sequence of float
    Tangent vector at the base point.

Returns
-------
point : :class:`~openturns.Point`
    Image of the tangent vector on the manifold.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Manifold::logMap
R"RAW(Logarithmic map at a base point.

Parameters
----------
basePoint : sequence of float
    Base point on the manifold.
point : sequence of float
    Point of the manifold.

Returns
-------
tangentVector : :class:`~openturns.Point`
    Preimage of the point in the tangent space at the base point.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Manifold::projectToManifold
R"RAW(Closest-point projection onto the manifold.

Parameters
----------
point : sequence of float
    Ambient vector.

Returns
-------
projectedPoint : :class:`~openturns.Point`
    Projection of the ambient vector onto the manifold.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Manifold::projectToTangentSpace
R"RAW(Orthogonal projection onto the tangent space at a base point.

Parameters
----------
basePoint : sequence of float
    Base point on the manifold.
vector : sequence of float
    Ambient vector.

Returns
-------
tangentVector : :class:`~openturns.Point`
    Projection of the ambient vector onto the tangent space.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Manifold::isOnManifold
R"RAW(Check manifold membership up to the tolerance.

Parameters
----------
point : sequence of float
    Ambient vector.

Returns
-------
isOnManifold : bool
    Whether the ambient vector lies on the manifold.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Manifold::getGeodesicDistance
R"RAW(Geodesic distance between two manifold points.

Parameters
----------
point1 : sequence of float
    First point of the manifold.
point2 : sequence of float
    Second point of the manifold.

Returns
-------
distance : float
    Geodesic distance between the two points.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Manifold::getTangentBasis
R"RAW(Orthonormal basis of the tangent space at a base point.

Parameters
----------
basePoint : sequence of float
    Base point on the manifold.

Returns
-------
basis : :class:`~openturns.Sample`
    Orthonormal ambient vectors spanning the tangent space.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Manifold::getExpMapVolumeDensity
R"RAW(Volume density of the pushed-forward Lebesgue measure.

Parameters
----------
coordinates : sequence of float
    Intrinsic coordinates in the tangent basis.

Returns
-------
density : float, :math:`> 0`
    Volume density at the given coordinates.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Manifold::getVolume
R"RAW(Total Riemannian volume of the manifold.

Returns
-------
volume : float
    Total volume of the manifold for its Riemannian metric.)RAW"
