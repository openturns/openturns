//                                               -*- C++ -*-
/**
 *  @brief The bijective function to select polynomials in the orthogonal basis
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
#include <limits>
#include "openturns/EnumerateFunctionImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EnumerateFunctionImplementation)

static const Factory<EnumerateFunctionImplementation> Factory_EnumerateFunctionImplementation;

/* Parameter constructor */
EnumerateFunctionImplementation::EnumerateFunctionImplementation(const UnsignedInteger dimension)
  : PersistentObject()
  , upperBound_(dimension, std::numeric_limits<UnsignedInteger>::max())
  , dimension_(dimension)
{
  if (!(dimension > 0))
    throw InvalidArgumentException (HERE) << "Cannot build an EnumerateFunction of dimension 0.";
}

/* Virtual constructor */
EnumerateFunctionImplementation * EnumerateFunctionImplementation::clone() const
{
  return new EnumerateFunctionImplementation(*this);
}

/* String converter */
String EnumerateFunctionImplementation::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " dimension=" << dimension_;
}

/* The cardinal of indices of degree max <= maximumDegree */
UnsignedInteger EnumerateFunctionImplementation::getMaximumDegreeCardinal(const UnsignedInteger maximumDegree) const
{
  return getStrataCumulatedCardinal(getMaximumDegreeStrataIndex(maximumDegree));
}

/* Evaluation operator */
Indices EnumerateFunctionImplementation::operator() (const UnsignedInteger) const
{
  throw NotYetImplementedException(HERE) << "In EnumerateFunctionImplementation::operator()";
}

/* The inverse of the association */
UnsignedInteger EnumerateFunctionImplementation::inverse(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = indices.getSize();
  if (size != dimension)
    throw InvalidArgumentException(HERE)  << "Error: the size of the given indices must match the dimension, here size=" << size << " and dimension=" << dimension;
  UnsignedInteger index = 0;
  while (operator()(index) != indices)
  {
    ++ index;
  }
  return index;
}

/* The cardinal of the given strata */
UnsignedInteger EnumerateFunctionImplementation::getStrataCardinal(const UnsignedInteger) const
{
  throw NotYetImplementedException(HERE) << "In EnumerateFunctionImplementation::getStrataCardinal";
}

/* The cardinal of the cumulated strata above or equal to the given strate */
UnsignedInteger EnumerateFunctionImplementation::getStrataCumulatedCardinal(const UnsignedInteger) const
{
  throw NotYetImplementedException(HERE) << "In EnumerateFunctionImplementation::getStrataCumulatedCardinal";
}

/* The index of the strata of degree max <= maximumDegree */
UnsignedInteger EnumerateFunctionImplementation::getMaximumDegreeStrataIndex(const UnsignedInteger) const
{
  throw NotYetImplementedException(HERE) << "In EnumerateFunctionImplementation::getMaximumDegreeStrataIndex";
}

/* The index of the strata of degree max <= maximumDegree */
UnsignedInteger EnumerateFunctionImplementation::getBasisSizeFromTotalDegree(const UnsignedInteger maximumDegree) const
{
  const UnsignedInteger idx = getMaximumDegreeStrataIndex(maximumDegree);
  return getStrataCumulatedCardinal(idx);
}

/* Dimension accessor */
void EnumerateFunctionImplementation::setDimension(const UnsignedInteger dimension)
{
  dimension_ = dimension;
}

UnsignedInteger EnumerateFunctionImplementation::getDimension() const
{
  return dimension_;
}

/* Upper bound accessor */
void EnumerateFunctionImplementation::setUpperBound(const Indices & upperBound)
{
  if (upperBound.getSize() != getDimension())
    throw InvalidArgumentException(HERE) << "Upper bound dimension must match enumerate function dimension.";
  upperBound_ = upperBound;
}

Indices EnumerateFunctionImplementation::getUpperBound() const
{
  return upperBound_;
}

/* Method save() stores the object through the StorageManager */
void EnumerateFunctionImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("dimension_", dimension_);
  adv.saveAttribute( "upperBound_", upperBound_ );
}

/* Method load() reloads the object from the StorageManager */
void EnumerateFunctionImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("dimension_", dimension_);
  if (adv.hasAttribute( "upperBound_"))
    adv.loadAttribute( "upperBound_", upperBound_ );
  else
    upperBound_ = Indices(getDimension(), std::numeric_limits<UnsignedInteger>::max());
}


END_NAMESPACE_OPENTURNS
