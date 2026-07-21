//                                               -*- C++ -*-
/**
 *  @brief Implementation of the Halton' sequence
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
#include <limits>
#include <algorithm>
#include <string>

#include "openturns/HaltonSequence.hxx"
#include "openturns/Log.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HaltonSequence)

static const Factory<HaltonSequence> Factory_HaltonSequence;

/* Constructor with parameters */
HaltonSequence::HaltonSequence(const UnsignedInteger dimension,
                               const String & scrambling)
  : LowDiscrepancySequenceImplementation(dimension)
  , scrambling_("") // To force a check in setScrambling
{
  // This call will check the value of scrambling and trigger the initialization
  setScrambling(scrambling);
}


/* Virtual constructor */
HaltonSequence * HaltonSequence::clone() const
{
  return new HaltonSequence(*this);
}

/* Comparison operators */
Bool HaltonSequence::operator ==(const HaltonSequence & other) const
{
  if (this == &other) return true;
  if (!hasEqualBase(other)) return false;
  if (scrambling_ != other.scrambling_) return false;
  if (base_.getSize() != other.base_.getSize()) return false;
  for (UnsignedInteger i = 0; i < base_.getSize(); ++i)
    if (base_[i] != other.base_[i]) return false;
  if (permutations_.getSize() != other.permutations_.getSize()) return false;
  for (UnsignedInteger i = 0; i < permutations_.getSize(); ++i)
    if (permutations_[i] != other.permutations_[i]) return false;
  return true;
}

Bool HaltonSequence::equals(const LowDiscrepancySequenceImplementation & other) const
{
  const HaltonSequence * p_other = dynamic_cast<const HaltonSequence *>(&other);
  return p_other && (*this == *p_other);
}

/* Initialize the sequence */
void HaltonSequence::initialize(const UnsignedInteger dimension)
{
  LowDiscrepancySequenceImplementation::initialize(dimension);
  base_ = GetFirstPrimeNumbers(dimension_);
  seed_ = ResourceMap::GetAsUnsignedInteger( "HaltonSequence-InitialSeed" );
  permutations_ = Collection<Indices>(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    const UnsignedInteger b = base_[i];
    Indices permutation(b);
    if (scrambling_ == "REVERSE")
    {
      for (UnsignedInteger j = 1; j < b; ++j)
        permutation[j] = b - j;
    } // REVERSE
    else if (scrambling_ == "RANDOM")
    {
      Indices buffer(b);
      buffer.fill();
      for (UnsignedInteger j = 1; j < b; ++j)
      {
        const UnsignedInteger index = j + LCGgenerate() % (b - j);
        permutation[j] = buffer[index];
        buffer[index] = buffer[j];
      }
    } // RANDOM
    else // NO SCRAMBLING
      permutation.fill();
    LOGDEBUG(OSS() << "b=" << b << ", permutation=" << permutation);
    permutations_[i] = permutation;
  } // i
}

/* Generate a pseudo-random vector of independent numbers uniformly distributed over [0, 1[ */
Point HaltonSequence::generate() const
{
  Point realization(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    const Unsigned64BitsInteger radix = base_[i];
    Unsigned64BitsInteger currentSeed = seed_;
    const Indices & permutation = permutations_[i];
    Scalar xI = 0.0;
    const Scalar inverseRadix = 1.0 / radix;
    Scalar inverseRadixN = inverseRadix;
    while (currentSeed > 0)
    {
      xI += permutation[static_cast<UnsignedInteger>(currentSeed % radix)] * inverseRadixN;
      currentSeed /= radix;
      inverseRadixN *= inverseRadix;
    }
    realization[i] = xI;
  }
  ++seed_;
  return realization;
}

/* Generate a sample of pseudo-random vectors */
Sample HaltonSequence::generate(const UnsignedInteger size) const
{
  Sample result(size, dimension_);
  Unsigned64BitsInteger localSeed = seed_;
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      const Unsigned64BitsInteger radix = base_[i];
      Unsigned64BitsInteger currentSeed = localSeed;
      const Indices & permutation = permutations_[i];
      Scalar xI = 0.0;
      const Scalar inverseRadix = 1.0 / radix;
      Scalar inverseRadixN = inverseRadix;
      while (currentSeed > 0)
      {
        xI += permutation[static_cast<UnsignedInteger>(currentSeed % radix)] * inverseRadixN;
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

/** Permutations accessor */
Collection<Indices> HaltonSequence::getPermutations() const
{
  return permutations_;
}

/* Scrambling accessor */
void HaltonSequence::setScrambling(const String & scrambling)
{
  if (scrambling != scrambling_)
  {
    if (scrambling != "NONE" &&
        scrambling != "REVERSE" &&
        scrambling != "RANDOM")
      throw InvalidArgumentException(HERE) << "Error: valid values for scrambling are \"NONE\", \"REVERSE\" and \"RANDOM\"";
    scrambling_ = scrambling;
    initialize(dimension_);
  }
}

String HaltonSequence::getScrambling() const
{
  return scrambling_;
}

/* String converter */
String HaltonSequence::__repr__() const
{
  OSS oss(true);
  oss << "class=" << HaltonSequence::GetClassName()
      << " base=" << base_
      << " seed=" << seed_
      << " permutations=" << permutations_
      << " scrambling=" << scrambling_;
  return oss;
}

/* String converter */
String HaltonSequence::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << "class=" << HaltonSequence::GetClassName()
      << " base=" << base_
      << " seed=" << seed_
      << " permutations=" << permutations_
      << " scrambling=" << scrambling_;
  return oss;
}


void HaltonSequence::save(Advocate & adv) const
{
  LowDiscrepancySequenceImplementation::save(adv);
  adv.saveAttribute( "base_", base_);
  adv.saveAttribute( "seed_", seed_);
  adv.saveAttribute( "permutations_", permutations_);
  adv.saveAttribute( "scrambling_", scrambling_);
}


/* Method load() reloads the object from the StorageManager */
void HaltonSequence::load(Advocate & adv)
{
  LowDiscrepancySequenceImplementation::load(adv);
  adv.loadAttribute( "base_", base_);
  adv.loadAttribute( "seed_", seed_);
  adv.loadAttribute( "permutations_", permutations_);
  adv.loadAttribute( "scrambling_", scrambling_);
}


END_NAMESPACE_OPENTURNS
