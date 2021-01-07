//                                               -*- C++ -*-
/**
 *  @brief MCMC base class
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
#ifndef OPENTURNS_MCMC_HXX
#define OPENTURNS_MCMC_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/RandomVectorImplementation.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/HistoryStrategy.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MCMC
 *
 * @brief MCMC base class
 *
 * This class is abstract so it can not be instanciated. It must be derived.
 */
class OT_API MCMC
  : public RandomVectorImplementation
{
  CLASSNAME
public:
  typedef Collection<Point> PointCollection;
  typedef PersistentCollection<Point> PointPersistentCollection;
  typedef Collection<PointWithDescription> PointWithDescriptionCollection;

  /** Default constructor */
  MCMC();

  /** Constructor with parameters*/
  MCMC(const Distribution & prior,
       const Distribution & conditional,
       const Sample & observations,
       const Point & initialState);

  /** Constructor with parameters*/
  MCMC(const Distribution & prior,
       const Distribution & conditional,
       const Function & model,
       const Sample & parameters,
       const Sample & observations,
       const Point & initialState);

  /** Virtual constructor */
  MCMC * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Compute the log-likelihood */
  Scalar computeLogLikelihood(const Point & currentState) const;

  /** Prior accessor */
  void setPrior(const Distribution & prior);
  Distribution getPrior() const;

  /** Conditional accessor */
  Distribution getConditional() const;

  /** Model accessor */
  Function getModel() const;

  /** Obervations accessor */
  void setObservations(const Sample & observations);
  Sample getObservations() const;

  /** Parameters accessor */
  void setParameters(const Sample & parameters);
  Sample getParameters() const;

  /// Burning accessor
  void setBurnIn(UnsignedInteger burnIn);
  UnsignedInteger getBurnIn() const;

  /// Thinning accessor
  void setThinning(UnsignedInteger thinning);
  UnsignedInteger getThinning() const;

  /** History accessor */
  void setHistory(const HistoryStrategy & strategy);
  HistoryStrategy getHistory() const;

  /** non-rejected components accessor */
  void setNonRejectedComponents(const Indices & nonRejectedComponents);
  Indices getNonRejectedComponents() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const override;

  /** Verbosity accessor */
  void setVerbose(const Bool verbose);
  Bool getVerbose() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  Point initialState_;
  mutable Point currentState_;
  mutable HistoryStrategy history_;
  Indices nonRejectedComponents_; // components that are always accepted

private:
  Distribution prior_;
  Distribution conditional_;

  mutable Function model_;
  Sample parameters_;
  Sample observations_;

  UnsignedInteger burnIn_; // number of first samples discarded to reach stationary regime
  UnsignedInteger thinning_; // number of samples skipped at each generation

  Bool verbose_;
}; /* class MCMC */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MCMC_HXX */
