//                                               -*- C++ -*-
/**
 *  @brief Directional-simulation quantile estimator
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
#ifndef OPENTURNS_DIRECTIONALSAMPLINGQUANTILE_HXX
#define OPENTURNS_DIRECTIONALSAMPLINGQUANTILE_HXX

#include "openturns/QuantileSimulationImplementation.hxx"
#include "openturns/RootStrategy.hxx"
#include "openturns/SamplingStrategy.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DirectionalSamplingQuantile
 *
 * Quantile estimator obtained by a deterministic bisection on the
 * directional-simulation probability estimate.  The direction sample
 * is drawn once and reused at every candidate threshold, so the
 * probability estimate is a smooth random function of the threshold
 * and the root is well defined even though the directions are random.
 * The interval follows from the delta method applied to the
 * conditional probabilities of the fixed direction sample.
 *
 * References:
 * P. Bjerager. Probability integration by directional simulation.
 * Journal of Engineering Mechanics, 114(8):1285-1302, 1988.  This
 * article introduces the directional conditional probability and the
 * directional average implemented here.
 * R. Y. Rubinstein and D. P. Kroese. Simulation and the Monte Carlo
 * Method, 3rd edition, Wiley, 2016.  The delta method and the
 * stochastic root-finding discussion used for the interval follow the
 * treatment in this book.
 */
class OT_API DirectionalSamplingQuantile
  : public QuantileSimulationImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  DirectionalSamplingQuantile();

  /** Constructor with parameters */
  DirectionalSamplingQuantile(const RandomVector & output,
                              const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                              const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Virtual constructor */
  DirectionalSamplingQuantile * clone() const override;

  /** Root strategy accessor */
  void setRootStrategy(const RootStrategy & rootStrategy);
  RootStrategy getRootStrategy() const;

  /** Sampling strategy accessor */
  void setSamplingStrategy(const SamplingStrategy & samplingStrategy);
  SamplingStrategy getSamplingStrategy() const;

  /** Root-finding tolerance accessor */
  void setRootTolerance(const Scalar tolerance);
  Scalar getRootTolerance() const;

  /** Maximum root-finding iteration accessor */
  void setMaximumRootIteration(const UnsignedInteger maximumIteration);
  UnsignedInteger getMaximumRootIteration() const;

  /** Pilot sample size accessor */
  void setPilotSampleSize(const UnsignedInteger pilotSampleSize);
  UnsignedInteger getPilotSampleSize() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Run the directional root-finding experiment */
  QuantileSimulationResult runFixedSample() override;

private:
  /** Probability estimate and conditional contributions at a threshold */
  Scalar computeProbability(const Scalar threshold, Point & contributions);

  RootStrategy rootStrategy_;
  SamplingStrategy samplingStrategy_;
  Scalar rootTolerance_ = ResourceMap::GetAsScalar("QuantileRootSolver-DefaultTolerance");
  UnsignedInteger maximumRootIteration_ = ResourceMap::GetAsUnsignedInteger("QuantileRootSolver-DefaultMaximumIteration");
  UnsignedInteger pilotSampleSize_ = ResourceMap::GetAsUnsignedInteger("QuantileRootSolver-PilotSampleSize");

  /** State cached for the duration of a run */
  Function standardFunction_;
  Distribution standardDistribution_;
  Sample directions_;
  Point originPoint_;
  Scalar originValue_ = 0.0;

}; /* class DirectionalSamplingQuantile */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DIRECTIONALSAMPLINGQUANTILE_HXX */
