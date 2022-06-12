//                                               -*- C++ -*-
/**
 *  @brief Implementation of the Haselgrove' sequence
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
#include "openturns/HaselgroveSequence.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HaselgroveSequence)

static const Factory<HaselgroveSequence> Factory_HaselgroveSequence;

/* Constructor with parameters */
HaselgroveSequence::HaselgroveSequence(const UnsignedInteger dimension)
  : LowDiscrepancySequenceImplementation(dimension)
{
  initialize(dimension);
}

/* Constructor with parameters */
HaselgroveSequence::HaselgroveSequence(const Point & base) :
  LowDiscrepancySequenceImplementation(base.getDimension())
{
  base_ = base;
  seed_ = ResourceMap::GetAsUnsignedInteger( "HaselgroveSequence-InitialSeed" );
}

/* Virtual constructor */
HaselgroveSequence * HaselgroveSequence::clone() const
{
  return new HaselgroveSequence(*this);
}

/* Initialize the sequence */
void HaselgroveSequence::initialize(const UnsignedInteger dimension)
{
  LowDiscrepancySequenceImplementation::initialize(dimension);
  const Unsigned64BitsIntegerCollection directions(GetFirstPrimeNumbers(dimension_));
  base_ = Point(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) base_[i] = sqrt(1.0 * directions[i]);
  seed_ = ResourceMap::GetAsUnsignedInteger( "HaselgroveSequence-InitialSeed" );
}

/* Generate a pseudo-random vector of independent numbers uniformly distributed over [0, 1[ */
Point HaselgroveSequence::generate() const
{
  Point realization(dimension_);
  // Loop over the components
  Scalar scratch = 0.0;
  for (UnsignedInteger i = 0; i < dimension_; ++i) realization[i] = std::modf(seed_ * base_[i], &scratch);
  ++seed_;
  return realization;
}

/* String converter */
String HaselgroveSequence::__repr__() const
{
  OSS oss(true);
  oss << "class=" << HaselgroveSequence::GetClassName()
      << " base=" << base_
      << " seed=" << seed_;
  return oss;
}

/* String converter */
String HaselgroveSequence::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << "class=" << HaselgroveSequence::GetClassName()
      << " base=" << base_
      << " seed=" << seed_;
  return oss;
}

void HaselgroveSequence::save(Advocate & adv) const
{
  LowDiscrepancySequenceImplementation::save(adv);
  adv.saveAttribute( "base_", base_);
  adv.saveAttribute( "seed_", seed_);
}


/* Method load() reloads the object from the StorageManager */
void HaselgroveSequence::load(Advocate & adv)
{
  LowDiscrepancySequenceImplementation::load(adv);
  adv.loadAttribute( "base_", base_);
  adv.loadAttribute( "seed_", seed_);
}

END_NAMESPACE_OPENTURNS
