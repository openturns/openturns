//                                               -*- C++ -*-
/**
 *  @brief Simulation is a generic view of simulation methods for computing
 * probabilities and related quantities by sampling and estimation
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_SIMULATION_HXX
#define OPENTURNS_SIMULATION_HXX

#include "openturns/SimulationAlgorithm.hxx"
#include "openturns/HistoryStrategy.hxx"
#include "openturns/Compact.hxx"
#include "openturns/Last.hxx"
#include "openturns/Graph.hxx"
#include "openturns/ProbabilitySimulationResult.hxx"
#include "openturns/Event.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Simulation
 */

class OT_API Simulation
  : public SimulationAlgorithm
{

  CLASSNAME
public:

  /** Constructor with parameters */
  explicit Simulation(const Event & event,
                      const Bool verbose = true,
                      const HistoryStrategy & convergenceStrategy = Compact());

  /** Virtual constructor */
  virtual Simulation * clone() const;

  /** Event accessor */
  Event getEvent() const;

  /** Result accessor */
  ProbabilitySimulationResult getResult() const;

  /** Convergence strategy accessor */
  void setConvergenceStrategy(const HistoryStrategy & convergenceStrategy);
  HistoryStrategy getConvergenceStrategy() const;

  /** String converter */
  String __repr__() const;

  /** Performs the actual computation. */
  virtual void run();

  /** Draw the probability convergence at the given level */
  Graph drawProbabilityConvergence(const Scalar level = ResourceMap::GetAsScalar("ProbabilitySimulationResult-DefaultConfidenceLevel")) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** Result accessor */
  void setResult(const ProbabilitySimulationResult & result);

  /** Compute the block sample and the points that realized the event */
  virtual Sample computeBlockSample();

  /** For save/load mechanism*/
  Simulation(const Bool verbose = true,
             const HistoryStrategy & convergenceStrategy = Compact());

  friend class Factory<Simulation>;

  /** History strategy for the probability and variance estimate */
  HistoryStrategy convergenceStrategy_;

  // The event we are computing the probability of
  Event event_;

  // Result of the simulation
  ProbabilitySimulationResult result_;

} ; /* class Simulation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SIMULATION_HXX */
