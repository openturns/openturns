%feature("docstring") OT::ManifoldImplementation
R"RAW(Abstract Riemannian manifold embedded in an ambient Euclidean space.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Points of the manifold and tangent vectors are both stored as ambient
vectors, i.e. :class:`~openturns.Point` objects of dimension equal to the
ambient dimension. A tangent vector at a base point satisfies the
linearized constraints of the manifold at that point.

The exponential map :meth:`expMap` pushes a tangent vector onto the
manifold and the logarithmic map :meth:`logMap` pulls a manifold point
back to the tangent space. Together with :meth:`projectToManifold` and
:meth:`projectToTangentSpace` they provide the pushforward/pullback
machinery used by manifold-valued distributions.

This class cannot be instantiated directly, use :class:`~openturns.experimental.SphereManifold`
or :class:`~openturns.experimental.SO3Manifold` instead.

See also
--------
SphereManifold, SO3Manifold

Notes
-----
The following :class:`~openturns.ResourceMap` key is used:

- ``Manifold-Epsilon`` (``Scalar``, default: ``1e-10``): tolerance used by :meth:`isOnManifold` and the
  projections.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ManifoldImplementation::getAmbientDimension
R"RAW(Accessor to the ambient dimension.

Returns
-------
ambientDimension : int
    Dimension of the ambient Euclidean space.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ManifoldImplementation::getIntrinsicDimension
R"RAW(Accessor to the intrinsic dimension.

Returns
-------
intrinsicDimension : int
    Dimension of the manifold as a Riemannian manifold.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ManifoldImplementation::getEpsilon
R"RAW(Accessor to the tolerance.

Returns
-------
epsilon : float
    Tolerance used by :meth:`isOnManifold` and the projections.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ManifoldImplementation::setEpsilon
R"RAW(Accessor to the tolerance.

Parameters
----------
epsilon : float, :math:`> 0`
    Tolerance used by :meth:`isOnManifold` and the projections.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ManifoldImplementation::expMap
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

%feature("docstring") OT::ManifoldImplementation::logMap
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

%feature("docstring") OT::ManifoldImplementation::projectToManifold
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

%feature("docstring") OT::ManifoldImplementation::projectToTangentSpace
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

%feature("docstring") OT::ManifoldImplementation::isOnManifold
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

%feature("docstring") OT::ManifoldImplementation::getGeodesicDistance
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

%feature("docstring") OT::ManifoldImplementation::getVolume
R"RAW(Total Riemannian volume of the manifold.

Returns
-------
volume : float
    Total volume of the manifold for its Riemannian metric.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ManifoldImplementation::getTangentBasis
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

%feature("docstring") OT::ManifoldImplementation::getExpMapVolumeDensity
R"RAW(Volume density of the pushed-forward Lebesgue measure.

The exponential map pushes the Lebesgue measure of the intrinsic
coordinate space onto the manifold; this density is its Radon-Nikodym
derivative with respect to the Riemannian volume, evaluated at the
given intrinsic coordinates in the :meth:`getTangentBasis` basis.

Parameters
----------
coordinates : sequence of float
    Intrinsic coordinates in the tangent basis.

Returns
-------
density : float, :math:`> 0`
    Volume density at the given coordinates.)RAW"
