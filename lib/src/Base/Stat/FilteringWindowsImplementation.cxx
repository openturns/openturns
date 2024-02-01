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
#include "openturns/FilteringWindowsImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FilteringWindowsImplementation
 */

CLASSNAMEINIT(FilteringWindowsImplementation)

static const Factory<FilteringWindowsImplementation> Factory_FilteringWindowsImplementation;

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

Scalar FilteringWindowsImplementation::operator()(const Scalar ) const
{
  throw NotYetImplementedException(HERE) << "In FilteringWindowsImplementation::operator()(const Scalar t) const";
}

/* String converter */
String FilteringWindowsImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class= " << FilteringWindowsImplementation::GetClassName();
  return oss;
}

/* String converter */
String FilteringWindowsImplementation::__str__(const String & ) const
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
