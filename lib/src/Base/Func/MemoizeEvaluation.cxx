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
MemoizeEvaluation::MemoizeEvaluation(const Implementation & p_Evaluation, const HistoryStrategy & historyStrategy)
  : EvaluationProxy(p_Evaluation)
  , inputStrategy_(historyStrategy)
  , outputStrategy_(historyStrategy)
  , isHistoryEnabled_(true)
{
  inputStrategy_.setDimension(p_evaluationImplementation_->getInputDimension());
  outputStrategy_.setDimension(p_evaluationImplementation_->getOutputDimension());
  // If argument is an MemoizeEvaluation, copy history
  MemoizeEvaluation * p_MemoizeEvaluation = dynamic_cast<MemoizeEvaluation*>(p_Evaluation.get());
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
void MemoizeEvaluation::setEvaluation(const Implementation & p_Evaluation)
{
  p_evaluationImplementation_ = p_Evaluation;
  inputStrategy_.setDimension(p_evaluationImplementation_->getInputDimension());
  outputStrategy_.setDimension(p_evaluationImplementation_->getOutputDimension());
  // If argument is a MemoizeEvaluation, copy history
  MemoizeEvaluation * p_MemoizeEvaluation = dynamic_cast<MemoizeEvaluation*>(p_Evaluation.get());
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
  const Point outPoint(p_evaluationImplementation_->operator()(inPoint));
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
  Sample outSample(p_evaluationImplementation_->operator()(inSample));
  outSample.setDescription(p_evaluationImplementation_->getOutputDescription());
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inSample);
    outputStrategy_.store(outSample);
  }
  return outSample;
}

/* Get the evaluation corresponding to indices components */
MemoizeEvaluation::Implementation MemoizeEvaluation::getMarginal(const Indices & indices) const
{
  Pointer<EvaluationImplementation> marginal = p_evaluationImplementation_->getMarginal(indices);
  return new MemoizeEvaluation(marginal.get()->clone(), inputStrategy_);
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

MemoizeEvaluation::Implementation MemoizeEvaluation::getEvaluation() const
{
  return p_evaluationImplementation_;
}

/* Comparison operator */
Bool MemoizeEvaluation::operator ==(const MemoizeEvaluation & other) const
{
  return p_evaluationImplementation_ == other.p_evaluationImplementation_;
}

/* String converter */
String MemoizeEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << MemoizeEvaluation::GetClassName()
      << " evaluation=" << p_evaluationImplementation_->__repr__()
      << " isHistoryEnabled=" << isHistoryEnabled_
      << " inputStrategy=" << inputStrategy_
      << " outputStrategy=" << outputStrategy_;
  return oss;
}

String MemoizeEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << MemoizeEvaluation::GetClassName()
      << " evaluation=" << p_evaluationImplementation_->__str__()
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
