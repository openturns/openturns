//                                               -*- C++ -*-
/**
 *  @brief ProbabilitySimulationResult is a handle to any concrete
 *  simulation result, preserving its dynamic type
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
#include "openturns/ProbabilitySimulationResult.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProbabilitySimulationResult
 */

CLASSNAMEINIT(ProbabilitySimulationResult)

/* Default constructor */
ProbabilitySimulationResult::ProbabilitySimulationResult()
  : TypedInterfaceObject<ProbabilitySimulationResultImplementation>(new ProbabilitySimulationResultImplementation())
{
  // Nothing to do
}

/* Standard constructor: build the implementation from the given values */
ProbabilitySimulationResult::ProbabilitySimulationResult(const RandomVector & event,
    const Scalar probabilityEstimate,
    const Scalar varianceEstimate,
    const UnsignedInteger outerSampling,
    const UnsignedInteger blockSize)
  : TypedInterfaceObject<ProbabilitySimulationResultImplementation>(new ProbabilitySimulationResultImplementation(event, probabilityEstimate, varianceEstimate, outerSampling, blockSize))
{
  // Nothing to do
}

/* Constructor from a concrete result: clone for independence */
ProbabilitySimulationResult::ProbabilitySimulationResult(const ProbabilitySimulationResultImplementation & result)
  : TypedInterfaceObject<ProbabilitySimulationResultImplementation>(Pointer<ProbabilitySimulationResultImplementation>(result.clone()))
{
  // Nothing to do
}

/* Implicit conversion to the base result type: copy the base subobject */
ProbabilitySimulationResult::operator SimulationResult() const
{
  return *getImplementation();
}

/* Constructor from implementation */
ProbabilitySimulationResult::ProbabilitySimulationResult(const Implementation & p_implementation)
  : TypedInterfaceObject<ProbabilitySimulationResultImplementation>(p_implementation)
{
  // Nothing to do
}

#ifndef SWIG
/* Constructor from implementation pointer */
ProbabilitySimulationResult::ProbabilitySimulationResult(ProbabilitySimulationResultImplementation * p_implementation)
  : TypedInterfaceObject<ProbabilitySimulationResultImplementation>(p_implementation)
{
  // Nothing to do
}
#endif

/* String converter: delegate to the wrapped result so that the textual
   representation of a result is unchanged on the user side */
String ProbabilitySimulationResult::__repr__() const
{
  return getImplementation()->__repr__();
}

String ProbabilitySimulationResult::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

/* Probability estimate accessor */
Scalar ProbabilitySimulationResult::getProbabilityEstimate() const
{
  return getImplementation()->getProbabilityEstimate();
}

/* Variance estimate accessor */
Scalar ProbabilitySimulationResult::getVarianceEstimate() const
{
  return getImplementation()->getVarianceEstimate();
}

/* Coefficient of variation estimate accessor */
Scalar ProbabilitySimulationResult::getCoefficientOfVariation() const
{
  return getImplementation()->getCoefficientOfVariation();
}

/* Standard deviation estimate accessor */
Scalar ProbabilitySimulationResult::getStandardDeviation() const
{
  return getImplementation()->getStandardDeviation();
}

/* Outer sampling accessor */
UnsignedInteger ProbabilitySimulationResult::getOuterSampling() const
{
  return getImplementation()->getOuterSampling();
}

void ProbabilitySimulationResult::setOuterSampling(const UnsignedInteger outerSampling)
{
  copyOnWrite();
  getImplementation()->setOuterSampling(outerSampling);
}

/* Block size accessor */
UnsignedInteger ProbabilitySimulationResult::getBlockSize() const
{
  return getImplementation()->getBlockSize();
}

void ProbabilitySimulationResult::setBlockSize(const UnsignedInteger blockSize)
{
  copyOnWrite();
  getImplementation()->setBlockSize(blockSize);
}

/* Elapsed time accessor */
Scalar ProbabilitySimulationResult::getTimeDuration() const
{
  return getImplementation()->getTimeDuration();
}

void ProbabilitySimulationResult::setTimeDuration(const Scalar timeDuration)
{
  copyOnWrite();
  getImplementation()->setTimeDuration(timeDuration);
}

/* Event accessor */
RandomVector ProbabilitySimulationResult::getEvent() const
{
  return getImplementation()->getEvent();
}

/* Event accessor: kept for source compatibility, removed in a future release */
void ProbabilitySimulationResult::setEvent(const RandomVector & event)
{
  LOGWARN("ProbabilitySimulationResult.setEvent is deprecated, use ProbabilitySimulationResultImplementation");
  copyOnWrite();
  getImplementation()->setEvent(event);
}

/* Probability estimate accessor: kept for source compatibility, removed in a future release */
void ProbabilitySimulationResult::setProbabilityEstimate(const Scalar probabilityEstimate)
{
  LOGWARN("ProbabilitySimulationResult.setProbabilityEstimate is deprecated, use ProbabilitySimulationResultImplementation");
  copyOnWrite();
  getImplementation()->setProbabilityEstimate(probabilityEstimate);
}

/* Variance estimate accessor: kept for source compatibility, removed in a future release */
void ProbabilitySimulationResult::setVarianceEstimate(const Scalar varianceEstimate)
{
  LOGWARN("ProbabilitySimulationResult.setVarianceEstimate is deprecated, use ProbabilitySimulationResultImplementation");
  copyOnWrite();
  getImplementation()->setVarianceEstimate(varianceEstimate);
}

/* Probability estimate distribution accessor */
Distribution ProbabilitySimulationResult::getProbabilityDistribution() const
{
  return getImplementation()->getProbabilityDistribution();
}

/* Mean point conditioned to the event realization accessor */
Point ProbabilitySimulationResult::getMeanPointInEventDomain() const
{
  return getImplementation()->getMeanPointInEventDomain();
}

/* Importance factors accessor: kept for source compatibility, removed in a future release */
PointWithDescription ProbabilitySimulationResult::getImportanceFactors() const
{
  LOGWARN("ProbabilitySimulationResult.getImportanceFactors is deprecated, use ProbabilitySimulationResultImplementation");
  return getImplementation()->getImportanceFactors();
}

/* Draw the importance factors: kept for source compatibility, removed in a future release */
Graph ProbabilitySimulationResult::drawImportanceFactors() const
{
  LOGWARN("ProbabilitySimulationResult.drawImportanceFactors is deprecated, use ProbabilitySimulationResultImplementation");
  return getImplementation()->drawImportanceFactors();
}

/* Confidence length: kept for source compatibility, removed in a future release */
Scalar ProbabilitySimulationResult::getConfidenceLength(const Scalar level) const
{
  LOGWARN("ProbabilitySimulationResult.getConfidenceLength is deprecated, use ProbabilitySimulationResultImplementation");
  return getImplementation()->getConfidenceLength(level);
}

END_NAMESPACE_OPENTURNS
