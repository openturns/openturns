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
#include "openturns/SphereManifold.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SphereManifold)

static const Factory<SphereManifold> Factory_SphereManifold;

/* Default constructor, the 2-sphere in R^3 */
SphereManifold::SphereManifold(const UnsignedInteger dimension)
  : ManifoldImplementation()
{
  if (dimension < 2) throw InvalidDimensionException(HERE) << "Error: the ambient dimension of a sphere manifold must be at least 2, here dimension=" << dimension;
  ambientDimension_ = dimension;
  intrinsicDimension_ = dimension - 1;
}

/* Virtual constructor */
SphereManifold * SphereManifold::clone() const
{
  return new SphereManifold(*this);
}

/* Exponential map: push a tangent vector at basePoint onto the sphere */
Point SphereManifold::expMap(const Point & basePoint,
                             const Point & tangentVector) const
{
  checkAmbientDimension(basePoint, "SphereManifold::expMap base point");
  checkAmbientDimension(tangentVector, "SphereManifold::expMap tangent vector");
  const Point p(projectToManifold(basePoint));
  const Point v(projectToTangentSpace(p, tangentVector));
  const Scalar theta = v.norm();
  if (theta <= epsilon_) return p;
  const Scalar cosTheta = std::cos(theta);
  const Scalar sinTheta = std::sin(theta);
  Point result(ambientDimension_);
  for (UnsignedInteger i = 0; i < ambientDimension_; ++i)
    result[i] = p[i] * cosTheta + v[i] * (sinTheta / theta);
  return result;
}

/* Logarithmic map: pull a sphere point back to the tangent space at basePoint */
Point SphereManifold::logMap(const Point & basePoint,
                             const Point & point) const
{
  checkAmbientDimension(basePoint, "SphereManifold::logMap base point");
  checkAmbientDimension(point, "SphereManifold::logMap point");
  const Point p(projectToManifold(basePoint));
  const Point q(projectToManifold(point));
  Scalar cosDistance = 0.0;
  for (UnsignedInteger i = 0; i < ambientDimension_; ++i)
    cosDistance += p[i] * q[i];
  // Clamp the scalar product to [-1, 1] for numerical safety
  cosDistance = std::max(-1.0, std::min(1.0, cosDistance));
  const Scalar distance = std::acos(cosDistance);
  if (distance <= epsilon_) return Point(ambientDimension_);
  const Scalar sinDistance = std::sin(distance);
  if (sinDistance <= epsilon_) throw InvalidArgumentException(HERE) << "Error: the logarithmic map is not defined at the cut locus (antipodal points) of the base point";
  Point result(ambientDimension_);
  for (UnsignedInteger i = 0; i < ambientDimension_; ++i)
    result[i] = (q[i] - cosDistance * p[i]) * (distance / sinDistance);
  return result;
}

/* Closest-point projection of an ambient vector onto the sphere */
Point SphereManifold::projectToManifold(const Point & point) const
{
  checkAmbientDimension(point, "SphereManifold::projectToManifold point");
  const Scalar norm = point.norm();
  if (norm <= epsilon_) throw InvalidArgumentException(HERE) << "Error: cannot project the null vector onto the sphere";
  return point / norm;
}

/* Orthogonal projection of an ambient vector onto the tangent space at basePoint */
Point SphereManifold::projectToTangentSpace(const Point & basePoint,
    const Point & vector) const
{
  checkAmbientDimension(basePoint, "SphereManifold::projectToTangentSpace base point");
  checkAmbientDimension(vector, "SphereManifold::projectToTangentSpace vector");
  const Point p(projectToManifold(basePoint));
  Scalar scalarProduct = 0.0;
  for (UnsignedInteger i = 0; i < ambientDimension_; ++i)
    scalarProduct += vector[i] * p[i];
  Point result(vector);
  for (UnsignedInteger i = 0; i < ambientDimension_; ++i)
    result[i] -= scalarProduct * p[i];
  return result;
}

/* Check if an ambient vector lies on the sphere up to epsilon_ */
Bool SphereManifold::isOnManifold(const Point & point) const
{
  if (point.getDimension() != ambientDimension_) return false;
  return std::abs(point.norm() - 1.0) <= epsilon_;
}

/* Geodesic distance (angle) between two sphere points */
Scalar SphereManifold::getGeodesicDistance(const Point & point1,
    const Point & point2) const
{
  checkAmbientDimension(point1, "SphereManifold::getGeodesicDistance first point");
  checkAmbientDimension(point2, "SphereManifold::getGeodesicDistance second point");
  const Point p(projectToManifold(point1));
  const Point q(projectToManifold(point2));
  Scalar cosDistance = 0.0;
  for (UnsignedInteger i = 0; i < ambientDimension_; ++i)
    cosDistance += p[i] * q[i];
  cosDistance = std::max(-1.0, std::min(1.0, cosDistance));
  return std::acos(cosDistance);
}

/* Orthonormal basis of the tangent space at basePoint, Gram-Schmidt on the canonical axes */
Sample SphereManifold::getTangentBasis(const Point & basePoint) const
{
  checkAmbientDimension(basePoint, "SphereManifold::getTangentBasis base point");
  const Point p(projectToManifold(basePoint));
  Sample basis(0, ambientDimension_);
  for (UnsignedInteger axis = 0; axis < ambientDimension_; ++axis)
  {
    Point candidate(ambientDimension_);
    candidate[axis] = 1.0;
    candidate = projectToTangentSpace(p, candidate);
    for (UnsignedInteger k = 0; k < basis.getSize(); ++k)
    {
      const Point previous(basis[k]);
      Scalar scalarProduct = 0.0;
      for (UnsignedInteger i = 0; i < ambientDimension_; ++i)
        scalarProduct += candidate[i] * previous[i];
      for (UnsignedInteger i = 0; i < ambientDimension_; ++i)
        candidate[i] -= scalarProduct * previous[i];
    }
    const Scalar norm = candidate.norm();
    if (norm <= epsilon_) continue;
    basis.add(candidate / norm);
    if (basis.getSize() == intrinsicDimension_) break;
  }
  if (basis.getSize() != intrinsicDimension_) throw InternalException(HERE) << "Error: failed to build a tangent basis at the given base point";
  return basis;
}

/* Volume density of the pushed-forward Lebesgue measure: (sin r / r)^(n-2) */
Scalar SphereManifold::getExpMapVolumeDensity(const Point & coordinates) const
{
  if (coordinates.getDimension() != intrinsicDimension_) throw InvalidArgumentException(HERE) << "Error: the coordinates must have dimension=" << intrinsicDimension_ << ", here dimension=" << coordinates.getDimension();
  const Scalar radius = coordinates.norm();
  if (radius <= epsilon_) return 1.0;
  return std::pow(std::sin(radius) / radius, static_cast<Scalar>(intrinsicDimension_ - 1));
}

/* Total Riemannian volume (area) of the sphere: 2.pi^(n/2)/Gamma(n/2) */
Scalar SphereManifold::getVolume() const
{
  const Scalar halfDimension = 0.5 * static_cast<Scalar>(ambientDimension_);
  return 2.0 * std::pow(M_PI, halfDimension) / SpecFunc::Gamma(halfDimension);
}

/* Comparison operator */
Bool SphereManifold::operator ==(const SphereManifold & other) const
{
  if (this == &other) return true;
  return ManifoldImplementation::operator==(other);
}

/* String converter */
String SphereManifold::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " ambientDimension=" << ambientDimension_
         << " intrinsicDimension=" << intrinsicDimension_
         << " epsilon=" << epsilon_;
}

String SphereManifold::__str__(const String & offset) const
{
  return OSS() << offset << getClassName()
         << "(ambientDimension=" << ambientDimension_
         << ", intrinsicDimension=" << intrinsicDimension_ << ")";
}

END_NAMESPACE_OPENTURNS
