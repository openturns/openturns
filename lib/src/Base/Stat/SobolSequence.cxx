//                                               -*- C++ -*-
/**
 *  @brief Implementation of the Sobol' sequence
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cstdlib>

#include "openturns/SobolSequence.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SobolSequence)
static const Factory<SobolSequence> Factory_SobolSequence;


const UnsignedInteger SobolSequence::MaximumBase2Logarithm    = 62;
const Scalar          SobolSequence::Epsilon                  = 1.0 / power2(MaximumBase2Logarithm);

// Not in the openturns subdir as it has not to be visible
#include "SobolSequenceDirections.hxx"

/* Constructor with parameters */
SobolSequence::SobolSequence(const UnsignedInteger dimension) :
  LowDiscrepancySequenceImplementation(dimension)
{
  initialize(dimension);
}


/* Virtual constructor */
SobolSequence * SobolSequence::clone() const
{
  return new SobolSequence(*this);
}


/* Initialize the sequence */
void SobolSequence::initialize(const UnsignedInteger dimension)
{
  if((dimension == 0) || (dimension > MaximumNumberOfDimension))
    throw InvalidDimensionException(HERE) << "Dimension must be in range [0-" << MaximumNumberOfDimension << "], here dimension=" << dimension << ".";
  dimension_ = dimension;
  // copy initial direction numbers
  base_ = Unsigned64BitsIntegerCollection(dimension_ * MaximumBase2Logarithm, 0);

  for ( UnsignedInteger i = 0; i < dimension_; ++ i )
    for ( UnsignedInteger j = 0; j < MaximumInitialDegree; ++ j )
      base_[i * MaximumBase2Logarithm + j] = InitialBase[i * MaximumInitialDegree + j];

  // initialize row 0 (first dimension)
  for ( UnsignedInteger j = 0; j < MaximumBase2Logarithm; ++ j )
    base_[j] = 1;
  // initialize remaining direction numbers, for each dimension <-> rows of base_[][]
  for ( UnsignedInteger i = 1; i < dimension_; ++ i )
  {
    // number of bits of PrimitivePolynomial[i]
    UnsignedInteger polynomialCoefficientDegree = 0;
    Unsigned64BitsInteger polynomialCoefficient = PrimitivePolynomial[i];

    // get number of bits of the PrimitivePolynomial[i] coefficient (could have used log2)
    while(polynomialCoefficient > 1)
    {
      polynomialCoefficient /= 2;
      ++ polynomialCoefficientDegree;
    }

    // generate remaining direction numbers
    for ( UnsignedInteger j = polynomialCoefficientDegree; j < MaximumBase2Logarithm; ++ j )
    {
      base_[i * MaximumBase2Logarithm + j] = base_[i * MaximumBase2Logarithm + j - polynomialCoefficientDegree];
      for ( UnsignedInteger k = 1; k <= polynomialCoefficientDegree; ++ k )
      {
        if((PrimitivePolynomial[i] & power2(polynomialCoefficientDegree - k)) > 0)
        {
          base_[i * MaximumBase2Logarithm + j] ^= base_[i * MaximumBase2Logarithm + j - k] * power2(k);
        }
      } // k
    } // j
  } // i

  // multiply columns of directionNumber[][] by appropriate power of 2
  for ( UnsignedInteger j = 0; j < MaximumBase2Logarithm - 1; ++ j )
    for ( UnsignedInteger i = 0; i < dimension_; ++ i )
      base_[i * MaximumBase2Logarithm + j] *= power2(MaximumBase2Logarithm - j - 1);

  // initialize integer coefficients of the sequence : first column of directionNumber[][]
  coefficients_ = Unsigned64BitsIntegerCollection(dimension_);
  for ( UnsignedInteger i = 0; i < dimension_; ++ i )
    coefficients_[i] = base_[i * MaximumBase2Logarithm];

  // set the seed
  seed_ = ResourceMap::GetAsUnsignedInteger( "SobolSequence-InitialSeed" );
}


/* Generate a pseudo-random vector of independant numbers uniformly distributed over [0, 1[ */
Point SobolSequence::generate() const
{
  // initialize a point with values 2^-MaximumBase2Logarithm
  Point sequencePoint(dimension_, Epsilon);

  // compute the position of the lowest 0 bit in the binary representation of seed_
  const UnsignedInteger positionOfLowest0BitOfSeed = computePositionOfLowest0Bit(seed_);

  // for each dimension
  for(UnsignedInteger i = 0; i < dimension_; ++ i )
  {
    // compute sequence from integer coefficients
    sequencePoint[i] *= coefficients_[i];

    // update integer coefficients for next generation
    coefficients_[i] ^= base_[i * MaximumBase2Logarithm + positionOfLowest0BitOfSeed - 1];
  }
  // increment seed for next generation
  ++seed_;
  return sequencePoint;
}


/* String converter */
String SobolSequence::__repr__() const
{
  OSS oss;
  oss << "class=" << SobolSequence::GetClassName()
      << " derived from " << LowDiscrepancySequenceImplementation::__repr__()
      << " coefficients=" << coefficients_
      << " seed=" << seed_;
  return oss;
}


/* return 2^n */
Unsigned64BitsInteger SobolSequence::power2(const UnsignedInteger n)
{
  return (Unsigned64BitsInteger)1 << n;
}


/* returns the position of the lowest '0' in the binary representation of an integer */
UnsignedInteger SobolSequence::computePositionOfLowest0Bit(const Unsigned64BitsInteger number)
{
  UnsignedInteger base2Logarithm = 0;
  while((number & power2(base2Logarithm)) && (base2Logarithm <= MaximumBase2Logarithm))
  {
    ++ base2Logarithm;
  }
  return base2Logarithm + 1;// 1 <=> first bit
}


void SobolSequence::save(Advocate & adv) const
{
  LowDiscrepancySequenceImplementation::save(adv);
  adv.saveAttribute( "seed_", seed_);
  adv.saveAttribute( "coefficients_", coefficients_);
}


/* Method load() reloads the object from the StorageManager */
void SobolSequence::load(Advocate & adv)
{
  LowDiscrepancySequenceImplementation::load(adv);
  initialize(dimension_);
  adv.loadAttribute( "seed_", seed_);
  adv.loadAttribute( "coefficients_", coefficients_);
}


END_NAMESPACE_OPENTURNS
