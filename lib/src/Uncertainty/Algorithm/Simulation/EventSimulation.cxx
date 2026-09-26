//                                               -*- C++ -*-
/**
 *  @brief EventSimulation is a generic view of simulation methods for computing
 * probabilities and related quantities by sampling and estimation
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
#include "openturns/EventSimulation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class EventSimulation
 */

CLASSNAMEINIT(EventSimulation)

/* Default constructor */
EventSimulation::EventSimulation()
  : TypedInterfaceObject<EventSimulationImplementation>(new EventSimulationImplementation())
{
}

/* Constructor with parameters */
EventSimulation::EventSimulation(const RandomVector & event,
                                 const HistoryStrategy & convergenceStrategy)
  : TypedInterfaceObject<EventSimulationImplementation>(new EventSimulationImplementation(event, convergenceStrategy))
{
  // Nothing to do
}

/* Constructor from implementation */
EventSimulation::EventSimulation(const EventSimulationImplementation & implementation)
  : TypedInterfaceObject<EventSimulationImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation pointer */
EventSimulation::EventSimulation(const Implementation & p_implementation)
  : TypedInterfaceObject<EventSimulationImplementation>(p_implementation)
{
  // Nothing to do
}

/* Virtual constructor */
EventSimulation * EventSimulation::clone() const
{
  return new EventSimulation(*this);
}

/*  Event accessor */
RandomVector EventSimulation::getEvent() const
{
  return getImplementation()->getEvent();
}

/*  Event accessor */
void EventSimulation::setEvent(const RandomVector & event)
{
  copyOnWrite();
  getImplementation()->setEvent(event);
}

/* Result accessor */
ProbabilitySimulationResult EventSimulation::getResult() const
{
  return getImplementation()->getResult();
}

/* String converter */
String EventSimulation::__repr__() const
{
  return getImplementation()->__repr__();
}

/* Performs the actual computation. */
void EventSimulation::run()
{
  copyOnWrite();
  getImplementation()->run();
}

/* Accessor to input sample */
Sample EventSimulation::getInputSample() const
{
  return getImplementation()->getInputSample();
}

/* Setter keep sample */
void EventSimulation::setKeepSample(const Bool keepSample)
{
  copyOnWrite();
  getImplementation()->setKeepSample(keepSample);
}

/* Draw the probability convergence at the given level */
Graph EventSimulation::drawProbabilityConvergence(const Scalar level) const
{
  return getImplementation()->drawProbabilityConvergence(level);
}

void EventSimulation::setMaximumOuterSampling(const UnsignedInteger maximumOuterSampling)
{
  copyOnWrite();
  getImplementation()->setMaximumOuterSampling(maximumOuterSampling);
}

UnsignedInteger EventSimulation::getMaximumOuterSampling() const
{
  return getImplementation()->getMaximumOuterSampling();
}

void EventSimulation::setMaximumCoefficientOfVariation(const Scalar maximumCoefficientOfVariation)
{
  copyOnWrite();
  getImplementation()->setMaximumCoefficientOfVariation(maximumCoefficientOfVariation);
}

Scalar EventSimulation::getMaximumCoefficientOfVariation() const
{
  return getImplementation()->getMaximumCoefficientOfVariation();
}

void EventSimulation::setMaximumStandardDeviation(const Scalar maximumStandardDeviation)
{
  copyOnWrite();
  getImplementation()->setMaximumStandardDeviation(maximumStandardDeviation);
}

Scalar EventSimulation::getMaximumStandardDeviation() const
{
  return getImplementation()->getMaximumStandardDeviation();
}

void EventSimulation::setBlockSize(const UnsignedInteger blockSize)
{
  copyOnWrite();
  getImplementation()->setBlockSize(blockSize);
}

UnsignedInteger EventSimulation::getBlockSize() const
{
  return getImplementation()->getBlockSize();
}

void EventSimulation::setConvergenceStrategy(const HistoryStrategy & convergenceStrategy)
{
  copyOnWrite();
  getImplementation()->setConvergenceStrategy(convergenceStrategy);
}

HistoryStrategy EventSimulation::getConvergenceStrategy() const
{
  return getImplementation()->getConvergenceStrategy();
}

void EventSimulation::setMaximumTimeDuration(const Scalar maximumTimeDuration)
{
  copyOnWrite();
  getImplementation()->setMaximumTimeDuration(maximumTimeDuration);
}

Scalar EventSimulation::getMaximumTimeDuration() const
{
  return getImplementation()->getMaximumTimeDuration();
}

void EventSimulation::setProgressCallback(ProgressCallback callBack, void * state)
{
  copyOnWrite();
  getImplementation()->setProgressCallback(callBack, state);
}

void EventSimulation::setStopCallback(StopCallback callBack, void * state)
{
  copyOnWrite();
  getImplementation()->setStopCallback(callBack, state);
}

END_NAMESPACE_OPENTURNS
