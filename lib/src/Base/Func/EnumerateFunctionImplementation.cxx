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
  , dimension_(dimension)
{
  if ( dimension == 0 )
    throw InvalidArgumentException ( HERE ) << "Cannot build an EnumerateFunction of dimension 0.";
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
Indices EnumerateFunctionImplementation::operator() (const UnsignedInteger ) const
{
  throw NotYetImplementedException( HERE ) << "In EnumerateFunctionImplementation::operator() (const UnsignedInteger index) const";
}

/* The inverse of the association */
UnsignedInteger EnumerateFunctionImplementation::inverse(const Indices & ) const
{
  throw NotYetImplementedException( HERE ) << "In EnumerateFunctionImplementation::inverse(const Indices & indices) const";
}

/* The cardinal of the given strata */
UnsignedInteger EnumerateFunctionImplementation::getStrataCardinal(const UnsignedInteger ) const
{
  throw NotYetImplementedException( HERE ) << "In EnumerateFunctionImplementation::getStrataCardinal(const UnsignedInteger strateIndex) const";
}

/* The cardinal of the cumulated strata above or equal to the given strate */
UnsignedInteger EnumerateFunctionImplementation::getStrataCumulatedCardinal(const UnsignedInteger ) const
{
  throw NotYetImplementedException( HERE ) << "In EnumerateFunctionImplementation::getStrataCumulatedCardinal(const UnsignedInteger strateIndex) const";
}

/* The index of the strata of degree max <= maximumDegree */
UnsignedInteger EnumerateFunctionImplementation::getMaximumDegreeStrataIndex(const UnsignedInteger ) const
{
  throw NotYetImplementedException( HERE ) << "In EnumerateFunctionImplementation::getMaximumDegreeStrataIndex(const UnsignedInteger maximumDegree) const";
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

/* Method save() stores the object through the StorageManager */
void EnumerateFunctionImplementation::save(Advocate & adv) const
{
  PersistentObject::save( adv );
  adv.saveAttribute( "dimension_", dimension_ );
}

/* Method load() reloads the object from the StorageManager */
void EnumerateFunctionImplementation::load(Advocate & adv)
{
  PersistentObject::load( adv );
  adv.loadAttribute( "dimension_", dimension_ );
}


END_NAMESPACE_OPENTURNS
