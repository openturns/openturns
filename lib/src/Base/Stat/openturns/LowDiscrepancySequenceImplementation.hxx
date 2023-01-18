//                                               -*- C++ -*-
/**
 *  @brief This class is a top-level class for low discrepancy sequences
 *  simulation algorithms. It delegates to its children the effective
 *  history strategy.
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LOWDISCREPANCYSEQUENCEIMPLEMENTATION_HXX
#define OPENTURNS_LOWDISCREPANCYSEQUENCEIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Interval.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS
/**
 * @class LowDiscrepancySequenceImplementation
 */

class OT_API LowDiscrepancySequenceImplementation
  : public PersistentObject
{

  CLASSNAME

public:


  /** Constructor with parameters */
  explicit LowDiscrepancySequenceImplementation(const UnsignedInteger dimension = 1);

  /** Virtual constructor */
  LowDiscrepancySequenceImplementation * clone() const override;

  /** Initialize the sequence */
  virtual void initialize(const UnsignedInteger dimension);

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Generate a quasi-random vector of numbers uniformly distributed over [0, 1) */
  virtual Point generate() const;

  /** Generate a sample of pseudo-random vectors of numbers uniformly distributed over [0, 1) */
  virtual Sample generate(const UnsignedInteger size) const;

  /** Compute the star discrepancy of a sample uniformly distributed over [0, 1) */
  static Scalar ComputeStarDiscrepancy(const Sample & sample);

  /** Scrambling state accessor */
  void setScramblingState(const UnsignedInteger state);
  Unsigned64BitsInteger getScramblingState() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /** Compute the local discrepancy of a sample, given a multidimensional interval */
  static Scalar ComputeLocalDiscrepancy(const Sample & sample,
                                        const Interval & interval);

protected:
  typedef Collection<Unsigned64BitsInteger>                           Unsigned64BitsIntegerCollection;
  typedef PersistentCollection<Unsigned64BitsInteger>                 Unsigned64BitsIntegerPersistentCollection;

  /* Implement a linear congruential pseudo-random numbers generator for internal
     use within the low discrepancy sequences, based on a 2^64 modulus */
  Unsigned64BitsInteger LCGgenerate() const
  {
    // No need to take the 2^64 modulus here as it is implicit in
    // the 64 bits arithmetic
    LCGState_ = 2862933555777941757 * LCGState_ + 3037000493;
    return LCGState_;
  }

  /** Compute the n first prime numbers */
  static Unsigned64BitsIntegerCollection GetFirstPrimeNumbers(const UnsignedInteger n);

  /** Compute the least prime number greater or equal to n */
  static Unsigned64BitsInteger GetNextPrimeNumber(const UnsignedInteger n);

  /** Dimension parameter */
  UnsignedInteger dimension_;

  /** LCG State */
  mutable Unsigned64BitsInteger LCGState_;
} ; /* class LowDiscrepancySequenceImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LOWDISCREPANCYSEQUENCEIMPLEMENTATION_HXX */
