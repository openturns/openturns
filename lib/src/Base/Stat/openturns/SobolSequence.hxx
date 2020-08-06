//                                               -*- C++ -*-
/**
 *  @brief Implementation of the Sobol sequence
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

#ifndef OPENTURNS_SOBOLSEQUENCE_HXX
#define OPENTURNS_SOBOLSEQUENCE_HXX

#include "openturns/LowDiscrepancySequenceImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS
/**
 * @class SobolSequence
 */

class OT_API SobolSequence :
  public LowDiscrepancySequenceImplementation
{
  CLASSNAME

public:
  // this implementation supports dimensions up to 1111
  static const UnsignedInteger MaximumNumberOfDimension;

  // this implementation has a cycle of 2^62 = ~5e18, thanks to the use of 64 bits integers (Unsigned64BitsInteger)
  static const UnsignedInteger MaximumBase2Logarithm;

  // this value is 2^-MaximumBase2Logarithm, precomputed to speed up generation
  static const Scalar Epsilon;

  // maximum number of columns in InitialBase array = 8
  static const UnsignedInteger MaximumInitialDegree;

  // numbers used to generate the coefficients of directionNumber_[][] each row corresponds to a component (dimension)
  static const UnsignedInteger InitialBase[];

  // a primitive polynomial used to generate the sequence
  static const Unsigned64BitsInteger PrimitivePolynomial[];

public:

  /** Constructor with parameters */
  explicit SobolSequence(const UnsignedInteger dimension = 1);

  /** Virtual constructor */
  SobolSequence * clone() const override;

  /** Initialize the sequence */
  void initialize(const UnsignedInteger dimension) override;

  /** Generate a quasi-random vector of numbers uniformly distributed over [0, 1[ */
  using LowDiscrepancySequenceImplementation::generate;
  Point generate() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** The numbers used to generate the sequence */
  Unsigned64BitsIntegerPersistentCollection base_;
  mutable Unsigned64BitsIntegerPersistentCollection coefficients_;

  /** Current seed */
  mutable Unsigned64BitsInteger seed_;

private:
  /** return 2^n */
  static Unsigned64BitsInteger inline power2(const UnsignedInteger n);

  /** Returns the position of the lowest '0' in the binary representation of an unsigned integer */
  static UnsignedInteger computePositionOfLowest0Bit(const Unsigned64BitsInteger number);

}; /* class SobolSequence */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SOBOLSEQUENCE_HXX */
