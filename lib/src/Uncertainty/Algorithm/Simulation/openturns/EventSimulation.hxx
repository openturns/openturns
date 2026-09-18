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
#ifndef OPENTURNS_EVENTSIMULATION_HXX
#define OPENTURNS_EVENTSIMULATION_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/EventSimulationImplementation.hxx"
#include "openturns/SimulationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class EventSimulation
 */

class OT_API EventSimulation
  : public TypedInterfaceObject<EventSimulationImplementation>
{

  CLASSNAME
public:

  typedef Pointer<EventSimulationImplementation> Implementation;
  typedef SimulationAlgorithm::ProgressCallback ProgressCallback;
  typedef SimulationAlgorithm::StopCallback StopCallback;

  /** Default constructor */
  EventSimulation();

  /** Constructor with parameters */
  explicit EventSimulation(const RandomVector & event,
                           const HistoryStrategy & convergenceStrategy = Compact());

  /** Constructor from implementation */
  EventSimulation(const EventSimulationImplementation & implementation);

  /** Constructor from implementation pointer */
  EventSimulation(const Implementation & p_implementation);

  /** Virtual constructor */
  EventSimulation * clone() const;

  /** Event accessor */
  RandomVector getEvent() const;
  
  /** Event accessor */
  void setEvent(const RandomVector & event);

  /** Result accessor */
  ProbabilitySimulationResult getResult() const;

  /** String converter */
  String __repr__() const override;

  /** Performs the actual computation. */
  void run();

  /** Draw the probability convergence at the given level */
  Graph drawProbabilityConvergence(const Scalar level = ResourceMap::GetAsScalar("ProbabilitySimulationResult-DefaultConfidenceLevel")) const;

  /** Input accessor */
  Sample getInputSample() const;
  
  /** Setter keep sample */
  void setKeepSample(const Bool);

  /** Maximum iterations number accessor */
  void setMaximumOuterSampling(const UnsignedInteger maximumOuterSampling);
  UnsignedInteger getMaximumOuterSampling() const;

  /** Maximum coefficient of variation accessor */
  void setMaximumCoefficientOfVariation(const Scalar maximumCoefficientOfVariation);
  Scalar getMaximumCoefficientOfVariation() const;

  /** Maximum standard deviation accessor */
  void setMaximumStandardDeviation(const Scalar maximumStandardDeviation);
  Scalar getMaximumStandardDeviation() const;

  /** Block size accessor */
  void setBlockSize(const UnsignedInteger blockSize);
  UnsignedInteger getBlockSize() const;

  /** Convergence strategy accessor */
  void setConvergenceStrategy(const HistoryStrategy & convergenceStrategy);
  HistoryStrategy getConvergenceStrategy() const;

  /** Maximum time accessor */
  void setMaximumTimeDuration(const Scalar maximumTimeDuration);
  Scalar getMaximumTimeDuration() const;

  /** Progress callback */
  void setProgressCallback(ProgressCallback callBack, void * state = nullptr);

  /** Stop callback */
  void setStopCallback(StopCallback callBack, void * state = nullptr);

} ; /* class EventSimulation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EVENTSIMULATION_HXX */
