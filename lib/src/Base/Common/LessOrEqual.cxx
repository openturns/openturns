//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all LessOrEqual
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/LessOrEqual.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LessOrEqual)

static const Factory<LessOrEqual> Factory_LessOrEqual;

/* Default constructor */
LessOrEqual::LessOrEqual()
  : ComparisonOperatorImplementation()
{
  // Nothing to do
}

/* String converter */
String LessOrEqual::__repr__() const
{
  return OSS() << "class=" << LessOrEqual::GetClassName()
         << " name=" << getName();
}

String LessOrEqual::__str__(const String & offset) const
{
  return OSS() << offset << "<=";
}

/* Here is the interface that all derived class must implement */

/* Virtual constructor */
LessOrEqual * LessOrEqual::clone() const
{
  return new LessOrEqual(*this);
}

/* Return true if comparison succeeds */
Bool LessOrEqual::operator() (const Scalar a,
                              const Scalar b) const
{
  return (a <= b);
}

/* Method save() stores the object through the StorageManager */
void LessOrEqual::save(Advocate & adv) const
{
  ComparisonOperatorImplementation::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void LessOrEqual::load(Advocate & adv)
{
  ComparisonOperatorImplementation::load(adv);
}





END_NAMESPACE_OPENTURNS
