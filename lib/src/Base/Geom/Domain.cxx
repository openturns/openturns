//                                               -*- C++ -*-
/**
 *  @brief Domain is the interface of DomainImplementation.cxx
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Domain);

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
  // Nothing to do
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

/* Check if the given point is inside of the closed interval */
Bool Domain::numericallyContains(const Point & point) const
{
  return getImplementation()->numericallyContains(point);
}

/* Check if the domain is empty, i.e if its numerical volume is zero */
Bool Domain::isEmpty() const
{
  return getImplementation()->isEmpty();
}

/* Check if the domain is numerically empty, i.e if its numerical volume is zero */
Bool Domain::isNumericallyEmpty() const
{
  return getImplementation()->isNumericallyEmpty();
}

/* Get the volume of the domain */
Scalar Domain::getVolume() const
{
  return getImplementation()->getVolume();
}

/* Get the numerical volume of the domain */
Scalar Domain::getNumericalVolume() const
{
  return getImplementation()->getNumericalVolume();
}

/* Lower bound of the bounding box */
Point Domain::getLowerBound() const
{
  return getImplementation()->getLowerBound();
}

/* Upper bound of the bounding box */
Point Domain::getUpperBound() const
{
  return getImplementation()->getUpperBound();
}

END_NAMESPACE_OPENTURNS
