//                                               -*- C++ -*-
/**
 *  @brief Metropolis-Hastings algorithm
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_USERDEFINEDMETROPOLISHASTINGS_HXX
#define OPENTURNS_USERDEFINEDMETROPOLISHASTINGS_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/MetropolisHastingsImplementation.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class UserDefinedMetropolisHastings
 *
 * @brief Metropolis-Hastings algorithm.
 *
 */
class OT_API UserDefinedMetropolisHastings
  : public MetropolisHastingsImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  UserDefinedMetropolisHastings();

  /** Constructor with parameters*/
  UserDefinedMetropolisHastings(const Distribution & targetDistribution,
                                const Point & initialState,
                                const Distribution & proposal,
                                const Function & linkFunction,
                                const Indices & marginalIndices = Indices());

  /** Constructor with parameters*/
  UserDefinedMetropolisHastings(const Function & targetLogPDF,
                                const Domain & support,
                                const Point & initialState,
                                const Distribution & proposal,
                                const Function & linkFunction,
                                const Indices & marginalIndices = Indices());
  /** String converter */
  String __repr__() const override;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  UserDefinedMetropolisHastings * clone() const override;

  /** Intrumental accessor */
  Distribution getProposal() const;
  Function getProposalLinkFunction() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Propose a new point in the chain */
  Point getCandidate() const override;

  /** Instrumental accessor */
  void setProposalAndLinkFunction(const Distribution & proposal,
                                  const Function & linkFunction);

private:

  // proposal distribution of the markov chain
  mutable Distribution proposal_;

  // link function to the proposal distribution
  Function proposalLinkFunction_;

}; /* class UserDefinedMetropolisHastings */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GenericMetropolisHastings_HXX */
