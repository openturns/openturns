//                                               -*- C++ -*-
/**
 *  @brief Implementation of the Halton' sequence
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
#include "openturns/HaltonSequence.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HaltonSequence)

static const Factory<HaltonSequence> Factory_HaltonSequence;

/* Constructor with parameters */
HaltonSequence::HaltonSequence(const UnsignedInteger dimension) :
  LowDiscrepancySequenceImplementation(dimension)
{
  initialize(dimension);
}


/* Virtual constructor */
HaltonSequence * HaltonSequence::clone() const
{
  return new HaltonSequence(*this);
}


/* Initialize the sequence */
void HaltonSequence::initialize(const UnsignedInteger dimension)
{
  if (dimension == 0) throw InvalidArgumentException(HERE) << "Dimension must be > 0.";
  dimension_ = dimension;
  base_ = ComputeFirstPrimeNumbers(dimension);
  seed_ = ResourceMap::GetAsUnsignedInteger( "HaltonSequence-InitialSeed" );
}

/* Generate a pseudo-random vector of independant numbers uniformly distributed over [0, 1[ */
Point HaltonSequence::generate() const
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
      xI += (currentSeed % radix) * inverseRadixN;
      currentSeed /= radix;
      inverseRadixN *= inverseRadix;
    }
    realization[i] = xI;
  }
  ++seed_;
  return realization;
}


/* String converter */
String HaltonSequence::__repr__() const
{
  OSS oss(true);
  oss << "class=" << HaltonSequence::GetClassName()
      << " derived from " << LowDiscrepancySequenceImplementation::__repr__()
      << " base=" << base_
      << " seed=" << seed_;
  return oss;
}

/* String converter */
String HaltonSequence::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << HaltonSequence::GetClassName()
      << " derived from " << LowDiscrepancySequenceImplementation::__str__(offset)
      << " base=" << base_
      << " seed=" << seed_;
  return oss;
}


void HaltonSequence::save(Advocate & adv) const
{
  LowDiscrepancySequenceImplementation::save(adv);
  adv.saveAttribute( "base_", base_);
  adv.saveAttribute( "seed_", seed_);
}


/* Method load() reloads the object from the StorageManager */
void HaltonSequence::load(Advocate & adv)
{
  LowDiscrepancySequenceImplementation::load(adv);
  initialize(dimension_);
  adv.loadAttribute( "base_", base_);
  adv.loadAttribute( "seed_", seed_);
}


END_NAMESPACE_OPENTURNS
