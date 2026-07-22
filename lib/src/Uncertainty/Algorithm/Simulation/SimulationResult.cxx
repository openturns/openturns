//                                               -*- C++ -*-
/**
 *  @brief Result of a simulation
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SimulationResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SimulationResult)

/* Default constructor */
SimulationResult::SimulationResult()
  : TypedInterfaceObject<SimulationResultImplementation>(new SimulationResultImplementation())
{
  // Nothing to do
}

/* Constructor from implementation */
SimulationResult::SimulationResult(const SimulationResultImplementation & implementation)
  : TypedInterfaceObject<SimulationResultImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation pointer */
SimulationResult::SimulationResult(SimulationResultImplementation * p_implementation)
  : TypedInterfaceObject<SimulationResultImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation */
SimulationResult::SimulationResult(const Implementation & p_implementation)
  : TypedInterfaceObject<SimulationResultImplementation>(p_implementation)
{
  // Nothing to do
}

/* Comparison operator */
Bool SimulationResult::operator ==(const SimulationResult & other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}

/* Comparison operator */
Bool SimulationResult::operator !=(const SimulationResult & other) const
{
  return !operator==(other);
}

/* Sample size accessor */
UnsignedInteger SimulationResult::getOuterSampling() const
{
  return getImplementation()->getOuterSampling();
}

void SimulationResult::setOuterSampling(const UnsignedInteger outerSampling)
{
  copyOnWrite();
  getImplementation()->setOuterSampling(outerSampling);
}

/* Block size accessor */
UnsignedInteger SimulationResult::getBlockSize() const
{
  return getImplementation()->getBlockSize();
}

void SimulationResult::setBlockSize(const UnsignedInteger blockSize)
{
  copyOnWrite();
  getImplementation()->setBlockSize(blockSize);
}

/* Elapsed time accessor */
void SimulationResult::setTimeDuration(const Scalar time)
{
  copyOnWrite();
  getImplementation()->setTimeDuration(time);
}

Scalar SimulationResult::getTimeDuration() const
{
  return getImplementation()->getTimeDuration();
}

/* String converter */
String SimulationResult::__repr__() const
{
  return getImplementation()->__repr__();
}

END_NAMESPACE_OPENTURNS
