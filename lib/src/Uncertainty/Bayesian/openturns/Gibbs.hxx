//                                               -*- C++ -*-
/**
 *  @brief Gibbs algorithm
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
#ifndef OPENTURNS_GIBBS_HXX
#define OPENTURNS_GIBBS_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/MetropolisHastings.hxx"
#include "openturns/Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Gibbs
 *
 * @brief Gibbs algorithm.
 *
 */
class OT_API Gibbs
  : public RandomVectorImplementation
{
  CLASSNAME
public:
  typedef Collection<MetropolisHastings> MetropolisHastingsCollection;
  typedef PersistentCollection<MetropolisHastings> MetropolisHastingsPersistentCollection;

  /** Default constructor */
  Gibbs();

  /** Constructor with parameters*/
  Gibbs(const MetropolisHastingsCollection & samplers);

  /** String converter */
  String __repr__() const override;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  Gibbs * clone() const override;

  /** Samplers accessor */
  MetropolisHastingsCollection getMetropolisHastingsCollection() const;

  /** Burnin accessor */
  void setBurnIn(const UnsignedInteger burnIn);
  UnsignedInteger getBurnIn() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Dimension accessor */
  UnsignedInteger getDimension() const override;

  /** Sampling method accessors */
  enum UpdatingMethod { DETERMINISTIC_UPDATING, RANDOM_UPDATING };
  void setUpdatingMethod(const UpdatingMethod updatingMethod);
  UpdatingMethod getUpdatingMethod() const;

  /** Propose a new point in the chain */
  Point getRealization() const override;

  /** Whether the posterior of a bloc is recomputed */
  Indices getRecomputeLogPosterior() const;

  /** History accessor */
  void setHistory(const HistoryStrategy & strategy);
  HistoryStrategy getHistory() const;

protected:
  // Sequentially sample from the MH blocks
  void computeRealizationDeterministicUpdating() const;

  // Sample from a randomly chosen MH block
  void computeRealizationRandomUpdating() const;

  mutable Point currentState_;
  mutable HistoryStrategy history_;

private:
  // total number of samples
  mutable UnsignedInteger samplesNumber_ = 0;

  // collection of MH samplers
  MetropolisHastingsPersistentCollection samplers_;

  // sampling method: determines in which order the MH samplers are called
  UnsignedInteger updatingMethod_ = 0;

  // which MH sampler was previously called (used when updatingMethod_ is RANDOM_UPDATING)
  mutable UnsignedInteger previouslyChosenSampler_ = 0;

  // number of first samples discarded to reach stationary regime
  UnsignedInteger burnIn_ = 0;

  mutable Indices recomputeLogPosterior_;

  // unnormalized log-posterior density of the current state
  mutable Scalar currentLogPosterior_ = 0.0;
}; /* class Gibbs */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GIBBS_HXX */
