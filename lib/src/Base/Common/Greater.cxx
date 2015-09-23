//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all Greater
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "Greater.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Greater);

static const Factory<Greater> RegisteredFactory;

/* Default constructor */
Greater::Greater()
  : ComparisonOperatorImplementation()
{
  // Nothing to do
}

/* String converter */
String Greater::__repr__() const
{
  return OSS() << "class=" << Greater::GetClassName()
         << " name=" << getName();
}


/* Here is the interface that all derived class must implement */

/* Virtual constructor */
Greater * Greater::clone() const
{
  return new Greater(*this);
}

/* Return true if comparison succeeds */
Bool Greater::operator() (const NumericalScalar a,
                          const NumericalScalar b) const
{
  return (a > b);
}

/* Method save() stores the object through the StorageManager */
void Greater::save(Advocate & adv) const
{
  ComparisonOperatorImplementation::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void Greater::load(Advocate & adv)
{
  ComparisonOperatorImplementation::load(adv);
}





END_NAMESPACE_OPENTURNS
