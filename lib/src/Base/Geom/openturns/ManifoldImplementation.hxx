//                                               -*- C++ -*-
/**
 *  @brief Abstract class for Riemannian manifolds embedded in R^n
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
#ifndef OPENTURNS_MANIFOLDIMPLEMENTATION_HXX
#define OPENTURNS_MANIFOLDIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ManifoldImplementation
 *
 * Abstract base class for Riemannian manifolds embedded in an ambient
 * Euclidean space. Points of the manifold and tangent vectors are both
 * stored as ambient vectors (Point of dimension equal to the ambient
 * dimension); tangent vectors satisfy the linearized constraints at
 * their base point.
 *
 * The exponential map expMap() pushes a tangent vector onto the manifold
 * and the logarithmic map logMap() pulls a manifold point back to the
 * tangent space. Together with projectToManifold() and
 * projectToTangentSpace() they provide the pushforward/pullback
 * machinery used by manifold-valued distributions.
 */
class OT_API OT_WARN_UNUSED ManifoldImplementation
  : public PersistentObject
{
  CLASSNAME

public:
  /** Virtual constructor */
  ManifoldImplementation * clone() const override = 0;

  /** Ambient dimension accessor */
  UnsignedInteger getAmbientDimension() const;

  /** Intrinsic dimension accessor */
  UnsignedInteger getIntrinsicDimension() const;

  /** Tolerance accessor, used by isOnManifold() and the projections */
  void setEpsilon(const Scalar epsilon);
  Scalar getEpsilon() const;

  /** Exponential map: push a tangent vector at basePoint onto the manifold */
  virtual Point expMap(const Point & basePoint,
                       const Point & tangentVector) const = 0;

  /** Logarithmic map: pull a manifold point back to the tangent space at basePoint */
  virtual Point logMap(const Point & basePoint,
                       const Point & point) const = 0;

  /** Closest-point projection of an ambient vector onto the manifold */
  virtual Point projectToManifold(const Point & point) const = 0;

  /** Orthogonal projection of an ambient vector onto the tangent space at basePoint */
  virtual Point projectToTangentSpace(const Point & basePoint,
                                      const Point & vector) const = 0;

  /** Check if an ambient vector lies on the manifold up to epsilon_ */
  virtual Bool isOnManifold(const Point & point) const = 0;

  /** Geodesic distance between two manifold points */
  virtual Scalar getGeodesicDistance(const Point & point1,
                                     const Point & point2) const = 0;

  /** Orthonormal basis of the tangent space at basePoint as ambient vectors */
  virtual Sample getTangentBasis(const Point & basePoint) const = 0;

  /** Volume density of the pushed-forward Lebesgue measure at intrinsic coordinates */
  virtual Scalar getExpMapVolumeDensity(const Point & coordinates) const = 0;

  /** Total Riemannian volume of the manifold */
  virtual Scalar getVolume() const = 0;

  /** Comparison operator */
  using PersistentObject::operator ==;
  Bool operator ==(const ManifoldImplementation & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Default constructor, for subclasses and persistence only */
  ManifoldImplementation();

  /** Check that a point has the ambient dimension */
  void checkAmbientDimension(const Point & point,
                             const String & where) const;

  /** Ambient dimension */
  UnsignedInteger ambientDimension_;

  /** Intrinsic dimension */
  UnsignedInteger intrinsicDimension_;

  /** Tolerance for manifold membership and projections */
  Scalar epsilon_;

}; /* class Manifold */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MANIFOLDIMPLEMENTATION_HXX */
