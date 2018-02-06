//                                               -*- C++ -*-
/**
 *  @brief Domain is the interface of DomainImplementation.cxx
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/Domain.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Interval.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Domain)

/* Default constructor */
Domain::Domain():
  TypedInterfaceObject<DomainImplementation>(new Interval())
{
  // Nothing to do
}

Domain::Domain(const Point & a,
               const Point & b):
  TypedInterfaceObject<DomainImplementation>(new Interval(a, b))
{
  LOGWARN(OSS() << "Domain::Domain(const Point & a, const Point & b) is deprecated in favor of Interval constructor.");
}

/* Default constructor */
Domain::Domain(const DomainImplementation & implementation):
  TypedInterfaceObject<DomainImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Dimension accessor */
UnsignedInteger Domain::getDimension() const
{
  return getImplementation()->getDimension();
}

/* Check if the given point is inside of the domain */
Bool Domain::contains(const Point & point) const
{
  return getImplementation()->contains(point);
}

/* Check if the given points are inside of the domain */
Domain::BoolCollection Domain::contains(const Sample & sample) const
{
  return getImplementation()->contains(sample);
}

/* Check if the given point is inside of the closed interval */
Bool Domain::numericallyContains(const Point & point) const
{
  LOGWARN(OSS() << "Domain::numericallyContains is deprecated in favor of specific method in classes derived from DomainImplementation.");
  return getImplementation()->numericallyContains(point);
}

/* Check if the domain is empty, i.e if its numerical volume is zero */
Bool Domain::isEmpty() const
{
  LOGWARN(OSS() << "Domain::isEmpty is deprecated in favor of specific method in classes derived from DomainImplementation.");
  return getImplementation()->isEmpty();
}

/* Check if the domain is numerically empty, i.e if its numerical volume is zero */
Bool Domain::isNumericallyEmpty() const
{
  LOGWARN(OSS() << "Domain::isNumericallyEmpty is deprecated in favor of specific method in classes derived from DomainImplementation.");
  return getImplementation()->isNumericallyEmpty();
}

/* Get the volume of the domain */
Scalar Domain::getVolume() const
{
  LOGWARN(OSS() << "Domain::getVolume is deprecated in favor of specific method in classes derived from DomainImplementation.");
  return getImplementation()->getVolume();
}

/* Get the numerical volume of the domain */
Scalar Domain::getNumericalVolume() const
{
  LOGWARN(OSS() << "Domain::getNumericalVolume is deprecated in favor of specific method in classes derived from DomainImplementation.");
  return getImplementation()->getNumericalVolume();
}

/* Lower bound of the bounding box */
Point Domain::getLowerBound() const
{
  LOGWARN(OSS() << "Domain::getLowerBound is deprecated in favor of specific method in classes derived from DomainImplementation.");
  return getImplementation()->getLowerBound();
}

/* Upper bound of the bounding box */
Point Domain::getUpperBound() const
{
  LOGWARN(OSS() << "Domain::getUpperBound is deprecated in favor of specific method in classes derived from DomainImplementation.");
  return getImplementation()->getUpperBound();
}

END_NAMESPACE_OPENTURNS
