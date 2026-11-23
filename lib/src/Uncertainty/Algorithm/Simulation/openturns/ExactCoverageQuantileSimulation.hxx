//                                               -*- C++ -*-
/**
 *  @brief Exact-coverage quantile simulation
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
#ifndef OPENTURNS_EXACTCOVERAGEQUANTILESIMULATION_HXX
#define OPENTURNS_EXACTCOVERAGEQUANTILESIMULATION_HXX

#include "openturns/QuantileSimulationImplementation.hxx"
#include "openturns/WeightedExperiment.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ExactCoverageQuantileSimulation
 *
 * Streaming estimator of an extreme quantile whose order-statistic
 * interval has an exact binomial (Wilks) coverage.  The exact ranks are
 * computed before sampling, then only the relevant tail is kept in a
 * bounded heap.  The estimate and the interval are exact order
 * statistics of the full sample, while the memory is proportional to
 * the retained tail.
 */
class OT_API ExactCoverageQuantileSimulation
  : public QuantileSimulationImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  ExactCoverageQuantileSimulation();

  /** Constructor with parameters */
  explicit ExactCoverageQuantileSimulation(const RandomVector & output,
                                       const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                                       const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Constructor with parameters and experiment */
  ExactCoverageQuantileSimulation(const RandomVector & output,
                              const WeightedExperiment & experiment,
                              const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                              const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Virtual constructor */
  ExactCoverageQuantileSimulation * clone() const override;

  /** Experiment accessor */
  void setExperiment(const WeightedExperiment & experiment);
  WeightedExperiment getExperiment() const;
  Bool hasExperiment() const;

  /** Sample size accessor */
  void setSampleSize(const UnsignedInteger sampleSize) override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object through the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Check the input parameters */
  void checkInputs() const override;

  /** Run a fixed-sample experiment */
  QuantileSimulationResult runFixedSample() override;

private:
  /** Resolve the experiment, building a Monte Carlo experiment when needed */
  WeightedExperiment resolveExperiment() const;

  WeightedExperiment experiment_;
  Bool isExperimentProvided_ = false;

}; /* class ExactCoverageQuantileSimulation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EXACTCOVERAGEQUANTILESIMULATION_HXX */
