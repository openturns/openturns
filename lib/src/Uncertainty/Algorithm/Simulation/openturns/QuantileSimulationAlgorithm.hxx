//                                               -*- C++ -*-
/**
 *  @brief Quantile simulation driven by a weighted experiment
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
#ifndef OPENTURNS_QUANTILESIMULATIONALGORITHM_HXX
#define OPENTURNS_QUANTILESIMULATIONALGORITHM_HXX

#include "openturns/QuantileSimulationImplementation.hxx"
#include "openturns/WeightedExperiment.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class QuantileSimulationAlgorithm
 *
 * Quantile simulation driven by a WeightedExperiment.  The algorithm
 * draws the experiment once (or several independent replicates for a
 * randomized stratified design), computes the weighted empirical
 * inverse, and dispatches the confidence interval according to the
 * experiment and estimator conventions.
 */
class OT_API QuantileSimulationAlgorithm
  : public QuantileSimulationImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  QuantileSimulationAlgorithm();

  /** Constructor with parameters */
  QuantileSimulationAlgorithm(const RandomVector & output,
                              const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                              const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Constructor with parameters and experiment */
  QuantileSimulationAlgorithm(const RandomVector & output,
                              const WeightedExperiment & experiment,
                              const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                              const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Virtual constructor */
  QuantileSimulationAlgorithm * clone() const override;

  /** Experiment accessor */
  void setExperiment(const WeightedExperiment & experiment);
  WeightedExperiment getExperiment() const;
  Bool hasExperiment() const;

  /** Sample size accessor */
  void setSampleSize(const UnsignedInteger sampleSize) override;

  /** Replicate count accessor */
  void setReplicateCount(const UnsignedInteger replicateCount);
  UnsignedInteger getReplicateCount() const;

  /** Keep sample accessor */
  void setKeepSample(const Bool keepSample);
  Bool getKeepSample() const;

  /** Input sample accessor */
  Sample getInputSample() const;

  /** Output sample accessor */
  Sample getOutputSample() const;

  /** Weights accessor */
  Point getWeights() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Check the input parameters */
  void checkInputs() const override;

  /** Run a fixed-sample experiment */
  QuantileSimulationResult runFixedSample() override;

private:
  WeightedExperiment experiment_;
  Bool isExperimentProvided_ = false;
  UnsignedInteger replicateCount_ = ResourceMap::GetAsUnsignedInteger("QuantileSimulationAlgorithm-DefaultReplicateCount");
  Bool keepSample_ = false;
  Sample inputSample_;
  Sample outputSample_;
  Point weights_;

}; /* class QuantileSimulationAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUANTILESIMULATIONALGORITHM_HXX */
