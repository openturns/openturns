//                                               -*- C++ -*-
/**
 *  @brief Metropolis-Hastings algorithm
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_RANDOMWALKMETROPOLISHASTINGS_HXX
#define OPENTURNS_RANDOMWALKMETROPOLISHASTINGS_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/MCMC.hxx"
#include "openturns/CalibrationStrategy.hxx"
#include "openturns/Interval.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RandomWalkMetropolisHastings
 *
 * @brief Metropolis-Hastings algorithm.
 *
 */
class OT_API RandomWalkMetropolisHastings
  : public MCMC
{
  CLASSNAME
public:
  typedef Collection<Distribution> DistributionCollection;
  typedef PersistentCollection<Distribution> DistributionPersistentCollection;
  typedef Collection<CalibrationStrategy> CalibrationStrategyCollection;
  typedef PersistentCollection<CalibrationStrategy> CalibrationStrategyPersistentCollection;

  /** Default constructor */
  RandomWalkMetropolisHastings();

  /** Constructor with parameters*/
  RandomWalkMetropolisHastings(const Distribution & prior,
                               const Distribution & conditional,
                               const Sample & observations,
                               const Point & initialState,
                               const DistributionCollection & proposal);

  /** Constructor with parameters*/
  RandomWalkMetropolisHastings(const Distribution & prior,
                               const Distribution & conditional,
                               const Function & model,
                               const Sample & parameters,
                               const Sample & observations,
                               const Point & initialState,
                               const DistributionCollection & proposal);

  /** String converter */
  String __repr__() const override;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  RandomWalkMetropolisHastings * clone() const override;

  /** Get a realization */
  Point getRealization() const override;

  /** Calibration strategy accessor */
  void setCalibrationStrategy(const CalibrationStrategy & calibrationStrategy);
  void setCalibrationStrategyPerComponent(const CalibrationStrategyCollection & calibrationStrategy);
  CalibrationStrategyCollection getCalibrationStrategyPerComponent() const;

  /** Proposal accessor */
  void setProposal(const DistributionCollection & proposal);
  DistributionCollection getProposal() const;

  /** Acceptance rate accessor*/
  Point getAcceptanceRate() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /// proposal densities of the markov chain
  DistributionPersistentCollection proposal_;

  /// how to calibrate the each chain component
  CalibrationStrategyPersistentCollection calibrationStrategy_;

  /// number of samples
  mutable UnsignedInteger samplesNumber_;

  /// number of samples accepted
  mutable Indices acceptedNumber_;

  /// unnormalized log-posterior density of the current state
  mutable Scalar currentPenalizedLogLikelihood_;
}; /* class RandomWalkMetropolisHastings */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RANDOMWALKMETROPOLISHASTINGS_HXX */
