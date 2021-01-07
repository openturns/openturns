//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all GreaterOrEqual
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/GreaterOrEqual.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GreaterOrEqual)

static const Factory<GreaterOrEqual> Factory_GreaterOrEqual;

/* Default constructor */
GreaterOrEqual::GreaterOrEqual()
  : ComparisonOperatorImplementation()
{
  // Nothing to do
}

/* String converter */
String GreaterOrEqual::__repr__() const
{
  return OSS() << "class=" << GreaterOrEqual::GetClassName()
         << " name=" << getName();
}

String GreaterOrEqual::__str__(const String & offset) const
{
  return OSS() << offset << ">=";
}

/* Here is the interface that all derived class must implement */

/* Virtual constructor */
GreaterOrEqual * GreaterOrEqual::clone() const
{
  return new GreaterOrEqual(*this);
}

/* Return true if comparison succeeds */
Bool GreaterOrEqual::operator() (const Scalar a,
                                 const Scalar b) const
{
  return (a >= b);
}

/* Method save() stores the object through the StorageManager */
void GreaterOrEqual::save(Advocate & adv) const
{
  ComparisonOperatorImplementation::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void GreaterOrEqual::load(Advocate & adv)
{
  ComparisonOperatorImplementation::load(adv);
}





END_NAMESPACE_OPENTURNS
