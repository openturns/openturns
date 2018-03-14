//                                               -*- C++ -*-
/**
 *  @brief This class is a Function with history of input and output.
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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

  /** Function implementation accessors */
  void setEvaluation(const Evaluation & evaluation);

  /** Comparison operator */
  Bool operator ==(const MemoizeFunction & other) const;

  /** Operator () */
  virtual Point operator() (const Point & inPoint) const;

  /** Operator () */
  virtual Sample operator() (const Sample & inSample) const;

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
