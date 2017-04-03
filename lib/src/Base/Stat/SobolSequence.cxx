//                                               -*- C++ -*-
/**
 *  @brief Implementation of the Sobol' sequence
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <cstdlib>

#include "openturns/SobolSequence.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SobolSequence);
static const Factory<SobolSequence> Factory_SobolSequence;


const UnsignedInteger    SobolSequence::MaximumNumberOfDimension = 40;
const UnsignedInteger    SobolSequence::MaximumBase2Logarithm    = 62;
const NumericalScalar SobolSequence::Epsilon                  = 1.0 / power2(MaximumBase2Logarithm);
const UnsignedInteger    SobolSequence::MaximumInitialDegree     = 8;

const UnsignedInteger SobolSequence::InitialBase[MaximumNumberOfDimension*MaximumInitialDegree] =
{
  1,   0,   0,   0,   0,   0,   0,   0,
  1,   0,   0,   0,   0,   0,   0,   0,
  1,   1,   0,   0,   0,   0,   0,   0,
  1,   3,   7,   0,   0,   0,   0,   0,
  1,   1,   5,   0,   0,   0,   0,   0,
  1,   3,   1,   1,   0,   0,   0,   0,
  1,   1,   3,   7,   0,   0,   0,   0,
  1,   3,   3,   9,   9,   0,   0,   0,
  1,   3,   7,  13,   3,   0,   0,   0,
  1,   1,   5,  11,  27,   0,   0,   0,
  1,   3,   5,   1,  15,   0,   0,   0,
  1,   1,   7,   3,  29,   0,   0,   0,
  1,   3,   7,   7,  21,   0,   0,   0,
  1,   1,   1,   9,  23,  37,   0,   0,
  1,   3,   3,   5,  19,  33,   0,   0,
  1,   1,   3,  13,  11,   7,   0,   0,
  1,   1,   7,  13,  25,   5,   0,   0,
  1,   3,   5,  11,   7,  11,   0,   0,
  1,   1,   1,   3,  13,  39,   0,   0,
  1,   3,   1,  15,  17,  63,  13,   0,
  1,   1,   5,   5,   1,  27,  33,   0,
  1,   3,   3,   3,  25,  17, 115,   0,
  1,   1,   3,  15,  29,  15,  41,   0,
  1,   3,   1,   7,   3,  23,  79,   0,
  1,   3,   7,   9,  31,  29,  17,   0,
  1,   1,   5,  13,  11,   3,  29,   0,
  1,   3,   1,   9,   5,  21, 119,   0,
  1,   1,   3,   1,  23,  13,  75,   0,
  1,   3,   3,  11,  27,  31,  73,   0,
  1,   1,   7,   7,  19,  25, 105,   0,
  1,   3,   5,   5,  21,   9,   7,   0,
  1,   1,   1,  15,   5,  49,  59,   0,
  1,   1,   1,   1,   1,  33,  65,   0,
  1,   3,   5,  15,  17,  19,  21,   0,
  1,   1,   7,  11,  13,  29,   3,   0,
  1,   3,   7,   5,   7,  11, 113,   0,
  1,   1,   5,   3,  15,  19,  61,   0,
  1,   3,   1,   1,   9,  27,  89,   7,
  1,   1,   3,   7,  31,  15,  45,  23,
  1,   3,   3,   9,   9,  25, 107,  39
} ;

const Unsigned64BitsInteger SobolSequence::PrimitivePolynomial[MaximumNumberOfDimension] =
{
  1,   3,   7,  11,  13,  19,  25,  37,
  59,  47,  61,  55,  41,  67,  97,  91,
  109, 103, 115, 131, 193, 137, 145, 143,
  241, 157, 185, 167, 229, 171, 213, 191,
  253, 203, 211, 239, 247, 285, 369, 299
} ;


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
