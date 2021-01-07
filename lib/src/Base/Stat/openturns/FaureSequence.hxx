//                                               -*- C++ -*-
/**
 *  @brief Implementation of the Faure sequence
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

#ifndef OPENTURNS_FAURESEQUENCE_HXX
#define OPENTURNS_FAURESEQUENCE_HXX

#include "openturns/LowDiscrepancySequenceImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS
/**
 * @class FaureSequence
 */

class OT_API FaureSequence :
  public LowDiscrepancySequenceImplementation
{
  CLASSNAME

public:

  /** Constructor with parameters */
  explicit FaureSequence(const UnsignedInteger dimension = 1);

  /** Virtual constructor */
  FaureSequence * clone() const override;

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

private:
  /** Compute the initial binomial coefficients table associated with the modulus */
  void computeInitialBinomialCoefficients();

  /** Update the binomial table by adding one column to the triangular array */
  void updateBinomialCoefficients() const;

  /** Binomial for the generation of the sequence */
  mutable Unsigned64BitsIntegerPersistentCollection coefficients_;

  /** Modulus of the sequence, i.e. least prime number greater or equal to the dimension */
  Unsigned64BitsInteger modulus_;

  /** Inverse of the modulus */
  Scalar modulusInverse_;

  /** Current seed into the sequence */
  mutable Unsigned64BitsInteger seed_;

  /** Next value of the seed that needs a coefficient update */
  mutable Unsigned64BitsInteger seedBound_;

  /** Number of digits of the seed in base the modulus */
  mutable UnsignedInteger logSeed_;

}; /* class FaureSequence */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FAURESEQUENCE_HXX */
