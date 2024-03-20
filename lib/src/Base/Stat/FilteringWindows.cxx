//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/FilteringWindows.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Hamming.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FilteringWindows)

/* Constructor with parameters */
FilteringWindows::FilteringWindows()
  : TypedInterfaceObject<FilteringWindowsImplementation>(new Hamming())
{
  // Nothing to do
}

/* Parameters constructor */
FilteringWindows::FilteringWindows(const FilteringWindowsImplementation & implementation)
  : TypedInterfaceObject<FilteringWindowsImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
FilteringWindows::FilteringWindows(const Implementation & p_implementation)
  : TypedInterfaceObject<FilteringWindowsImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
FilteringWindows::FilteringWindows(FilteringWindowsImplementation * p_implementation)
  : TypedInterfaceObject<FilteringWindowsImplementation>(p_implementation)
{
  // Nothing to do
}

/* functor - evaluation of filter using () */
Scalar FilteringWindows::operator()(const Scalar t) const
{
  return getImplementation()->operator()(t);
}

/* String converter */
String FilteringWindows::__repr__() const
{
  OSS oss(true);
  oss << "class=" << FilteringWindows::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String FilteringWindows::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << FilteringWindows::GetClassName()
      << " implementation=" << getImplementation()->__str__(offset);
  return oss;
}

END_NAMESPACE_OPENTURNS
