//                                               -*- C++ -*-
/**
 *  @brief Generic probability-inversion quantile estimator
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
#ifndef OPENTURNS_PROBABILITYINVERSIONQUANTILE_HXX
#define OPENTURNS_PROBABILITYINVERSIONQUANTILE_HXX

#include "openturns/QuantileSimulationImplementation.hxx"
#include "openturns/EventSimulation.hxx"
#include "openturns/ComparisonOperator.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProbabilityInversionQuantile
 *
 * Generic quantile estimator obtained by inverting the probability
 * estimate of an event-simulation engine.  The class is a fallback for
 * engines that expose only a scalar probability for a threshold event
 * and no sample, weighted sample or threshold sequence.
 *
 * A plain Monte Carlo probability-simulation engine is not inverted:
 * it exposes a sample and is delegated to QuantileSimulationAlgorithm,
 * which returns the exact order-statistic interval.  Other engines are
 * inverted by a confidence-bound bisection.  The wrapped engine is
 * cloned for every probability evaluation; its event is replaced by the
 * threshold event and the engine is run from its current state.  The
 * process-wide random generator state is never saved or restored.
 *
 * References:
 * R. Y. Rubinstein and D. P. Kroese. Simulation and the Monte Carlo
 * Method, 3rd edition, Wiley, 2016.  The stochastic root-finding and
 * the confidence-bound bisection used here follow the treatment in
 * this book.
 * H. Robbins and S. Monro. A stochastic approximation method. The
 * Annals of Mathematical Statistics, 22(3):400-407, 1951.  The
 * stochastic-approximation alternative to bisection is discussed in
 * the implementation notes even though the current adapter always
 * uses bisection.
 */
class OT_API ProbabilityInversionQuantile
  : public QuantileSimulationImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  ProbabilityInversionQuantile();

  /** Constructor with parameters */
  ProbabilityInversionQuantile(const EventSimulation & algorithm,
                               const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                               const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Virtual constructor */
  ProbabilityInversionQuantile * clone() const override;

  /** Wrapped algorithm accessor */
  void setAlgorithm(const EventSimulation & algorithm);
  EventSimulation getAlgorithm() const;
  Bool hasAlgorithm() const;

  /** Common-random-number request accessor */
  void setUseCommonRandomNumbers(const Bool useCommonRandomNumbers);
  Bool getUseCommonRandomNumbers() const;

  /** Root-finding tolerance accessor */
  void setRootTolerance(const Scalar tolerance);
  Scalar getRootTolerance() const;

  /** Maximum root-finding iteration accessor */
  void setMaximumRootIteration(const UnsignedInteger maximumIteration);
  UnsignedInteger getMaximumRootIteration() const;

  /** Pilot sample size accessor */
  void setPilotSampleSize(const UnsignedInteger pilotSampleSize);
  UnsignedInteger getPilotSampleSize() const;

  /** Replicate count accessor */
  void setReplicateCount(const UnsignedInteger replicateCount);
  UnsignedInteger getReplicateCount() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Check the input parameters */
  void checkInputs() const override;

  /** Run the probability-inversion experiment */
  QuantileSimulationResult runFixedSample() override;

private:
  /** One probability evaluation of the wrapped engine */
  struct Evaluation
  {
    Scalar eventProbability = 0.0;
    Scalar tailProbability = 0.0;
    Scalar standardDeviation = -1.0;
    Bool hasStandardDeviation = false;
  };

  /** One complete inversion */
  struct Inversion
  {
    Scalar estimate = 0.0;
    Interval bracket;
    UnsignedInteger iterationCount = 0;
    UnsignedInteger probabilityEvaluationCount = 0;
    Sample history;
    Bool converged = false;
  };

  /** Whether the wrapped engine is a plain Monte Carlo simulation */
  Bool isPlainMonteCarlo() const;

  /** Whether the wrapped engine evaluates a fixed sample, hence a
      deterministic survival function */
  Bool hasFixedExperiment() const;

  /** Nominal sample size of the wrapped engine */
  UnsignedInteger getNominalSampleSize() const;

  /** Evaluate the wrapped engine at the given threshold */
  Evaluation evaluate(const Scalar threshold) const;

  /** Invert the probability: sign bisection for a deterministic fixed
      experiment, confidence-bound bisection otherwise */
  Inversion invert(const Scalar start,
                   const Scalar step,
                   const Bool useConfidenceBounds) const;

  /** Delegate a plain Monte Carlo engine to direct extraction */
  QuantileSimulationResult runDelegated();

  EventSimulation algorithm_;
  Bool hasAlgorithm_ = false;
  ComparisonOperator thresholdOperator_;
  Bool upperTail_ = true;
  Bool useCommonRandomNumbers_ = true;
  Scalar rootTolerance_ = ResourceMap::GetAsScalar("QuantileRootSolver-DefaultTolerance");
  UnsignedInteger maximumRootIteration_ = ResourceMap::GetAsUnsignedInteger("QuantileRootSolver-DefaultMaximumIteration");
  UnsignedInteger pilotSampleSize_ = ResourceMap::GetAsUnsignedInteger("QuantileRootSolver-PilotSampleSize");
  UnsignedInteger replicateCount_ = 1;

}; /* class ProbabilityInversionQuantile */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PROBABILITYINVERSIONQUANTILE_HXX */
