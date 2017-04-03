//                                               -*- C++ -*-
/**
 *  @brief MCMC updates the chain
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_MCMC_HXX
#define OPENTURNS_MCMC_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/SamplerImplementation.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MCMC
 *
 * @brief The class implements the concept of comparison operator defined in Sampler.
 *
 * This class is abstract so it can not be instanciated. It must be derived.
 * @see Sampler
 */
class OT_API MCMC
  : public SamplerImplementation
{
  CLASSNAME;
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

  /** String converter */
  virtual String __repr__() const;

  /** Compute the likelihood w.r.t. observartions */
  NumericalScalar computeLogLikelihood(const Point & currentState) const;

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

  /** Virtual constructor */
  virtual MCMC * clone() const;

  /** Dimension accessor */
  virtual UnsignedInteger getDimension() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

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

}; /* class MCMC */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MCMC_HXX */
