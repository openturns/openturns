//                                               -*- C++ -*-
/**
 *  @brief This an abstract class for 1D function factories
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
#include "openturns/UniVariateFunctionFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(UniVariateFunctionFactory)

static const Factory<UniVariateFunctionFactory> Factory_UniVariateFunctionFactory;


/* Default constructor */
UniVariateFunctionFactory::UniVariateFunctionFactory()
  : PersistentObject()
  , functionsCache_(0)
{
  // Nothing to do. The derived class will have to call initializeCache().
}

/* Virtual constructor */
UniVariateFunctionFactory * UniVariateFunctionFactory::clone() const
{
  return new UniVariateFunctionFactory(*this);
}


/* String converter */
String UniVariateFunctionFactory::__repr__() const
{
  return OSS() << "class=" << getClassName();
}


/* The method to get the function of any order */
UniVariateFunction UniVariateFunctionFactory::build(const UnsignedInteger ) const
{
  throw NotYetImplementedException(HERE) << "UniVariateFunctionFactory::build";
}

/* Cache initialization */
void UniVariateFunctionFactory::initializeCache()
{
  // Nothing to do
}

/* Method save() stores the object through the StorageManager */
void UniVariateFunctionFactory::save(Advocate & adv) const
{
  PersistentObject::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void UniVariateFunctionFactory::load(Advocate & adv)
{
  PersistentObject::load(adv);
}

END_NAMESPACE_OPENTURNS
