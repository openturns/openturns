//                                               -*- C++ -*-
/**
 *  @brief ProbabilitySimulationAlgorithm is a generic view of simulation methods for computing
 * probabilities and related quantities by sampling and estimation
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PROBABILITYSIMULATIONALGORITHM_HXX
#define OPENTURNS_PROBABILITYSIMULATIONALGORITHM_HXX

#include "openturns/EventSimulation.hxx"
#include "openturns/WeightedExperiment.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProbabilitySimulationAlgorithm
 */

class OT_API ProbabilitySimulationAlgorithm : public EventSimulation
{

  CLASSNAME
public:
  /** Default constructor */
  ProbabilitySimulationAlgorithm(const Bool verbose = true,
                                 const HistoryStrategy & convergenceStrategy = Compact());

  /** Constructor with parameters */
  ProbabilitySimulationAlgorithm(const RandomVector & event,
                                 const Bool verbose = true,
                                 const HistoryStrategy & convergenceStrategy = Compact());

  /** Constructor with parameters */
  ProbabilitySimulationAlgorithm(const RandomVector & event,
                                 const WeightedExperiment & experiment,
                                 const Bool verbose = true,
                                 const HistoryStrategy & convergenceStrategy = Compact());

  /** Virtual constructor */
  ProbabilitySimulationAlgorithm * clone() const override;

  /** Experiment accessor */
  WeightedExperiment getExperiment() const;
  void setExperiment(const WeightedExperiment & experiment);

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Block size accessor */
  void setBlockSize(const UnsignedInteger blockSize) override;

protected:

  /** Compute the block sample and the points that realized the event */
  Sample computeBlockSample() override;
  Sample computeBlockSampleComposite();

  // The experiment type
  WeightedExperiment experiment_;
  Bool isExperimentProvided_;
private:

} ; /* class ProbabilitySimulationAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SIMULATION_HXX */
