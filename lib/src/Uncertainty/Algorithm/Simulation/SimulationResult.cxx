//                                               -*- C++ -*-
/**
 *  @brief SimulationResult stores the simulation result
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
#include <cmath>
#include "openturns/SimulationResult.hxx"
#include "openturns/DistFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS




CLASSNAMEINIT(SimulationResult);

/* Default constructor */
SimulationResult::SimulationResult()
  : TypedInterfaceObject<SimulationResultImplementation>(SimulationResultImplementation().clone())
{
  // Nothing to do
}

/* Standard constructor */
SimulationResult::SimulationResult(const Event & event,
                                   const NumericalScalar probabilityEstimate,
                                   const NumericalScalar varianceEstimate,
                                   const UnsignedInteger outerSampling,
                                   const UnsignedInteger blockSize)
  : TypedInterfaceObject<SimulationResultImplementation>(new SimulationResultImplementation(event, probabilityEstimate, varianceEstimate, outerSampling, blockSize))
{
  // Nothing to do
}

/* Constructor with parameters */
SimulationResult::SimulationResult(Implementation & p_implementation)
  : TypedInterfaceObject<SimulationResultImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor with parameters */
SimulationResult::SimulationResult(const SimulationResultImplementation & implementation)
  : TypedInterfaceObject<SimulationResultImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Event accessor */
Event SimulationResult::getEvent() const
{
  return getImplementation()->getEvent();
}

/* Probability estimate accessor */
NumericalScalar SimulationResult::getProbabilityEstimate() const
{
  return getImplementation()->getProbabilityEstimate();
}

void SimulationResult::setProbabilityEstimate(const NumericalScalar probabilityEstimate)
{
  copyOnWrite();
  getImplementation()->setProbabilityEstimate(probabilityEstimate);
}

/* Variance estimate accessor */
NumericalScalar SimulationResult::getVarianceEstimate() const
{
  return getImplementation()->getVarianceEstimate();
}

void SimulationResult::setVarianceEstimate(const NumericalScalar varianceEstimate)
{
  copyOnWrite();
  getImplementation()->setVarianceEstimate(varianceEstimate);
}

/* Coefficient of variation estimate accessor */
NumericalScalar SimulationResult::getCoefficientOfVariation() const
{
  return getImplementation()->getCoefficientOfVariation();
}

/* Standard deviation estimate accessor */
NumericalScalar SimulationResult::getStandardDeviation() const
{
  return getImplementation()->getStandardDeviation();
}

/* Mean point conditioned to the event realization accessor */
NumericalPoint SimulationResult::getMeanPointInEventDomain() const
{
  return getImplementation()->getMeanPointInEventDomain();
}

/* Outer sampling accessor */
UnsignedInteger SimulationResult::getOuterSampling() const
{
  return getImplementation()->getOuterSampling();
}

void SimulationResult::setOuterSampling(const UnsignedInteger outerSampling)
{
  copyOnWrite();
  getImplementation()->setOuterSampling(outerSampling);
}

/* String converter */
String SimulationResult::__repr__() const
{
  return getImplementation()->__repr__();
}

/* Confidence length */
NumericalScalar SimulationResult::getConfidenceLength(const NumericalScalar level) const
{
  return getImplementation()->getConfidenceLength(level);
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

/* Importance factors accessor */
NumericalPointWithDescription SimulationResult::getImportanceFactors() const
{
  return getImplementation()->getImportanceFactors();
}

/* Draw the importance factors */
Graph SimulationResult::drawImportanceFactors() const
{
  return getImplementation()->drawImportanceFactors();
}

END_NAMESPACE_OPENTURNS
