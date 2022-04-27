//                                               -*- C++ -*-
/**
 *  @brief Implementation of the Haselgrove sequence
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

#ifndef OPENTURNS_HASELGROVESEQUENCE_HXX
#define OPENTURNS_HASELGROVESEQUENCE_HXX

#include "openturns/LowDiscrepancySequenceImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS
/**
 * @class HaselgroveSequence
 */

class OT_API HaselgroveSequence :
  public LowDiscrepancySequenceImplementation
{
  CLASSNAME

public:

  /** Constructor with parameters */
  explicit HaselgroveSequence(const UnsignedInteger dimension = 1);

  /** Constructor with parameters */
  explicit HaselgroveSequence(const Point & base);

  /** Virtual constructor */
  HaselgroveSequence * clone() const override;

  /** Initialize the sequence */
  void initialize(const UnsignedInteger dimension) override;

  /** Generate a quasi-random vector of numbers uniformly distributed over [0, 1[ */
  using LowDiscrepancySequenceImplementation::generate;
  Point generate() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Base for the generation of the sequence */
  Point base_;

  /** Current seed into the sequence */
  mutable Unsigned64BitsInteger seed_;

}; /* class HaselgroveSequence */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HASELGROVESEQUENCE_HXX */
