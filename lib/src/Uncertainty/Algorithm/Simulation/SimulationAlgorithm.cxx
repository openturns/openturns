//                                               -*- C++ -*-
/**
 *  @brief SimulationAlgorithm algorithms base class
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
#include "openturns/SimulationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SimulationAlgorithm)

/* Default constructor */
SimulationAlgorithm::SimulationAlgorithm()
  : TypedInterfaceObject<SimulationAlgorithmImplementation>(new SimulationAlgorithmImplementation())
{
  // Nothing to do
}

/* Constructor from implementation */
SimulationAlgorithm::SimulationAlgorithm(const SimulationAlgorithmImplementation & implementation)
  : TypedInterfaceObject<SimulationAlgorithmImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation pointer */
SimulationAlgorithm::SimulationAlgorithm(SimulationAlgorithmImplementation * p_implementation)
  : TypedInterfaceObject<SimulationAlgorithmImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation */
SimulationAlgorithm::SimulationAlgorithm(const Implementation & p_implementation)
  : TypedInterfaceObject<SimulationAlgorithmImplementation>(p_implementation)
{
  // Nothing to do
}

/* Comparison operator */
Bool SimulationAlgorithm::operator ==(const SimulationAlgorithm & other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}

/* Comparison operator */
Bool SimulationAlgorithm::operator !=(const SimulationAlgorithm & other) const
{
  return !operator==(other);
}

/* Maximum sample size accessor */
void SimulationAlgorithm::setMaximumOuterSampling(const UnsignedInteger maximumOuterSampling)
{
  copyOnWrite();
  getImplementation()->setMaximumOuterSampling(maximumOuterSampling);
}

UnsignedInteger SimulationAlgorithm::getMaximumOuterSampling() const
{
  return getImplementation()->getMaximumOuterSampling();
}

/* Maximum coefficient of variation accessor */
void SimulationAlgorithm::setMaximumCoefficientOfVariation(const Scalar maximumCoefficientOfVariation)
{
  copyOnWrite();
  getImplementation()->setMaximumCoefficientOfVariation(maximumCoefficientOfVariation);
}

Scalar SimulationAlgorithm::getMaximumCoefficientOfVariation() const
{
  return getImplementation()->getMaximumCoefficientOfVariation();
}

/* Maximum standard deviation accessor */
void SimulationAlgorithm::setMaximumStandardDeviation(const Scalar maximumStandardDeviation)
{
  copyOnWrite();
  getImplementation()->setMaximumStandardDeviation(maximumStandardDeviation);
}

Scalar SimulationAlgorithm::getMaximumStandardDeviation() const
{
  return getImplementation()->getMaximumStandardDeviation();
}

/* Block size accessor */
void SimulationAlgorithm::setBlockSize(const UnsignedInteger blockSize)
{
  copyOnWrite();
  getImplementation()->setBlockSize(blockSize);
}

UnsignedInteger SimulationAlgorithm::getBlockSize() const
{
  return getImplementation()->getBlockSize();
}

/* String converter */
String SimulationAlgorithm::__repr__() const
{
  return getImplementation()->__repr__();
}

/* Performs the actual computation. */
void SimulationAlgorithm::run()
{
  copyOnWrite();
  getImplementation()->run();
}

/* Convergence strategy accessor */
void SimulationAlgorithm::setConvergenceStrategy(const HistoryStrategy & convergenceStrategy)
{
  copyOnWrite();
  getImplementation()->setConvergenceStrategy(convergenceStrategy);
}

HistoryStrategy SimulationAlgorithm::getConvergenceStrategy() const
{
  return getImplementation()->getConvergenceStrategy();
}

void SimulationAlgorithm::setProgressCallback(ProgressCallback callBack, void * state)
{
  copyOnWrite();
  getImplementation()->setProgressCallback(callBack, state);
}

void SimulationAlgorithm::setStopCallback(StopCallback callBack, void * state)
{
  copyOnWrite();
  getImplementation()->setStopCallback(callBack, state);
}

/* Maximum time accessor */
void SimulationAlgorithm::setMaximumTimeDuration(const Scalar maximumTimeDuration)
{
  copyOnWrite();
  getImplementation()->setMaximumTimeDuration(maximumTimeDuration);
}

Scalar SimulationAlgorithm::getMaximumTimeDuration() const
{
  return getImplementation()->getMaximumTimeDuration();
}

END_NAMESPACE_OPENTURNS
