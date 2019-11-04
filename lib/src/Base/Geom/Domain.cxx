//                                               -*- C++ -*-
/**
 *  @brief Domain is the interface of DomainImplementation.cxx
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

String Domain::__repr__() const
{
  return getImplementation()->__repr__();
}

END_NAMESPACE_OPENTURNS
