//                                               -*- C++ -*-
/**
 *  @brief
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
#include "FilteringWindowsImplementation.hxx"
#include "Exception.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FilteringWindowsImplementation
 */

CLASSNAMEINIT(FilteringWindowsImplementation);

static Factory<FilteringWindowsImplementation> RegisteredFactory("FilteringWindowsImplementation");

/* Constructor without parameters */
FilteringWindowsImplementation::FilteringWindowsImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Virtual constructor */
FilteringWindowsImplementation * FilteringWindowsImplementation::clone() const
{
  return new FilteringWindowsImplementation(*this);
}

NumericalScalar FilteringWindowsImplementation::operator()(const NumericalScalar t) const
{
  throw NotYetImplementedException(HERE) << "In FilteringWindowsImplementation::operator()(const NumericalScalar t) const";
}

/* String converter */
String FilteringWindowsImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class= " << FilteringWindowsImplementation::GetClassName();
  return oss;
}

/* String converter */
String FilteringWindowsImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class= " << FilteringWindowsImplementation::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void FilteringWindowsImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void FilteringWindowsImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
}

END_NAMESPACE_OPENTURNS
