//                                               -*- C++ -*-
/**
 * @brief Class for identity evaluation
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

#ifndef OPENTURNS_MEMOIZEEVALUATION_HXX
#define OPENTURNS_MEMOIZEEVALUATION_HXX

#include "openturns/EvaluationProxy.hxx"
#include "openturns/Evaluation.hxx"
#include "openturns/HistoryStrategy.hxx"
#include "openturns/Full.hxx"
#include "openturns/Cache.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Indices.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MemoizeEvaluation
 */
class OT_API MemoizeEvaluation
  : public EvaluationProxy
{
  CLASSNAME
public:

  typedef PersistentCollection<Scalar>                        CacheKeyType;
  typedef PersistentCollection<Scalar>                        CacheValueType;
  typedef Cache<CacheKeyType, CacheValueType>                 CacheType;
  typedef Pointer<CacheType>                                  CacheImplementation;

  /** Default constructor */
  MemoizeEvaluation();

  /** Parameter constructor */
  explicit MemoizeEvaluation(const Evaluation & evaluation, const HistoryStrategy & historyStrategy = Full());

  /** Virtual constructor */
  MemoizeEvaluation * clone() const override;

  /** Function implementation accessors */
  void setEvaluation(const Evaluation & evaluation);
  Evaluation getEvaluation() const;

  /** Comparison operator */
  using EvaluationProxy::operator ==;
  Bool operator ==(const MemoizeEvaluation & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */

  /** Operator () */
  Point operator() (const Point & inPoint) const override;

  /** Operator () */
  Sample operator() (const Sample & inSample) const override;

  /** Get the evaluation corresponding to indices components */
  using EvaluationProxy::getMarginal;
  Evaluation getMarginal(const Indices & indices) const override;

  /** Enable or disable the internal cache */
  void enableCache() const;
  void disableCache() const;

  /** @brief Test the internal cache activity
   * @see enableCache()
   */
  Bool isCacheEnabled() const;

  /** @brief Returns the number of successful hits in the cache
   */
  UnsignedInteger getCacheHits() const;

  /** @brief Add some content to the cache
   */
  void addCacheContent(const Sample & inSample, const Sample & outSample);

  /** @brief Returns the cache input
   */
  Sample getCacheInput() const;

  /** @brief Returns the cache output
   */
  Sample getCacheOutput() const;

  /** @brief Empty the cache
   */
  void clearCache() const;

  /** Enable or disable the input/output history */
  void enableHistory() const;
  void disableHistory() const;

  /** Test the history mechanism activity */
  Bool isHistoryEnabled() const;

  /** Clear history of the input and output values */
  void clearHistory() const;

  /** Retrieve the history of the input values */
  Sample getInputHistory() const;

  /** Retrieve the history of the output values */
  Sample getOutputHistory() const;

  /** Is it safe to call in parallel? */
  Bool isParallel() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** An history mechanism that allows one to remember all the input/output associations including duplicate calls */
  mutable HistoryStrategy inputStrategy_;
  mutable HistoryStrategy outputStrategy_;

  /** Flag to activate or deactivate the history mechanism */
  mutable Bool isHistoryEnabled_;

  /** A cache to store already computed points */
  mutable CacheImplementation p_cache_;

}; /* class MemoizeEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MEMOIZEEVALUATION_HXX */
