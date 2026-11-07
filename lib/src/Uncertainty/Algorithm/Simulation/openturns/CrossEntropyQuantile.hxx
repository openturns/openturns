//                                               -*- C++ -*-
/**
 *  @brief Cross-entropy quantile estimator
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
#ifndef OPENTURNS_CROSSENTROPYQUANTILE_HXX
#define OPENTURNS_CROSSENTROPYQUANTILE_HXX

#include "openturns/QuantileSimulationImplementation.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CrossEntropyQuantile
 *
 * Quantile estimator based on a cross-entropy importance-sampling
 * sequence.  A pilot sample locates the target tail, the cross-entropy
 * engine builds an auxiliary distribution on that tail, and the
 * quantile is the weighted empirical inverse of a final sample drawn
 * from a defensive proposal derived from the auxiliary distribution,
 * with weights f_initial / f_proposal.  The interval is the weighted
 * Woodruff interval.
 *
 * The cross-entropy auxiliary distribution concentrates on the tail, so
 * its standard deviations are typically smaller than one in the
 * standard space.  The corresponding raw importance weights then have
 * an infinite variance, which makes the weighted quantile unstable.
 * The defensive proposal keeps the cross-entropy mean but inflates
 * every standard deviation to a floor of at least one, which restores a
 * finite variance.  The floor is controlled by
 * setMinimumProposalStandardDeviation().
 *
 * References:
 * R. Y. Rubinstein and D. P. Kroese. Simulation and the Monte Carlo
 * Method. Wiley Series in Probability and Statistics, 2nd edition,
 * 2008.  The book describes the cross-entropy adaptive importance
 * sampling construction and its parametric update.
 * P.-T. de Boer, D. P. Kroese, S. Mannor and R. Y. Rubinstein. A
 * tutorial on the cross-entropy method. Annals of Operations Research,
 * 134(1):19-67, 2005.  The tutorial details the intermediate quantile
 * levels of the adaptive sequence.
 * R. S. Woodruff. Confidence intervals for medians and other position
 * measures. Journal of the American Statistical Association,
 * 47(260):635-646, 1952.  The article gives the interval obtained by
 * inverting a confidence interval for the distribution function, which
 * is applied here to the weighted empirical CDF.
 */
class OT_API CrossEntropyQuantile
  : public QuantileSimulationImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  CrossEntropyQuantile();

  /** Constructor with parameters */
  CrossEntropyQuantile(const RandomVector & output,
                       const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                       const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Virtual constructor */
  CrossEntropyQuantile * clone() const override;

  /** Intermediate quantile level accessor */
  void setIntermediateQuantileLevel(const Scalar level);
  Scalar getIntermediateQuantileLevel() const;

  /** Pilot sample size accessor */
  void setPilotSize(const UnsignedInteger pilotSize);
  UnsignedInteger getPilotSize() const;

  /** Minimum proposal standard deviation accessor */
  void setMinimumProposalStandardDeviation(const Scalar value);
  Scalar getMinimumProposalStandardDeviation() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Run the cross-entropy sequence */
  QuantileSimulationResult runFixedSample() override;

private:
  Scalar intermediateQuantileLevel_ = ResourceMap::GetAsScalar("CrossEntropyImportanceSampling-DefaultQuantileLevel");
  UnsignedInteger pilotSize_ = ResourceMap::GetAsUnsignedInteger("CrossEntropyQuantile-DefaultPilotSize");
  Scalar minimumProposalStandardDeviation_ = ResourceMap::GetAsScalar("CrossEntropyQuantile-MinimumProposalStandardDeviation");

}; /* class CrossEntropyQuantile */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CROSSENTROPYQUANTILE_HXX */
