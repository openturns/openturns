//                                               -*- C++ -*-
/**
 *  @brief This is the interface class for orthogonal basis
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
#include "openturns/OrthogonalBasis.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(OrthogonalBasis)

/* Default constructor */
OrthogonalBasis::OrthogonalBasis()
  : TypedInterfaceObject<OrthogonalFunctionFactory>(new OrthogonalFunctionFactory())
{
  // Nothing to do
}


/* Constructor from implementation */
OrthogonalBasis::OrthogonalBasis(const OrthogonalFunctionFactory & implementation)
  : TypedInterfaceObject<OrthogonalFunctionFactory>(implementation.clone())
{
  // Nothing to do
}


/* Build the Function of the given index */
Function OrthogonalBasis::build(const UnsignedInteger index) const
{
  return getImplementation()->build(index);
}


/* Build the Function of the given multi-indices */
Function OrthogonalBasis::build(const Indices & indices) const
{
  return getImplementation()->build(indices);
}


/* Return the measure upon which the basis is orthogonal */
Distribution OrthogonalBasis::getMeasure() const
{
  return getImplementation()->getMeasure();
}

/* Return the enumerate function that translate unidimensional indices into multidimensional indices */
EnumerateFunction OrthogonalBasis::getEnumerateFunction() const
{
  return getImplementation()->getEnumerateFunction();
}

/* String converter */
String OrthogonalBasis::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " implementation=" << getImplementation()->__repr__();
}

END_NAMESPACE_OPENTURNS
