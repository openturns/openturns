//                                               -*- C++ -*-
/**
 *  @brief Simulation algorithm to estimate Sobol indices
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SOBOLSIMULATIONALGORITHM_HXX
#define OPENTURNS_SOBOLSIMULATIONALGORITHM_HXX

#include "openturns/SimulationAlgorithm.hxx"
#include "openturns/SobolSimulationResult.hxx"
#include "openturns/SobolIndicesAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SobolSimulationAlgorithm
 */

class OT_API SobolSimulationAlgorithm
  : public SimulationAlgorithm
{

  CLASSNAME
public:
  /** Default constructor */
  SobolSimulationAlgorithm();

  /** Constructor with parameters */
  SobolSimulationAlgorithm(const Distribution & distribution,
                           const Function & model,
                           const SobolIndicesAlgorithm & estimator);

  /** Virtual constructor */
  SobolSimulationAlgorithm * clone() const override;

  /** RandomVector accessor */
  Distribution getDistribution() const;

  /** Result accessor */
  SobolSimulationResult getResult() const;

  /** Criterion type on standard deviation accessor */
  void setIndexQuantileLevel(const Scalar indexQuantileLevel);
  Scalar getIndexQuantileLevel() const;

  /** Maximum of standard deviation on each component */
  void setIndexQuantileEpsilon(const Scalar indexQuantileEpsilon);
  Scalar getIndexQuantileEpsilon() const;

  /** Estimator accessor */
  void setEstimator(const SobolIndicesAlgorithm & estimator);
  SobolIndicesAlgorithm getEstimator() const;

  /** Size of evaluation blocks */
  void setBatchSize(const UnsignedInteger & replicationSize);
  UnsignedInteger getBatchSize() const;

  /** String converter */
  String __repr__() const override;

  /** Performs the actual computation. */
  void run() override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Draw the probability convergence at the given level */
  Graph drawFirstOrderIndexConvergence(const UnsignedInteger marginalIndex = 0,
                                       const Scalar level = ResourceMap::GetAsScalar("ProbabilitySimulationResult-DefaultConfidenceLevel")) const;
  Graph drawTotalOrderIndexConvergence(const UnsignedInteger marginalIndex = 0,
                                       const Scalar level = ResourceMap::GetAsScalar("ProbabilitySimulationResult-DefaultConfidenceLevel")) const;
protected:
  /** Draw the probability convergence at the given level */
  Graph drawIndexConvergence(const UnsignedInteger marginalIndex,
                             const Scalar level,
                             const String & label) const;

  /** Result accessor */
  void setResult(const SobolSimulationResult & result);

  // The vector to study
  Distribution distribution_;
  Function model_;

  // Estimator type
  SobolIndicesAlgorithm estimator_;

  // Size of evaluation blocks
  UnsignedInteger batchSize_;

  // Result of the simulation
  SobolSimulationResult result_;

  // stopping criteria on the indices estimates
  Scalar indexQuantileLevel_;
  Scalar indexQuantileEpsilon_;

  // criterion to discriminate a small/controlled/large index
  Scalar smallIndexThreshold_;

} ; /* class SobolSimulationAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SOBOLSIMULATIONALGORITHM_HXX */
