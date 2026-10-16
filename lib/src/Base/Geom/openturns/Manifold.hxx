//                                               -*- C++ -*-
/**
 *  @brief Interface object for Riemannian manifolds embedded in R^n
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_MANIFOLD_HXX
#define OPENTURNS_MANIFOLD_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/ManifoldImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Manifold
 *
 * Interface object for Riemannian manifolds embedded in an ambient
 * Euclidean space. A Manifold object is built from a ManifoldImplementation
 * (SphereManifold, SO3Manifold) and forwards all the geometric operations
 * to it.
 */
class OT_API OT_WARN_UNUSED Manifold
  : public TypedInterfaceObject<ManifoldImplementation>
{
  CLASSNAME

public:
  /** Comparison operator */
  using TypedInterfaceObject<ManifoldImplementation>::operator ==;
  Bool operator ==(const Manifold & other) const;

  /** Default constructor, the 2-sphere in R^3 */
  Manifold();

  /** Copy-Standard constructor */
  Manifold(const ManifoldImplementation & implementation);

  /** Ambient dimension accessor */
  UnsignedInteger getAmbientDimension() const;

  /** Intrinsic dimension accessor */
  UnsignedInteger getIntrinsicDimension() const;

  /** Tolerance accessor, used by isOnManifold() and the projections */
  void setEpsilon(const Scalar epsilon);
  Scalar getEpsilon() const;

  /** Exponential map: push a tangent vector at basePoint onto the manifold */
  Point expMap(const Point & basePoint,
               const Point & tangentVector) const;

  /** Logarithmic map: pull a manifold point back to the tangent space at basePoint */
  Point logMap(const Point & basePoint,
               const Point & point) const;

  /** Closest-point projection of an ambient vector onto the manifold */
  Point projectToManifold(const Point & point) const;

  /** Orthogonal projection of an ambient vector onto the tangent space at basePoint */
  Point projectToTangentSpace(const Point & basePoint,
                              const Point & vector) const;

  /** Check if an ambient vector lies on the manifold up to the tolerance */
  Bool isOnManifold(const Point & point) const;

  /** Geodesic distance between two manifold points */
  Scalar getGeodesicDistance(const Point & point1,
                             const Point & point2) const;

  /** Orthonormal basis of the tangent space at basePoint as ambient vectors */
  Sample getTangentBasis(const Point & basePoint) const;

  /** Volume density of the pushed-forward Lebesgue measure at intrinsic coordinates */
  Scalar getExpMapVolumeDensity(const Point & coordinates) const;

  /** Total Riemannian volume of the manifold */
  Scalar getVolume() const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

}; /* class Manifold */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MANIFOLD_HXX */
