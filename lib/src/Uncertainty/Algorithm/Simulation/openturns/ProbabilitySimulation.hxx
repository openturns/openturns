//                                               -*- C++ -*-
/**
 *  @brief ProbabilitySimulation is a generic view of simulation methods for computing
 * probabilities and related quantities by sampling and estimation
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
#ifndef OPENTURNS_PROBABILITYSIMULATION_HXX
#define OPENTURNS_PROBABILITYSIMULATION_HXX

#include "openturns/Simulation.hxx"
#include "openturns/WeightedExperiment.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProbabilitySimulation
 */

class OT_API ProbabilitySimulation : public Simulation
{

  CLASSNAME;
public:
  /** Default constructor */
  ProbabilitySimulation(const Bool verbose = true,
                        const HistoryStrategy & convergenceStrategy = Compact());

  /** Constructor with parameters */
  ProbabilitySimulation(const Event & event,
                        const WeightedExperiment & experiment,
                        const Bool verbose = true,
                        const HistoryStrategy & convergenceStrategy = Compact());

  /** Virtual constructor */
  virtual ProbabilitySimulation * clone() const;

  /** Experiment accessor */
  WeightedExperiment getExperiment() const;
  void setExperiment(const WeightedExperiment & experiment);

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  /** Block size accessor */
  virtual void setBlockSize(const UnsignedInteger blockSize);

protected:

  /** Compute the block sample and the points that realized the event */
  virtual Sample computeBlockSample();

  // The experiment type
  WeightedExperiment experiment_;


private:

} ; /* class ProbabilitySimulation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SIMULATION_HXX */
