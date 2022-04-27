//                                               -*- C++ -*-
/**
 *  @brief Splitter implementation
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SplitterImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SplitterImplementation)

static const Factory<SplitterImplementation> Factory_SplitterImplementation;


/* Default constructor */
SplitterImplementation::SplitterImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/** Constructor with temperature T0 & iMax*/
SplitterImplementation::SplitterImplementation(const UnsignedInteger N)
  : PersistentObject()
  , N_(N)
{
  // Nothing to do
}

/* Virtual constructor method */
SplitterImplementation * SplitterImplementation::clone() const
{
  return new SplitterImplementation(*this);
}

Indices SplitterImplementation::generate(Indices &) const
{
  throw NotYetImplementedException(HERE);
}

/* Reset sequence state */
void SplitterImplementation::reset() const
{
  currentIndex_ = 0;
}

/* Set of indices size accessor */
UnsignedInteger SplitterImplementation::getN() const
{
  return N_;
}

/* Number of indices pairs accessor */
UnsignedInteger SplitterImplementation::getSize() const
{
  throw NotYetImplementedException(HERE);
}

/* String converter */
String SplitterImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << SplitterImplementation::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SplitterImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("N_", N_);
}

/* Method load() reloads the object from the StorageManager */
void SplitterImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("N_", N_);
}


END_NAMESPACE_OPENTURNS
