//                                               -*- C++ -*-
/**
 *  @brief Implementation of the Faure' sequence
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
#include "openturns/FaureSequence.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FaureSequence)

static const Factory<FaureSequence> Factory_FaureSequence;

/* Constructor with parameters */
FaureSequence::FaureSequence(const UnsignedInteger dimension)
  : LowDiscrepancySequenceImplementation(dimension)
{
  initialize(dimension);
}


/* Virtual constructor */
FaureSequence * FaureSequence::clone() const
{
  return new FaureSequence(*this);
}


/* Initialize the sequence */
void FaureSequence::initialize(const UnsignedInteger dimension)
{
  LowDiscrepancySequenceImplementation::initialize(dimension);
  modulus_ = GetNextPrimeNumber(dimension_);
  modulusInverse_ = 1.0 / modulus_;
  // Initialize the seed at a value large enough to avoid some of the correlation problems
  seed_ = ResourceMap::GetAsUnsignedInteger("FaureSequence-InitialSeed");
  // Number of digits of seed_ in base modulus_
  logSeed_ = 1;
  // Seed bound
  seedBound_ = modulus_;
  while (seedBound_ < seed_)
  {
    ++logSeed_;
    seedBound_ *= modulus_;
  }
  // Binomial coefficients modulo the modulus associated withthe current value of the seed
  computeInitialBinomialCoefficients();
}

/* Generate a pseudo-random vector of independent numbers uniformly distributed over [0, 1[ */
Point FaureSequence::generate() const
{
  Point realization(dimension_);
  // First, compute the decomposition of seed_ in base modulus_
  Unsigned64BitsIntegerCollection aI(logSeed_);
  UnsignedInteger n = seed_;
  for (UnsignedInteger i = 0; i < logSeed_; ++i)
  {
    aI[i] = n % modulus_;
    n /= modulus_;
  }
  // Stores the first component of the point
  Scalar xI = 0.0;
  Scalar factor = modulusInverse_;
  for (UnsignedInteger i = 0; i < logSeed_; ++i)
  {
    xI += aI[i] * factor;
    factor *= modulusInverse_;
  }
  realization[0] = xI;
  // Loop over the dimensions
  for (UnsignedInteger i = 1; i < dimension_; ++i)
  {
    // Compute the new digits as a matrix/vector multiply using uint64_t
    Unsigned64BitsIntegerCollection aINew(logSeed_);
    for (UnsignedInteger j = 0; j < logSeed_; ++j)
    {
      Unsigned64BitsInteger aINewJ = 0;
      // We perform the reduction modulo modulus_ in order to avoid integer overflow as much as possible
      for (UnsignedInteger k = j; k < logSeed_; ++k) aINewJ = (aINewJ + coefficients_[j + (k * (k + 1)) / 2] * aI[k]) % modulus_;
      aINew[j] = aINewJ;
    }
    // Compute the current component
    Scalar xJ = 0.0;
    factor = modulusInverse_;
    for (UnsignedInteger j = 0; j < logSeed_; ++j)
    {
      xJ += aINew[j] * factor;
      factor *= modulusInverse_;
    }
    realization[i] = xJ;
    aI = aINew;
  } // Loop over the dimension
  ++seed_;
  // Check if the seed has cross its upper bound
  if (seed_ == seedBound_)
  {
    seedBound_ *= modulus_;
    ++logSeed_;
    updateBinomialCoefficients();
  }
  return realization;
}


/* String converter */
String FaureSequence::__repr__() const
{
  OSS oss;
  oss << "class=" << FaureSequence::GetClassName()
      << " modulus=" << modulus_
      << " seed=" << seed_;
  return oss;
}

/* Compute the initial binomial coefficients table associated with the modulus */
void FaureSequence::computeInitialBinomialCoefficients()
{
  // The coefficients C(n, k) = n! / (k! (n-k)!) mod modulus for 0<= n < logSeed, 0<= k <= n, are
  // stored column-wise in a flat array:
  // C(n, k) = n! / (k! (n - k)!) is at position (k, n) with flat index n + k(k + 1) / 2
  coefficients_ = Unsigned64BitsIntegerCollection((logSeed_ * (logSeed_ + 1)) / 2, 1);
  // Main part of the array
  UnsignedInteger currentIndex = 4;
  UnsignedInteger previousIndex1 = 2;
  UnsignedInteger previousIndex2 = 1;
  for(UnsignedInteger n = 2; n < logSeed_; ++n)
  {
    for (UnsignedInteger k = 1; k < n; ++k)
    {
      // coeff(k, n) = coeff(k, n-1) + coeff(k-1, n-1)
      coefficients_[currentIndex] = (coefficients_[previousIndex1] + coefficients_[previousIndex2]) % modulus_;
      ++currentIndex;
      ++previousIndex1;
      ++previousIndex2;
    } // loop over the columns
    // Skip the 1's on the diagonal
    currentIndex += 2;
    ++previousIndex1;
    ++previousIndex2;
  } // loop over the rows
}

/* Update the binomial coefficients table by adding one column to the triangular array */
void FaureSequence::updateBinomialCoefficients() const
{
  // Here, we assume that the coefficient table has already been initialized and stores the values associated with logSeed-1
  UnsignedInteger previousIndex2 = coefficients_.getSize() - logSeed_ + 1;
  UnsignedInteger previousIndex1 = previousIndex2 + 1;
  coefficients_.add(1);
  for (UnsignedInteger k = 2; k < logSeed_; ++k)
  {
    coefficients_.add((coefficients_[previousIndex1] + coefficients_[previousIndex2]) % modulus_);
    ++previousIndex1;
    ++previousIndex2;
  }
  coefficients_.add(1);
}

void FaureSequence::save(Advocate & adv) const
{
  LowDiscrepancySequenceImplementation::save(adv);
  adv.saveAttribute( "coefficients_", coefficients_);
  adv.saveAttribute( "modulus_", modulus_);
  adv.saveAttribute( "modulusInverse_", modulusInverse_);
  adv.saveAttribute( "seed_", seed_);
  adv.saveAttribute( "seedBound_", seedBound_);
  adv.saveAttribute( "logSeed_", logSeed_);
}


/* Method load() reloads the object from the StorageManager */
void FaureSequence::load(Advocate & adv)
{
  LowDiscrepancySequenceImplementation::load(adv);
  adv.loadAttribute( "coefficients_", coefficients_);
  adv.loadAttribute( "modulus_", modulus_);
  adv.loadAttribute( "modulusInverse_", modulusInverse_);
  adv.loadAttribute( "seed_", seed_);
  adv.loadAttribute( "seedBound_", seedBound_);
  adv.loadAttribute( "logSeed_", logSeed_);
}

END_NAMESPACE_OPENTURNS
