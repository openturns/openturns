//                                               -*- C++ -*-
/**
 *  @brief The hypersphere S^(n-1) embedded in R^n
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
#ifndef OPENTURNS_SPHEREMANIFOLD_HXX
#define OPENTURNS_SPHEREMANIFOLD_HXX

#include "openturns/ManifoldImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SphereManifold
 *
 * The unit hypersphere S^(n-1) embedded in R^n with the metric induced
 * by the ambient Euclidean scalar product. Points are unit-norm ambient
 * vectors and tangent vectors at p satisfy v.p = 0.
 *
 * The exponential and logarithmic maps are available in closed form
 * along great circles, the geodesic distance is the angle between two
 * points and the total volume is 2.pi^(n/2)/Gamma(n/2).
 */
class OT_API OT_WARN_UNUSED SphereManifold
  : public ManifoldImplementation
{
  CLASSNAME

public:
  /** Default constructor, the 2-sphere in R^3 */
  explicit SphereManifold(const UnsignedInteger dimension = 3);

  /** Virtual constructor */
  SphereManifold * clone() const override;

  /** Exponential map: push a tangent vector at basePoint onto the sphere */
  Point expMap(const Point & basePoint,
               const Point & tangentVector) const override;

  /** Logarithmic map: pull a sphere point back to the tangent space at basePoint */
  Point logMap(const Point & basePoint,
               const Point & point) const override;

  /** Closest-point projection of an ambient vector onto the sphere */
  Point projectToManifold(const Point & point) const override;

  /** Orthogonal projection of an ambient vector onto the tangent space at basePoint */
  Point projectToTangentSpace(const Point & basePoint,
                              const Point & vector) const override;

  /** Check if an ambient vector lies on the sphere up to epsilon_ */
  Bool isOnManifold(const Point & point) const override;

  /** Geodesic distance (angle) between two sphere points */
  Scalar getGeodesicDistance(const Point & point1,
                             const Point & point2) const override;

  /** Orthonormal basis of the tangent space at basePoint as ambient vectors */
  Sample getTangentBasis(const Point & basePoint) const override;

  /** Volume density of the pushed-forward Lebesgue measure at intrinsic coordinates */
  Scalar getExpMapVolumeDensity(const Point & coordinates) const override;

  /** Total Riemannian volume (area) of the sphere */
  Scalar getVolume() const override;

  /** Comparison operator */
  using ManifoldImplementation::operator ==;
  Bool operator ==(const SphereManifold & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

}; /* class SphereManifold */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPHEREMANIFOLD_HXX */
