//                                               -*- C++ -*-
/**
 *  @brief The rotation group SO(3) embedded in R^(3x3)
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
#ifndef OPENTURNS_SO3MANIFOLD_HXX
#define OPENTURNS_SO3MANIFOLD_HXX

#include "openturns/ManifoldImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SO3Manifold
 *
 * The special orthogonal group SO(3) of 3D rotations, embedded in R^(3x3)
 * as flattened row-major 3x3 matrices (ambient dimension 9, intrinsic
 * dimension 3). This layout matches the points of the MatrixFisher
 * distribution.
 *
 * The Riemannian metric is the bi-invariant metric induced by the
 * axis-angle identification of the Lie algebra so(3) with R^3: the
 * geodesic distance between two rotations is the angle of the relative
 * rotation and the total volume is 8.pi^2. The exponential and logarithmic
 * maps are available in closed form through the Rodrigues formulas.
 */
class OT_API OT_WARN_UNUSED SO3Manifold
  : public ManifoldImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  SO3Manifold();

  /** Virtual constructor */
  SO3Manifold * clone() const override;

  /** Exponential map: push a tangent vector at basePoint onto SO(3) */
  Point expMap(const Point & basePoint,
               const Point & tangentVector) const override;

  /** Logarithmic map: pull a rotation back to the tangent space at basePoint */
  Point logMap(const Point & basePoint,
               const Point & point) const override;

  /** Closest-point (polar factor) projection of a 3x3 matrix onto SO(3) */
  Point projectToManifold(const Point & point) const override;

  /** Orthogonal projection of a 3x3 matrix onto the tangent space at basePoint */
  Point projectToTangentSpace(const Point & basePoint,
                              const Point & vector) const override;

  /** Check if a flattened 3x3 matrix is a rotation up to epsilon_ */
  Bool isOnManifold(const Point & point) const override;

  /** Geodesic distance (rotation angle) between two rotations */
  Scalar getGeodesicDistance(const Point & point1,
                             const Point & point2) const override;

  /** Orthonormal basis of the tangent space at basePoint as ambient vectors */
  Sample getTangentBasis(const Point & basePoint) const override;

  /** Volume density of the pushed-forward Lebesgue measure at intrinsic coordinates */
  Scalar getExpMapVolumeDensity(const Point & coordinates) const override;

  /** Total Riemannian volume (8.pi^2) of SO(3) */
  Scalar getVolume() const override;

  /** Comparison operator */
  using ManifoldImplementation::operator ==;
  Bool operator ==(const SO3Manifold & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

}; /* class SO3Manifold */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SO3MANIFOLD_HXX */
