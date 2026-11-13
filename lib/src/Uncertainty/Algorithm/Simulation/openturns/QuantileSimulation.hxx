//                                               -*- C++ -*-
/**
 *  @brief Handle of quantile simulation algorithms
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
#ifndef OPENTURNS_QUANTILESIMULATION_HXX
#define OPENTURNS_QUANTILESIMULATION_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/QuantileSimulationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class QuantileSimulation
 *
 * Handle to a quantile simulation algorithm.
 */
class OT_API QuantileSimulation
  : public TypedInterfaceObject<QuantileSimulationImplementation>
{
  CLASSNAME
public:
  typedef Pointer<QuantileSimulationImplementation> Implementation;
  typedef SimulationAlgorithm::ProgressCallback ProgressCallback;
  typedef SimulationAlgorithm::StopCallback StopCallback;

  /** Default constructor */
  QuantileSimulation();

  /** Standard constructor */
  QuantileSimulation(const RandomVector & output,
                     const Scalar alpha = 0.95,
                     const Scalar beta = 0.95);

  /** Constructor from implementation */
  QuantileSimulation(const QuantileSimulationImplementation & implementation);

  /** Constructor from implementation pointer */
  QuantileSimulation(const Implementation & p_implementation);

  /** Output random vector accessor */
  void setOutput(const RandomVector & output);
  RandomVector getOutput() const;

  /** Quantile level accessor */
  void setAlpha(const Scalar alpha);
  Scalar getAlpha() const;

  /** Confidence level accessor */
  void setConfidenceLevel(const Scalar beta);
  Scalar getConfidenceLevel() const;

  /** Sample size accessor */
  void setSampleSize(const UnsignedInteger sampleSize);
  UnsignedInteger getSampleSize() const;

  /** Estimator type accessor */
  void setEstimatorType(const QuantileSimulationResultImplementation::EstimatorType type);
  QuantileSimulationResultImplementation::EstimatorType getEstimatorType() const;

  /** Required coverage accessor */
  void setRequiredCoverage(const QuantileSimulationResultImplementation::CoverageType type);
  QuantileSimulationResultImplementation::CoverageType getRequiredCoverage() const;

  /** Performs the actual computation */
  void run();

  /** Result accessor */
  QuantileSimulationResult getResult() const;

  /** Maximum outer sampling accessor */
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

  /** Maximum time duration accessor */
  void setMaximumTimeDuration(const Scalar maximumTimeDuration);
  Scalar getMaximumTimeDuration() const;

  /** Convergence strategy accessor */
  void setConvergenceStrategy(const HistoryStrategy & convergenceStrategy);
  HistoryStrategy getConvergenceStrategy() const;

  /** Progress callback */
  void setProgressCallback(ProgressCallback callBack, void * state = nullptr);

  /** Stop callback */
  void setStopCallback(StopCallback callBack, void * state = nullptr);

  /** String converter */
  String __repr__() const override;

}; /* class QuantileSimulation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUANTILESIMULATION_HXX */
