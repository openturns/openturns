//                                               -*- C++ -*-
/**
 *  @brief MetropolisHastingsImplementation base class
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
#ifndef OPENTURNS_METROPOLISHASTINGSIMPLEMENTATION_HXX
#define OPENTURNS_METROPOLISHASTINGSIMPLEMENTATION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/RandomVectorImplementation.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/HistoryStrategy.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @brief MetropolisHastingsImplementation base class
 */
class OT_API MetropolisHastingsImplementation
  : public RandomVectorImplementation
{
  CLASSNAME
  friend class Gibbs;
public:

  /** Default constructor */
  MetropolisHastingsImplementation();

  /** Constructor with parameters*/
  MetropolisHastingsImplementation(const Point & initialState,
                                   const Indices & marginalIndices = Indices());

  /** Constructor with parameters*/
  MetropolisHastingsImplementation(const Distribution & targetDistribution,
                                   const Point & initialState,
                                   const Indices & marginalIndices = Indices());

  /** Constructor with parameters*/
  MetropolisHastingsImplementation(const Function & targetLogPDF,
                                   const Domain & support,
                                   const Point & initialState,
                                   const Indices & marginalIndices = Indices());

  /** Virtual constructor */
  MetropolisHastingsImplementation * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Get a realization */
  Point getRealization() const override;

  /** Compute the log-likelihood */
  virtual Scalar computeLogLikelihood(const Point & state) const;

  /** Compute the log-likelihood */
  virtual Scalar computeLogPosterior(const Point & state) const;

  /** Target distribution accessor */
  virtual Distribution getTargetDistribution() const;

  /** Target log-pdf accessor */
  virtual Function getTargetLogPDF() const;
  virtual Domain getTargetLogPDFSupport() const;

  /** Initial state accessor */
  virtual Point getInitialState() const;

  /** Marginal indices accessor */
  virtual Indices getMarginalIndices() const;

  /** Likelihood accessor */
  virtual void setLikelihood(const Distribution & conditional,
                             const Sample & observations,
                             const Function & linkFunction = Function(),
                             const Sample & covariates = Sample(0, 0));

  /** Conditional accessor */
  Distribution getConditional() const;

  /** Link function accessor */
  Function getLinkFunction() const;

  /** Obervations accessor */
  Sample getObservations() const;

  /** Covariates accessor */
  Sample getCovariates() const;

  /** Burnin accessor */
  void setBurnIn(const UnsignedInteger burnIn);
  UnsignedInteger getBurnIn() const;

  /** Thinning accessor */
  void setThinning(const UnsignedInteger thinning);
  UnsignedInteger getThinning() const;

  /** History accessor */
  void setHistory(const HistoryStrategy & strategy);
  HistoryStrategy getHistory() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const override;

  /** Verbosity accessor */
  void setVerbose(const Bool verbose);
  Bool getVerbose() const;

  /** Acceptance rate accessor*/
  Scalar getAcceptanceRate() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  Point initialState_;
  mutable Point currentState_;
  mutable Scalar logProbCurrentConditionedToNew_ = 0.0;
  mutable Scalar logProbNewConditionedToCurrent_ = 0.0;
  Indices marginalIndices_;
  mutable HistoryStrategy history_;

  // total number of samples
  mutable UnsignedInteger samplesNumber_ = 0;

  // number of accepted candidates
  mutable UnsignedInteger acceptedNumber_ = 0;

  // number of accepted candidates since adaptation
  mutable UnsignedInteger acceptedNumberAdaptation_ = 0;

  /** Propose a new point in the chain */
  virtual Point getCandidate() const;

  // Set conditional probabilities of new and current state w.r.t. one another
  void setConditionalLogProbabilities(const Scalar logProbNewConditionedToCurrent, const Scalar logProbCurrentConditionedToNew) const;

private:
  // target distribution
  Distribution targetDistribution_;
  Bool hasTargetDistribution_ = false;

  // ... when defined via its log-pdf
  Function targetLogPDF_;
  Domain support_;

  // likelihood term
  Distribution conditional_;
  Function linkFunction_;
  Sample covariates_;
  Sample observations_;

  // number of first samples discarded to reach stationary regime
  UnsignedInteger burnIn_ = 0;

  // number of samples skipped at each generation
  UnsignedInteger thinning_ = 0;

  // verbosity flag
  Bool verbose_ = false;

  // unnormalized log-posterior density of the current state
  mutable Scalar currentLogPosterior_ = 0.0;

  // prior log pdf
  virtual Scalar computeLogPDFPrior(const Point & state) const;

  void setTargetDistribution(const Distribution & targetDistribution);
  void setTargetLogPDF(const Function & targetLogPDF, const Domain & support);
  void setMarginalIndices(const Indices & marginalIndices);

}; /* class MetropolisHastingsImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_METROPOLISHASTINGSIMPLEMENTATION_HXX */
