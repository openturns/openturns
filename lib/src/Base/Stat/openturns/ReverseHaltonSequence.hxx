//                                               -*- C++ -*-
/**
 *  @brief Implementation of the ReverseHalton sequence
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

#ifndef OPENTURNS_REVERSEHALTONSEQUENCE_HXX
#define OPENTURNS_REVERSEHALTONSEQUENCE_HXX

#include "openturns/LowDiscrepancySequenceImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS
/**
 * @class ReverseHaltonSequence
 */

class OT_API ReverseHaltonSequence :
  public LowDiscrepancySequenceImplementation
{
  CLASSNAME

public:

  /** Constructor with parameters */
  explicit ReverseHaltonSequence(const UnsignedInteger dimension = 1);

  /** Virtual constructor */
  ReverseHaltonSequence * clone() const override;

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
  /** Base for the generation of the sequence */
  Unsigned64BitsIntegerPersistentCollection base_;

  /** Current seed into the sequence */
  mutable Unsigned64BitsInteger seed_;

}; /* class ReverseHaltonSequence */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_REVERSEHALTONSEQUENCE_HXX */
