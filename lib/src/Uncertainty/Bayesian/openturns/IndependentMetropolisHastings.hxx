//                                               -*- C++ -*-
/**
 *  @brief Metropolis-Hastings algorithm
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
#ifndef OPENTURNS_INDEPENDENTMETROPOLISHASTINGS_HXX
#define OPENTURNS_INDEPENDENTMETROPOLISHASTINGS_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/MetropolisHastingsImplementation.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class IndependentMetropolisHastings
 *
 * @brief Metropolis-Hastings algorithm.
 *
 */
class OT_API IndependentMetropolisHastings
  : public MetropolisHastingsImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  IndependentMetropolisHastings();

  /** Constructor with parameters*/
  IndependentMetropolisHastings(const Distribution & targetDistribution,
                                const Point & initialState,
                                const Distribution & proposal,
                                const Indices & marginalIndices = Indices());

  /** Constructor with parameters*/
  IndependentMetropolisHastings(const Function & targetLogPDF,
                                const Domain & support,
                                const Point & initialState,
                                const Distribution & proposal,
                                const Indices & marginalIndices = Indices());
  /** String converter */
  String __repr__() const override;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  IndependentMetropolisHastings * clone() const override;

  /** Intrumental accessor */
  void setProposal(const Distribution & proposal);
  Distribution getProposal() const;

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

}; /* class IndependentMetropolisHastings */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INDEPENDENTMETROPOLISHASTINGS_HXX */
