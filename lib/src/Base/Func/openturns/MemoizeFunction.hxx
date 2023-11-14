//                                               -*- C++ -*-
/**
 *  @brief This class is a Function with history of input and output.
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
#ifndef OPENTURNS_MEMOIZEFUNCTION_HXX
#define OPENTURNS_MEMOIZEFUNCTION_HXX

#include "openturns/FunctionImplementation.hxx"
#include "openturns/Function.hxx"
#include "openturns/Evaluation.hxx"
#include "openturns/HistoryStrategy.hxx"
#include "openturns/Full.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MemoizeFunction
 *
 * This class is a Function with history of input and output.
 */
class OT_API MemoizeFunction
  : public FunctionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  MemoizeFunction();

  /** Constructor with parameters */
  explicit MemoizeFunction(const Function & function, const HistoryStrategy & historyStrategy = Full());

  /** Virtual copy constructor */
  MemoizeFunction * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Function implementation accessors */
  void setEvaluation(const Evaluation & evaluation) override;

  /** Comparison operator */
  using FunctionImplementation::operator ==;
  Bool operator ==(const MemoizeFunction & other) const;

  /** Operator () */
  Point operator() (const Point & inPoint) const override;

  /** Operator () */
  Sample operator() (const Sample & inSample) const override;

  /** @brief Enable the internal cache
   *
   * The cache stores previously computed output values, so calling the cache before processing the %Function
   * can save much time and avoid useless computations. However, calling the cache can eat time if the computation is
   * very short. So cache is disabled by default, except when the underlying implementation uses a wrapper.
   *
   * The reason is that building and linking to a wrapper is an extra burden that is valuable only if the computation
   * code is long enough to justify it. Calling the cache in this case will save time for sure.
   */
  void enableCache() const;

  /** @brief Disable the internal cache
   * @see enableCache()
   */
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

}; /* class MemoizeFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MEMOIZEFUNCTION_HXX */
