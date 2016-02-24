//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all ComparisonOperatorImplementation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ComparisonOperatorImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ComparisonOperatorImplementation);

static const Factory<ComparisonOperatorImplementation> RegisteredFactory;

/* Default constructor */
ComparisonOperatorImplementation::ComparisonOperatorImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Virtual constructor */
ComparisonOperatorImplementation * ComparisonOperatorImplementation::clone() const
{
  return new ComparisonOperatorImplementation(*this);
}

/* Evaluation operator */
Bool ComparisonOperatorImplementation::operator() (const NumericalScalar a,
    const NumericalScalar b) const
{
  throw NotYetImplementedException(HERE) << "In ComparisonOperatorImplementation::operator() (const NumericalScalar a, const NumericalScalar b) const";
}

/* String converter */
String ComparisonOperatorImplementation::__repr__() const
{
  return OSS() << "class=" << ComparisonOperatorImplementation::GetClassName()
         << " name=" << getName();
}


/* Here is the interface that all derived class must implement */

/* Method save() stores the object through the StorageManager */
void ComparisonOperatorImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void ComparisonOperatorImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
}


END_NAMESPACE_OPENTURNS
