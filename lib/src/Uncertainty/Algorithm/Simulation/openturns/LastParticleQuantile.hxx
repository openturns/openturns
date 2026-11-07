//                                               -*- C++ -*-
/**
 *  @brief Last-particle quantile estimator
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
#ifndef OPENTURNS_LASTPARTICLEQUANTILE_HXX
#define OPENTURNS_LASTPARTICLEQUANTILE_HXX

#include "openturns/QuantileSimulationImplementation.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LastParticleQuantile
 *
 * Last-particle algorithm of Guyader, Hengartner and Matzner-Lober.
 * A population of N particles is maintained; at each move the
 * extreme particle is replaced by a point drawn from the conditional
 * distribution through a Metropolis-Hastings kernel.  The estimate is
 * the average of the two thresholds around the Poisson move count
 * M = ceil(-N log p), and the interval is the Poisson rank interval.
 *
 * References:
 * A. Guyader, N. Hengartner and E. Matzner-Lober. Simulation and
 * estimation of extreme quantiles and extreme probabilities. Applied
 * Mathematics & Optimization, 64(2):171-196, 2011.  This article
 * introduces the last-particle process, the Poisson move count and the
 * rank-based confidence interval.
 * C. Walter. Moving particles: a parallel optimal multilevel splitting
 * method with application in quantiles estimation and meta-model based
 * algorithms. Structural Safety, 55:10-25, 2015.  The article gives the
 * midpoint reduced-bias quantile estimator used by this implementation.
 */
class OT_API LastParticleQuantile
  : public QuantileSimulationImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  LastParticleQuantile();

  /** Constructor with parameters */
  LastParticleQuantile(const RandomVector & output,
                       const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                       const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Virtual constructor */
  LastParticleQuantile * clone() const override;

  /** Particle count accessor */
  void setParticleCount(const UnsignedInteger particleCount);
  UnsignedInteger getParticleCount() const;

  /** Sample size accessor */
  void setSampleSize(const UnsignedInteger sampleSize) override;

  /** Burn-in accessor */
  void setBurnIn(const UnsignedInteger burnIn);
  UnsignedInteger getBurnIn() const;

  /** Proposal range accessor */
  void setProposalRange(const Scalar proposalRange);
  Scalar getProposalRange() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Run the last-particle process */
  QuantileSimulationResult runFixedSample() override;

private:
  UnsignedInteger particleCount_ = ResourceMap::GetAsUnsignedInteger("LastParticleQuantile-DefaultParticleCount");
  UnsignedInteger burnIn_ = ResourceMap::GetAsUnsignedInteger("LastParticleQuantile-DefaultBurnIn");
  Scalar proposalRange_ = ResourceMap::GetAsScalar("SubsetSampling-DefaultProposalRange");

}; /* class LastParticleQuantile */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LASTPARTICLEQUANTILE_HXX */
