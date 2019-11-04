//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all Equal
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
#include "openturns/Equal.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Equal)

static const Factory<Equal> Factory_Equal;

/* Default constructor */
Equal::Equal()
  : ComparisonOperatorImplementation()
{
  // Nothing to do
}

/* String converter */
String Equal::__repr__() const
{
  return OSS() << "class=" << Equal::GetClassName()
         << " name=" << getName();
}

String Equal::__str__(const String & offset) const
{
  return "=";
}

/* Here is the interface that all derived class must implement */

/* Virtual constructor */
Equal * Equal::clone() const
{
  return new Equal(*this);
}

/* Return true if comparison succeeds */
Bool Equal::operator() (const Scalar a,
                        const Scalar b) const
{
  return (a == b);
}

/* Method save() stores the object through the StorageManager */
void Equal::save(Advocate & adv) const
{
  ComparisonOperatorImplementation::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void Equal::load(Advocate & adv)
{
  ComparisonOperatorImplementation::load(adv);
}




END_NAMESPACE_OPENTURNS
