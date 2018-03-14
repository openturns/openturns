//                                               -*- C++ -*-
/**
 * @brief The class that implements the composition between numerical
 *        math functions implementations
 *
 * Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#include "openturns/MemoizeEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MemoizeEvaluation)

static const Factory<MemoizeEvaluation> Factory_MemoizeEvaluation;

/* Default constructor */
MemoizeEvaluation::MemoizeEvaluation()
  : EvaluationProxy()
  , inputStrategy_(Full())
  , outputStrategy_(Full())
  , isHistoryEnabled_(true)
{
  // Nothing to do
}

/* Parameter constructor */
MemoizeEvaluation::MemoizeEvaluation(const Evaluation & evaluation, const HistoryStrategy & historyStrategy)
  : EvaluationProxy(evaluation)
  , inputStrategy_(historyStrategy)
  , outputStrategy_(historyStrategy)
  , isHistoryEnabled_(true)
{
  inputStrategy_.setDimension(evaluation_.getInputDimension());
  outputStrategy_.setDimension(evaluation_.getOutputDimension());
  // If argument is an MemoizeEvaluation, copy history
  MemoizeEvaluation * p_MemoizeEvaluation = dynamic_cast<MemoizeEvaluation*>(evaluation.getImplementation().get());
  if (p_MemoizeEvaluation)
  {
    const Sample inSample(p_MemoizeEvaluation->getInputHistory());
    const Sample outSample(p_MemoizeEvaluation->getOutputHistory());
    inputStrategy_.store(inSample);
    outputStrategy_.store(outSample);
    isHistoryEnabled_ = p_MemoizeEvaluation->isHistoryEnabled_;
  }
}

/* Virtual constructor */
MemoizeEvaluation * MemoizeEvaluation::clone() const
{
  return new MemoizeEvaluation(*this);
}

/** Function implementation accessors */
void MemoizeEvaluation::setEvaluation(const Evaluation & evaluation)
{
  evaluation_ = evaluation;
  inputStrategy_.setDimension(evaluation_.getInputDimension());
  outputStrategy_.setDimension(evaluation_.getOutputDimension());
  // If argument is a MemoizeEvaluation, copy history
  MemoizeEvaluation * p_MemoizeEvaluation = dynamic_cast<MemoizeEvaluation*>(evaluation.getImplementation().get());
  if (p_MemoizeEvaluation)
  {
    const Sample inSample(p_MemoizeEvaluation->getInputHistory());
    const Sample outSample(p_MemoizeEvaluation->getOutputHistory());
    inputStrategy_.store(inSample);
    outputStrategy_.store(outSample);
    isHistoryEnabled_ = p_MemoizeEvaluation->isHistoryEnabled_;
  }
}

/* Operator () */
Point MemoizeEvaluation::operator() (const Point & inPoint) const
{
  const Point outPoint(evaluation_.operator()(inPoint));
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inPoint);
    outputStrategy_.store(outPoint);
  }
  return outPoint;
}

/* Operator () */
Sample MemoizeEvaluation::operator() (const Sample & inSample) const
{
  Sample outSample(evaluation_.operator()(inSample));
  outSample.setDescription(evaluation_.getOutputDescription());
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inSample);
    outputStrategy_.store(outSample);
  }
  return outSample;
}

/* Get the evaluation corresponding to indices components */
Evaluation MemoizeEvaluation::getMarginal(const Indices & indices) const
{
  return new MemoizeEvaluation(evaluation_.getMarginal(indices), inputStrategy_);
}

/* Enable or disable the input/output history */
void MemoizeEvaluation::enableHistory() const
{
  isHistoryEnabled_ = true;
}

void MemoizeEvaluation::disableHistory() const
{
  isHistoryEnabled_ = false;
}

/* Test the history mechanism activity */
Bool MemoizeEvaluation::isHistoryEnabled() const
{
  return isHistoryEnabled_;
}

/* Clear history of the input and output values */
void MemoizeEvaluation::clearHistory() const
{
  inputStrategy_.clear();
  outputStrategy_.clear();
}

/* Retrieve the history of the input values */
Sample MemoizeEvaluation::getInputHistory() const
{
  return inputStrategy_.getSample();
}

/* Retrieve the history of the output values */
Sample MemoizeEvaluation::getOutputHistory() const
{
  return outputStrategy_.getSample();
}

Evaluation MemoizeEvaluation::getEvaluation() const
{
  return evaluation_;
}

/* Comparison operator */
Bool MemoizeEvaluation::operator ==(const MemoizeEvaluation & other) const
{
  return evaluation_ == other.evaluation_;
}

/* String converter */
String MemoizeEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << MemoizeEvaluation::GetClassName()
      << " evaluation=" << evaluation_.getImplementation()->__repr__()
      << " isHistoryEnabled=" << isHistoryEnabled_
      << " inputStrategy=" << inputStrategy_
      << " outputStrategy=" << outputStrategy_;
  return oss;
}

String MemoizeEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << MemoizeEvaluation::GetClassName()
      << " evaluation=" << evaluation_.getImplementation()->__str__()
      << " isHistoryEnabled=" << isHistoryEnabled_
      << " inputStrategy=" << inputStrategy_
      << " outputStrategy=" << outputStrategy_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void MemoizeEvaluation::save(Advocate & adv) const
{
  EvaluationProxy::save(adv);
  adv.saveAttribute( "inputStrategy_", inputStrategy_ );
  adv.saveAttribute( "outputStrategy_", outputStrategy_ );
  adv.saveAttribute( "isHistoryEnabled_", isHistoryEnabled_ );
}

/* Method load() reloads the object from the StorageManager */
void MemoizeEvaluation::load(Advocate & adv)
{
  EvaluationProxy::load(adv);
  adv.loadAttribute( "inputStrategy_", inputStrategy_ );
  adv.loadAttribute( "outputStrategy_", outputStrategy_ );
  adv.loadAttribute( "isHistoryEnabled_", isHistoryEnabled_ );
}

END_NAMESPACE_OPENTURNS
