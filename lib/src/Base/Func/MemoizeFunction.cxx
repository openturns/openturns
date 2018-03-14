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
#include "openturns/MemoizeFunction.hxx"
#include "openturns/MemoizeEvaluation.hxx"
#include "openturns/NoGradient.hxx"
#include "openturns/NoHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MemoizeFunction)

/* Default constructor */
MemoizeFunction::MemoizeFunction ()
  : FunctionImplementation()
{
  // Nothing to do
}


/* Parameter constructor */
MemoizeFunction::MemoizeFunction (const Function & function, const HistoryStrategy & historyStrategy)
  : FunctionImplementation(new MemoizeEvaluation(function.getEvaluation(), historyStrategy),
                           function.getGradient(),
                           function.getHessian())
{
  // Nothing to do
}

/** Operator () */
Point MemoizeFunction::operator() (const Point & inPoint) const
{
  return getEvaluation().operator()(inPoint);
}

/** Operator () */
Sample MemoizeFunction::operator() (const Sample & inSample) const
{
  return getEvaluation().operator()(inSample);
}

/* Function implementation accessors */
void MemoizeFunction::setEvaluation(const Evaluation & evaluation)
{
  // Ensure that p_evaluation_ always points to an MemoizeEvaluation
  MemoizeEvaluation* p_evaluation = static_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  p_evaluation->setEvaluation(evaluation);
}

/* Enable or disable the input/output history */
void MemoizeFunction::enableHistory() const
{
  MemoizeEvaluation* p_evaluation = static_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  p_evaluation->enableHistory();
}

void MemoizeFunction::disableHistory() const
{
  MemoizeEvaluation* p_evaluation = static_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  p_evaluation->disableHistory();
}

/* Test the history mechanism activity */
Bool MemoizeFunction::isHistoryEnabled() const
{
  MemoizeEvaluation* p_evaluation = static_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->isHistoryEnabled();
}


/** Clear history of the input and output values */
void MemoizeFunction::clearHistory() const
{
  MemoizeEvaluation* p_evaluation = static_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  p_evaluation->clearHistory();
}

/** Retrieve the history of the input values */
Sample MemoizeFunction::getInputHistory() const
{
  MemoizeEvaluation* p_evaluation = static_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->getInputHistory();
}

/** Retrieve the history of the output values */
Sample MemoizeFunction::getOutputHistory() const
{
  MemoizeEvaluation* p_evaluation = static_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->getOutputHistory();
}

/* Comparison operator */
Bool MemoizeFunction::operator ==(const MemoizeFunction & other) const
{
  if (this == &other) return true;
  return getEvaluation() == other.getEvaluation();
}

END_NAMESPACE_OPENTURNS
