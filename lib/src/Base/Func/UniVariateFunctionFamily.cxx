//                                               -*- C++ -*-
/**
 *  @brief This is the interface class for  function factories
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
#include "openturns/UniVariateFunctionFamily.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(UniVariateFunctionFamily)

/* Default constructor */
UniVariateFunctionFamily::UniVariateFunctionFamily()
  : TypedInterfaceObject<UniVariateFunctionFactory>(new UniVariateFunctionFactory)
{
  // Nothing to do
}


/* Constructor from implementation */
UniVariateFunctionFamily::UniVariateFunctionFamily(const UniVariateFunctionFactory & implementation)
  : TypedInterfaceObject<UniVariateFunctionFactory>(implementation.clone())
{
  // Nothing to do
}


/* The method to get the function of any order */
UniVariateFunction UniVariateFunctionFamily::build(const UnsignedInteger order) const
{
  return getImplementation()->build(order);
}


/* String converter */
String UniVariateFunctionFamily::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " implementation=" << getImplementation()->__repr__();
}

END_NAMESPACE_OPENTURNS
