//                                               -*- C++ -*-
/**
 *  @brief This is the interface class for orthogonal function factories
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
#include "openturns/OrthogonalUniVariateFunctionFamily.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/FourierSeriesFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(OrthogonalUniVariateFunctionFamily)

/* Default constructor */
OrthogonalUniVariateFunctionFamily::OrthogonalUniVariateFunctionFamily()
  : TypedInterfaceObject<OrthogonalUniVariateFunctionFactory>(new FourierSeriesFactory)
{
  // Nothing to do
}


/* Constructor from implementation */
OrthogonalUniVariateFunctionFamily::OrthogonalUniVariateFunctionFamily(const OrthogonalUniVariateFunctionFactory & implementation)
  : TypedInterfaceObject<OrthogonalUniVariateFunctionFactory>(implementation.clone())
{
  // Nothing to do
}


/* Constructor from implementation */
OrthogonalUniVariateFunctionFamily::OrthogonalUniVariateFunctionFamily(const Implementation & p_implementation)
  : TypedInterfaceObject<OrthogonalUniVariateFunctionFactory>(p_implementation)
{
  // Nothing to do
}


/* Constructor from implementation */
OrthogonalUniVariateFunctionFamily::OrthogonalUniVariateFunctionFamily(OrthogonalUniVariateFunctionFactory * p_implementation)
  : TypedInterfaceObject<OrthogonalUniVariateFunctionFactory>(p_implementation)
{
  // Nothing to do
}


/* The method to get the function of any order */
UniVariateFunction OrthogonalUniVariateFunctionFamily::build(const UnsignedInteger order) const
{
  return getImplementation()->build(order);
}


/* Measure accessor */
Distribution OrthogonalUniVariateFunctionFamily::getMeasure() const
{
  return getImplementation()->getMeasure();
}

/* String converter */
String OrthogonalUniVariateFunctionFamily::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " implementation=" << getImplementation()->__repr__();
}

END_NAMESPACE_OPENTURNS
