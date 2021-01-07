//                                               -*- C++ -*-
/**
 *  @brief The bijective function to select polynomials in the orthogonal basis
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
#include "openturns/EnumerateFunction.hxx"
#include "openturns/LinearEnumerateFunction.hxx"
#include "openturns/HyperbolicAnisotropicEnumerateFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(EnumerateFunction)

/* Default constructor */
EnumerateFunction::EnumerateFunction()
  : TypedInterfaceObject<EnumerateFunctionImplementation>(new LinearEnumerateFunction(1))
{
  // Nothing to do
}

/* Constructor with parameters */
EnumerateFunction::EnumerateFunction(const EnumerateFunctionImplementation & implementation)
  : TypedInterfaceObject<EnumerateFunctionImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor with parameters */
EnumerateFunction::EnumerateFunction(Implementation & p_implementation)
  : TypedInterfaceObject<EnumerateFunctionImplementation>(p_implementation)
{
  // Nothing to do
}

/* String converter */
String EnumerateFunction::__repr__() const
{
  return getImplementation()->__repr__();
}

String EnumerateFunction::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

/* The bijective association between an integer and a set of indices */
Indices EnumerateFunction::operator() (const UnsignedInteger index) const
{
  return getImplementation()->operator()( index );
}

/* The inverse of the association */
UnsignedInteger EnumerateFunction::inverse(const Indices & indices) const
{
  return getImplementation()->inverse( indices );
}

/* The cardinal of the given strata */
UnsignedInteger EnumerateFunction::getStrataCardinal(const UnsignedInteger strataIndex) const
{
  return getImplementation()->getStrataCardinal( strataIndex );
}

/* The cardinal of the cumulated strata above or equal to the given strata */
UnsignedInteger EnumerateFunction::getStrataCumulatedCardinal(const UnsignedInteger strataIndex) const
{
  return getImplementation()->getStrataCumulatedCardinal( strataIndex );
}

/* The index of the strata of degree max < degree */
UnsignedInteger EnumerateFunction::getMaximumDegreeStrataIndex(const UnsignedInteger maximumDegree) const
{
  return getImplementation()->getMaximumDegreeStrataIndex( maximumDegree );
}

/* The cardinal of indices of degree max <= maximumDegree */
UnsignedInteger EnumerateFunction::getMaximumDegreeCardinal(const UnsignedInteger maximumDegree) const
{
  return getImplementation()->getMaximumDegreeCardinal(maximumDegree);
}

/* Dimension accessor */
void EnumerateFunction::setDimension(const UnsignedInteger dimension)
{
  copyOnWrite();
  getImplementation()->setDimension( dimension );
}

UnsignedInteger EnumerateFunction::getDimension() const
{
  return getImplementation()->getDimension();
}

END_NAMESPACE_OPENTURNS
