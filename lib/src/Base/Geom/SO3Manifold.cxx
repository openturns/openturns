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
#include "openturns/SO3Manifold.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SO3Manifold)

static const Factory<SO3Manifold> Factory_SO3Manifold;

namespace
{

/* Convert a flattened row-major 9-vector to a 3x3 matrix */
SquareMatrix PointToRotation(const Point & point)
{
  SquareMatrix rotation(3);
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = 0; j < 3; ++j)
      rotation(i, j) = point[i * 3 + j];
  return rotation;
}

/* Convert a 3x3 matrix to a flattened row-major 9-vector */
Point RotationToPoint(const SquareMatrix & rotation)
{
  Point point(9);
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = 0; j < 3; ++j)
      point[i * 3 + j] = rotation(i, j);
  return point;
}

/* Skew-symmetric matrix of an axis-angle vector */
SquareMatrix SkewFromVector(const Point & axisAngle)
{
  SquareMatrix skew(3);
  skew(0, 0) = 0.0;
  skew(0, 1) = -axisAngle[2];
  skew(0, 2) = axisAngle[1];
  skew(1, 0) = axisAngle[2];
  skew(1, 1) = 0.0;
  skew(1, 2) = -axisAngle[0];
  skew(2, 0) = -axisAngle[1];
  skew(2, 1) = axisAngle[0];
  skew(2, 2) = 0.0;
  return skew;
}

/* Axis-angle vector of a skew-symmetric matrix */
Point VectorFromSkew(const SquareMatrix & skew)
{
  Point axisAngle(3);
  axisAngle[0] = skew(2, 1);
  axisAngle[1] = skew(0, 2);
  axisAngle[2] = skew(1, 0);
  return axisAngle;
}

/* Matrix exponential of a skew-symmetric matrix (Rodrigues formula) */
SquareMatrix ExpSkew(const Point & axisAngle,
                     const Scalar epsilon)
{
  const Scalar theta = axisAngle.norm();
  SquareMatrix exponential(IdentityMatrix(3));
  if (theta <= epsilon) return exponential;
  const SquareMatrix skew(SkewFromVector(axisAngle));
  const SquareMatrix skew2(skew * skew);
  const Scalar sinTheta = std::sin(theta);
  const Scalar cosTheta = std::cos(theta);
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = 0; j < 3; ++j)
      exponential(i, j) += skew(i, j) * (sinTheta / theta) + skew2(i, j) * ((1.0 - cosTheta) / (theta * theta));
  return exponential;
}

/* Axis-angle vector of a near-rotation matrix, throws at the cut locus (angle pi) */
Point LogRotation(const SquareMatrix & rotation,
                  const Scalar epsilon)
{
  const Scalar cosTheta = std::max(-1.0, std::min(1.0, 0.5 * (rotation(0, 0) + rotation(1, 1) + rotation(2, 2) - 1.0)));
  const SquareMatrix skewPart((rotation - rotation.transpose()) * 0.5);
  const Point vee(VectorFromSkew(skewPart));
  const Scalar sinTheta = vee.norm();
  const Scalar theta = std::atan2(sinTheta, cosTheta);
  if (theta <= epsilon) return Point(3);
  if (M_PI - theta <= epsilon) throw InvalidArgumentException(HERE) << "Error: the logarithmic map is not defined at the cut locus (half-turn rotations) of the base point";
  Point axisAngle(vee);
  for (UnsignedInteger i = 0; i < 3; ++i)
    axisAngle[i] *= theta / sinTheta;
  return axisAngle;
}

} /* anonymous namespace */

/* Default constructor */
SO3Manifold::SO3Manifold()
  : ManifoldImplementation()
{
  ambientDimension_ = 9;
  intrinsicDimension_ = 3;
}

/* Virtual constructor */
SO3Manifold * SO3Manifold::clone() const
{
  return new SO3Manifold(*this);
}

/* Exponential map: push a tangent vector at basePoint onto SO(3) */
Point SO3Manifold::expMap(const Point & basePoint,
                          const Point & tangentVector) const
{
  checkAmbientDimension(basePoint, "SO3Manifold::expMap base point");
  checkAmbientDimension(tangentVector, "SO3Manifold::expMap tangent vector");
  const SquareMatrix base(PointToRotation(projectToManifold(basePoint)));
  const SquareMatrix tangent(PointToRotation(tangentVector));
  // Pull the increment back to the Lie algebra and keep its skew part
  const SquareMatrix pulled(base.transpose() * tangent);
  const SquareMatrix skew((pulled - pulled.transpose()) * 0.5);
  const Point axisAngle(VectorFromSkew(skew));
  const SquareMatrix rotation(base * ExpSkew(axisAngle, epsilon_));
  return RotationToPoint(rotation);
}

/* Logarithmic map: pull a rotation back to the tangent space at basePoint */
Point SO3Manifold::logMap(const Point & basePoint,
                          const Point & point) const
{
  checkAmbientDimension(basePoint, "SO3Manifold::logMap base point");
  checkAmbientDimension(point, "SO3Manifold::logMap point");
  const SquareMatrix base(PointToRotation(projectToManifold(basePoint)));
  const SquareMatrix target(PointToRotation(projectToManifold(point)));
  const SquareMatrix relative(base.transpose() * target);
  const Point axisAngle(LogRotation(relative, epsilon_));
  const SquareMatrix tangent(base * SkewFromVector(axisAngle));
  return RotationToPoint(tangent);
}

/* Closest-point (polar factor) projection of a 3x3 matrix onto SO(3) */
Point SO3Manifold::projectToManifold(const Point & point) const
{
  checkAmbientDimension(point, "SO3Manifold::projectToManifold point");
  Matrix matrix(3, 3);
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = 0; j < 3; ++j)
      matrix(i, j) = point[i * 3 + j];
  Matrix left(3, 3);
  Matrix rightT(3, 3);
  matrix.computeSVD(left, rightT);
  Matrix projected(left * rightT);
  const SquareMatrix candidate(projected.getImplementation());
  if (candidate.computeDeterminant() < 0.0)
  {
    // Flip the last column of the left factor for a proper rotation
    for (UnsignedInteger i = 0; i < 3; ++i)
      left(i, 2) = -left(i, 2);
    projected = left * rightT;
  }
  const SquareMatrix rotation(projected.getImplementation());
  return RotationToPoint(rotation);
}

/* Orthogonal projection of a 3x3 matrix onto the tangent space at basePoint */
Point SO3Manifold::projectToTangentSpace(const Point & basePoint,
    const Point & vector) const
{
  checkAmbientDimension(basePoint, "SO3Manifold::projectToTangentSpace base point");
  checkAmbientDimension(vector, "SO3Manifold::projectToTangentSpace vector");
  const SquareMatrix base(PointToRotation(projectToManifold(basePoint)));
  const SquareMatrix ambient(PointToRotation(vector));
  const SquareMatrix pulled(base.transpose() * ambient);
  const SquareMatrix skew((pulled - pulled.transpose()) * 0.5);
  const SquareMatrix tangent(base * skew);
  return RotationToPoint(tangent);
}

/* Check if a flattened 3x3 matrix is a rotation up to epsilon_ */
Bool SO3Manifold::isOnManifold(const Point & point) const
{
  if (point.getDimension() != ambientDimension_) return false;
  const SquareMatrix rotation(PointToRotation(point));
  const SquareMatrix identity(rotation * rotation.transpose());
  if ((identity - IdentityMatrix(3)).frobeniusNorm() > epsilon_) return false;
  if (std::abs(rotation.computeDeterminant() - 1.0) > epsilon_) return false;
  return true;
}

/* Geodesic distance (rotation angle) between two rotations */
Scalar SO3Manifold::getGeodesicDistance(const Point & point1,
                                        const Point & point2) const
{
  checkAmbientDimension(point1, "SO3Manifold::getGeodesicDistance first point");
  checkAmbientDimension(point2, "SO3Manifold::getGeodesicDistance second point");
  const SquareMatrix first(PointToRotation(projectToManifold(point1)));
  const SquareMatrix second(PointToRotation(projectToManifold(point2)));
  const SquareMatrix relative(first.transpose() * second);
  const Scalar cosTheta = std::max(-1.0, std::min(1.0, 0.5 * (relative(0, 0) + relative(1, 1) + relative(2, 2) - 1.0)));
  const SquareMatrix skewPart((relative - relative.transpose()) * 0.5);
  const Scalar sinTheta = VectorFromSkew(skewPart).norm();
  return std::atan2(sinTheta, cosTheta);
}

/* Orthonormal basis of the tangent space at basePoint: scaled skew-symmetric axis matrices */
Sample SO3Manifold::getTangentBasis(const Point & basePoint) const
{
  checkAmbientDimension(basePoint, "SO3Manifold::getTangentBasis base point");
  const SquareMatrix base(PointToRotation(projectToManifold(basePoint)));
  // The matrices base * skew(e_k) / sqrt(2) are orthonormal for the ambient Frobenius product
  const Scalar normalization = 1.0 / std::sqrt(2.0);
  Sample basis(0, ambientDimension_);
  for (UnsignedInteger axis = 0; axis < 3; ++axis)
  {
    Point axisAngle(3);
    axisAngle[axis] = 1.0;
    const SquareMatrix direction(base * SkewFromVector(axisAngle) * normalization);
    basis.add(RotationToPoint(direction));
  }
  return basis;
}

/* Volume density of the pushed-forward Lebesgue measure: (sin(theta/2) / (theta/2))^2 */
Scalar SO3Manifold::getExpMapVolumeDensity(const Point & coordinates) const
{
  if (coordinates.getDimension() != intrinsicDimension_) throw InvalidArgumentException(HERE) << "Error: the coordinates must have dimension=" << intrinsicDimension_ << ", here dimension=" << coordinates.getDimension();
  // The intrinsic coordinates relate to the rotation angle by |coordinates| = sqrt(2) * theta
  const Scalar theta = coordinates.norm() / std::sqrt(2.0);
  if (theta <= epsilon_) return 1.0;
  const Scalar halfTheta = 0.5 * theta;
  const Scalar ratio = std::sin(halfTheta) / halfTheta;
  return ratio * ratio;
}

/* Total Riemannian volume (8.pi^2) of SO(3) */
Scalar SO3Manifold::getVolume() const
{
  return 8.0 * M_PI * M_PI;
}

/* Comparison operator */
Bool SO3Manifold::operator ==(const SO3Manifold & other) const
{
  if (this == &other) return true;
  return ManifoldImplementation::operator==(other);
}

/* String converter */
String SO3Manifold::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " ambientDimension=" << ambientDimension_
         << " intrinsicDimension=" << intrinsicDimension_
         << " epsilon=" << epsilon_;
}

String SO3Manifold::__str__(const String & offset) const
{
  return OSS() << offset << getClassName()
         << "(ambientDimension=" << ambientDimension_
         << ", intrinsicDimension=" << intrinsicDimension_ << ")";
}

END_NAMESPACE_OPENTURNS
