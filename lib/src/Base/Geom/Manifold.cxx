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
#include "openturns/Manifold.hxx"
#include "openturns/SphereManifold.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Manifold)

/* Comparison operator */
Bool Manifold::operator ==(const Manifold & other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}

/* Default constructor, the 2-sphere in R^3 */
Manifold::Manifold()
  : TypedInterfaceObject<ManifoldImplementation>(new SphereManifold())
{
  // Nothing to do
}

/* Copy-Standard constructor */
Manifold::Manifold(const ManifoldImplementation & implementation)
  : TypedInterfaceObject<ManifoldImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Ambient dimension accessor */
UnsignedInteger Manifold::getAmbientDimension() const
{
  return getImplementation()->getAmbientDimension();
}

/* Intrinsic dimension accessor */
UnsignedInteger Manifold::getIntrinsicDimension() const
{
  return getImplementation()->getIntrinsicDimension();
}

/* Tolerance accessor */
void Manifold::setEpsilon(const Scalar epsilon)
{
  copyOnWrite();
  getImplementation()->setEpsilon(epsilon);
}

Scalar Manifold::getEpsilon() const
{
  return getImplementation()->getEpsilon();
}

/* Exponential map: push a tangent vector at basePoint onto the manifold */
Point Manifold::expMap(const Point & basePoint,
                       const Point & tangentVector) const
{
  return getImplementation()->expMap(basePoint, tangentVector);
}

/* Logarithmic map: pull a manifold point back to the tangent space at basePoint */
Point Manifold::logMap(const Point & basePoint,
                       const Point & point) const
{
  return getImplementation()->logMap(basePoint, point);
}

/* Closest-point projection of an ambient vector onto the manifold */
Point Manifold::projectToManifold(const Point & point) const
{
  return getImplementation()->projectToManifold(point);
}

/* Orthogonal projection of an ambient vector onto the tangent space at basePoint */
Point Manifold::projectToTangentSpace(const Point & basePoint,
                                      const Point & vector) const
{
  return getImplementation()->projectToTangentSpace(basePoint, vector);
}

/* Check if an ambient vector lies on the manifold up to the tolerance */
Bool Manifold::isOnManifold(const Point & point) const
{
  return getImplementation()->isOnManifold(point);
}

/* Geodesic distance between two manifold points */
Scalar Manifold::getGeodesicDistance(const Point & point1,
                                     const Point & point2) const
{
  return getImplementation()->getGeodesicDistance(point1, point2);
}

/* Orthonormal basis of the tangent space at basePoint as ambient vectors */
Sample Manifold::getTangentBasis(const Point & basePoint) const
{
  return getImplementation()->getTangentBasis(basePoint);
}

/* Volume density of the pushed-forward Lebesgue measure at intrinsic coordinates */
Scalar Manifold::getExpMapVolumeDensity(const Point & coordinates) const
{
  return getImplementation()->getExpMapVolumeDensity(coordinates);
}

/* Total Riemannian volume of the manifold */
Scalar Manifold::getVolume() const
{
  return getImplementation()->getVolume();
}

/* String converter */
String Manifold::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " implementation=" << getImplementation()->__repr__();
}

String Manifold::__str__(const String & offset) const
{
  return OSS() << offset << getImplementation()->__str__();
}

END_NAMESPACE_OPENTURNS
