//                                               -*- C++ -*-
/**
 *  @brief Implementation of the ReverseHalton' sequence
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ReverseHaltonSequence.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ReverseHaltonSequence)

static const Factory<ReverseHaltonSequence> Factory_ReverseHaltonSequence;

/* Constructor with parameters */
ReverseHaltonSequence::ReverseHaltonSequence(const UnsignedInteger dimension)
  : LowDiscrepancySequenceImplementation(dimension)
{
  initialize(dimension);
}


/* Virtual constructor */
ReverseHaltonSequence * ReverseHaltonSequence::clone() const
{
  return new ReverseHaltonSequence(*this);
}

/* Comparison operators */
Bool ReverseHaltonSequence::operator ==(const ReverseHaltonSequence & other) const
{
  if (this == &other) return true;
  if (!hasEqualBase(other)) return false;
  if (base_.getSize() != other.base_.getSize()) return false;
  for (UnsignedInteger i = 0; i < base_.getSize(); ++i)
    if (base_[i] != other.base_[i]) return false;
  return true;
}

Bool ReverseHaltonSequence::equals(const LowDiscrepancySequenceImplementation & other) const
{
  const ReverseHaltonSequence * p_other = dynamic_cast<const ReverseHaltonSequence *>(&other);
  return p_other && (*this == *p_other);
}

/* Initialize the sequence */
void ReverseHaltonSequence::initialize(const UnsignedInteger dimension)
{
  LowDiscrepancySequenceImplementation::initialize(dimension);
  base_ = GetFirstPrimeNumbers(dimension_);
  seed_ = ResourceMap::GetAsUnsignedInteger( "ReverseHaltonSequence-InitialSeed" );
}

/* Generate a pseudo-random vector of independent numbers uniformly distributed over [0, 1[
   See Bart Vandewoestyne, Ronald Cools, "Good permutations for deterministic scrambled Halton sequences in terms of L2-discrepancy", Journal of Computational and Applied Mathematics, 189, 341-361 (2006) */
Point ReverseHaltonSequence::generate() const
{
  Point realization(dimension_);
  // Loop over the components
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    Scalar xI = 0.0;
    const Unsigned64BitsInteger radix = base_[i];
    const Scalar inverseRadix = 1.0 / radix;
    Scalar inverseRadixN = inverseRadix;
    Unsigned64BitsInteger currentSeed = seed_;
    while (currentSeed > 0)
    {
      const UnsignedInteger digit = currentSeed % radix;
      if (digit != 0) xI += (radix - digit) * inverseRadixN;
      currentSeed /= radix;
      inverseRadixN *= inverseRadix;
    }
    realization[i] = xI;
  }
  ++seed_;
  return realization;
}


/* Generate a sample of quasi-random vectors */
Sample ReverseHaltonSequence::generate(const UnsignedInteger size) const
{
  Sample result(size, dimension_);
  Unsigned64BitsInteger localSeed = seed_;
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      Scalar xI = 0.0;
      const Unsigned64BitsInteger radix = base_[i];
      const Scalar inverseRadix = 1.0 / radix;
      Scalar inverseRadixN = inverseRadix;
      Unsigned64BitsInteger currentSeed = localSeed;
      while (currentSeed > 0)
      {
        const UnsignedInteger digit = static_cast<UnsignedInteger>(currentSeed % radix);
        if (digit != 0) xI += (radix - digit) * inverseRadixN;
        currentSeed /= radix;
        inverseRadixN *= inverseRadix;
      }
      result(k, i) = xI;
    }
    ++localSeed;
  }
  seed_ = localSeed;
  return result;
}

/* String converter */
String ReverseHaltonSequence::__repr__() const
{
  OSS oss;
  oss << "class=" << ReverseHaltonSequence::GetClassName()
      << " base=" << base_
      << " seed=" << seed_;
  return oss;
}

void ReverseHaltonSequence::save(Advocate & adv) const
{
  LowDiscrepancySequenceImplementation::save(adv);
  adv.saveAttribute( "base_", base_);
  adv.saveAttribute( "seed_", seed_);
}


/* Method load() reloads the object from the StorageManager */
void ReverseHaltonSequence::load(Advocate & adv)
{
  LowDiscrepancySequenceImplementation::load(adv);
  adv.loadAttribute( "base_", base_);
  adv.loadAttribute( "seed_", seed_);
}


END_NAMESPACE_OPENTURNS
