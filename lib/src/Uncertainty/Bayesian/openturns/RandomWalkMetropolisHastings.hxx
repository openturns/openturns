//                                               -*- C++ -*-
/**
 *  @brief Metropolis-Hastings algorithm
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
#ifndef OPENTURNS_RANDOMWALKMETROPOLISHASTINGS_HXX
#define OPENTURNS_RANDOMWALKMETROPOLISHASTINGS_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/MetropolisHastingsImplementation.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RandomWalkMetropolisHastings
 *
 * @brief Metropolis-Hastings algorithm.
 *
 */
class OT_API RandomWalkMetropolisHastings
  : public MetropolisHastingsImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  RandomWalkMetropolisHastings();

  /** Constructor with parameters*/
  RandomWalkMetropolisHastings(const Distribution & targetDistribution,
                               const Point & initialState,
                               const Distribution & proposal,
                               const Indices & marginalIndices = Indices());

  /** Constructor with parameters*/
  RandomWalkMetropolisHastings(const Function & targetLogPDF,
                              const Domain & support,
                              const Point & initialState,
                              const Distribution & proposal,
                              const Indices & marginalIndices = Indices());
  /** String converter */
  String __repr__() const override;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  RandomWalkMetropolisHastings * clone() const override;

  /** Intrumental accessor */
  void setProposal(const Distribution & proposal);
  Distribution getProposal() const;

  /** Adaptation range accessor */
  void setAdaptationRange(const Interval & range);
  Interval getAdaptationRange() const;

  /** Adaptation expansion factor accessor */
  void setAdaptationExpansionFactor(const Scalar expansionFactor);
  Scalar getAdaptationExpansionFactor() const;

  /** Adaptation shrink factor accessor */
  void setAdaptationShrinkFactor(const Scalar shrinkFactor);
  Scalar getAdaptationShrinkFactor() const;

  /** Adaptation period accessor */
  void setAdaptationPeriod(const UnsignedInteger period);
  UnsignedInteger getAdaptationPeriod() const;

  /** Adaptation factor accessor */
  Scalar getAdaptationFactor() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Propose a new point in the chain */
  Point getCandidate() const override;

private:
  // proposal densities of the markov chain
  Distribution proposal_;
  Bool proposalIsSymmetric_;

  // update factor
  mutable Scalar adaptationFactor_ = 1.0;

  // adaptation parameters
  Interval adaptationRange_;
  Scalar adaptationExpansionFactor_ = 0.0;
  Scalar adaptationShrinkFactor_ = 0.0;
  UnsignedInteger adaptationPeriod_ = 0;

}; /* class RandomWalkMetropolisHastings */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RANDOMWALKMETROPOLISHASTINGS_HXX */
