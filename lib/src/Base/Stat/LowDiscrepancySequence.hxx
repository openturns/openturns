//                                               -*- C++ -*-
/**
 *  @brief      Interface for low discrepancy sequences
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_LOWDISCREPANCYSEQUENCE_HXX
#define OPENTURNS_LOWDISCREPANCYSEQUENCE_HXX

#include "LowDiscrepancySequenceImplementation.hxx"
#include "Pointer.hxx"
#include "TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS
/**
 * @class LowDiscrepancySequence
 */

class OT_API LowDiscrepancySequence :
  public TypedInterfaceObject<LowDiscrepancySequenceImplementation>
{
  CLASSNAME;

public:
  typedef Pointer<LowDiscrepancySequenceImplementation> Implementation;

  /** Default constructor */
  LowDiscrepancySequence();

  /** Constructor with parameters*/
  explicit LowDiscrepancySequence(const UnsignedInteger dimension);

  /** Constructor with parameters */
  LowDiscrepancySequence(const LowDiscrepancySequenceImplementation & implementation);

  /** Constructor with parameters */
  LowDiscrepancySequence(Implementation & p_implementation);

  /** Initialize the sequence */
  void initialize(const UnsignedInteger dimension);

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Generate a quasi-random vector of numbers uniformly distributed over [0, 1) */
  NumericalPoint generate();

  /** Generate a sample of pseudo-random vectors of numbers uniformly distributed over [0, 1) */
  NumericalSample generate(const UnsignedInteger size);

  /** Compute the star discrepancy of a sample uniformly distributed over [0, 1) */
  NumericalScalar computeStarDiscrepancy(const NumericalSample & sample) const;

  /** String converter */
  String __repr__() const;

}; /* class LowDiscrepancySequence */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LOWDISCREPANCYSEQUENCE_HXX */
