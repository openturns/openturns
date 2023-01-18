//                                               -*- C++ -*-
/**
 *  @brief This class is a Function with history of input and output.
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
#include "openturns/MemoizeFunction.hxx"
#include "openturns/MemoizeEvaluation.hxx"
#include "openturns/NoGradient.hxx"
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/NonCenteredFiniteDifferenceGradient.hxx"
#include "openturns/NoHessian.hxx"
#include "openturns/CenteredFiniteDifferenceHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MemoizeFunction)

static const Factory<MemoizeFunction> Factory_MemoizeFunction;

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
  // Modify the evaluation in the gradient and the hessian if they are based on finite difference
  // First try: CenteredFiniteDifferenceGradient
  {
    const CenteredFiniteDifferenceGradient * p_gradient = dynamic_cast<const CenteredFiniteDifferenceGradient *>(getGradient().getImplementation().get());
    if (p_gradient) setGradient(CenteredFiniteDifferenceGradient(p_gradient->getFiniteDifferenceStep(), getEvaluation()));
  }
  // Second try: NonCenteredFiniteDifferenceGradient
  {
    const NonCenteredFiniteDifferenceGradient * p_gradient = dynamic_cast<const NonCenteredFiniteDifferenceGradient *>(getGradient().getImplementation().get());
    if (p_gradient) setGradient(NonCenteredFiniteDifferenceGradient(p_gradient->getFiniteDifferenceStep(), getEvaluation()));
  }
  // Third try: CenteredFiniteDifferenceHessian
  {
    const CenteredFiniteDifferenceHessian * p_hessian = dynamic_cast<const CenteredFiniteDifferenceHessian *>(getHessian().getImplementation().get());
    if (p_hessian) setHessian(CenteredFiniteDifferenceHessian(p_hessian->getFiniteDifferenceStep(), getEvaluation()));
  }
  setUseDefaultGradientImplementation(function.getUseDefaultGradientImplementation());
  setUseDefaultHessianImplementation(function.getUseDefaultHessianImplementation());
}


/* Virtual constructor */
MemoizeFunction * MemoizeFunction::clone() const
{
  return new MemoizeFunction(*this);
}


/* String converter */
String MemoizeFunction::__repr__() const
{
  return FunctionImplementation::__repr__();
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
  MemoizeEvaluation* p_evaluation = dynamic_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  p_evaluation->setEvaluation(evaluation);
}

/* Enable or disable the internal cache */
void MemoizeFunction::enableCache() const
{
  MemoizeEvaluation* p_evaluation = dynamic_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  p_evaluation->enableCache();
}

void MemoizeFunction::disableCache() const
{
  MemoizeEvaluation* p_evaluation = dynamic_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  p_evaluation->disableCache();
}

Bool MemoizeFunction::isCacheEnabled() const
{
  MemoizeEvaluation* p_evaluation = dynamic_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->isCacheEnabled();
}

UnsignedInteger MemoizeFunction::getCacheHits() const
{
  MemoizeEvaluation* p_evaluation = dynamic_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->getCacheHits();
}

void MemoizeFunction::addCacheContent(const Sample & inSample, const Sample & outSample)
{
  MemoizeEvaluation* p_evaluation = dynamic_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  p_evaluation->addCacheContent(inSample, outSample);
}

Sample MemoizeFunction::getCacheInput() const
{
  MemoizeEvaluation* p_evaluation = dynamic_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->getCacheInput();
}

Sample MemoizeFunction::getCacheOutput() const
{
  MemoizeEvaluation* p_evaluation = dynamic_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->getCacheOutput();
}

void MemoizeFunction::clearCache() const
{
  MemoizeEvaluation* p_evaluation = dynamic_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->clearCache();
}


/* Enable or disable the input/output history */
void MemoizeFunction::enableHistory() const
{
  MemoizeEvaluation* p_evaluation = dynamic_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  p_evaluation->enableHistory();
}

void MemoizeFunction::disableHistory() const
{
  MemoizeEvaluation* p_evaluation = dynamic_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  p_evaluation->disableHistory();
}

/* Test the history mechanism activity */
Bool MemoizeFunction::isHistoryEnabled() const
{
  MemoizeEvaluation* p_evaluation = dynamic_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->isHistoryEnabled();
}


/** Clear history of the input and output values */
void MemoizeFunction::clearHistory() const
{
  MemoizeEvaluation* p_evaluation = dynamic_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  p_evaluation->clearHistory();
}

/** Retrieve the history of the input values */
Sample MemoizeFunction::getInputHistory() const
{
  MemoizeEvaluation* p_evaluation = dynamic_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->getInputHistory();
}

/** Retrieve the history of the output values */
Sample MemoizeFunction::getOutputHistory() const
{
  MemoizeEvaluation* p_evaluation = dynamic_cast<MemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->getOutputHistory();
}

/* Comparison operator */
Bool MemoizeFunction::operator ==(const MemoizeFunction & other) const
{
  if (this == &other) return true;
  return getEvaluation() == other.getEvaluation();
}

END_NAMESPACE_OPENTURNS
