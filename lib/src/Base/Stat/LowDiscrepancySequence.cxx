//                                               -*- C++ -*-
/**
 *  @brief      Interface for low discrepancy sequences
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/LowDiscrepancySequence.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LowDiscrepancySequence)


/* Default constructor */
LowDiscrepancySequence::LowDiscrepancySequence()
  : TypedInterfaceObject<LowDiscrepancySequenceImplementation>(LowDiscrepancySequenceImplementation().clone())
{
  // Nothing to do
}


/* Constructor with parameters*/
LowDiscrepancySequence::LowDiscrepancySequence(const UnsignedInteger dimension)
  : TypedInterfaceObject<LowDiscrepancySequenceImplementation>(LowDiscrepancySequenceImplementation(dimension).clone())
{
  // Nothing to do
}


/* Constructor with parameters */
LowDiscrepancySequence::LowDiscrepancySequence(Implementation & p_implementation)
  : TypedInterfaceObject<LowDiscrepancySequenceImplementation>(p_implementation)
{
  // Nothing to do
}


/* Constructor with parameters */
LowDiscrepancySequence::LowDiscrepancySequence(const LowDiscrepancySequenceImplementation & implementation)
  : TypedInterfaceObject<LowDiscrepancySequenceImplementation>(implementation.clone())
{
  // Nothing to do
}


/* Initialize the sequence */
void LowDiscrepancySequence::initialize(const UnsignedInteger dimension)
{
  return getImplementation()->initialize(dimension);
}


/* Dimension accessor */
UnsignedInteger LowDiscrepancySequence::getDimension() const
{
  return getImplementation()->getDimension();
}


/* Generate a sample of pseudo-random vectors of numbers uniformly distributed over [0, 1) */
Point LowDiscrepancySequence::generate() const
{
  return getImplementation()->generate();
}


/* Generate a sample of pseudo-random vectors of numbers uniformly distributed over [0, 1) */
Sample LowDiscrepancySequence::generate(const UnsignedInteger size) const
{
  return getImplementation()->generate(size);
}


/* Compute the star discrepancy of a sample uniformly distributed over [0, 1) */
Scalar LowDiscrepancySequence::computeStarDiscrepancy(const Sample & sample) const
{
  return getImplementation()->ComputeStarDiscrepancy(sample);
}


/* String converter */
String LowDiscrepancySequence::__repr__() const
{
  OSS oss;
  oss << "class=" << LowDiscrepancySequence::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}


END_NAMESPACE_OPENTURNS
