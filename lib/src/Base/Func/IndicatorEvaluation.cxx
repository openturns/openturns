//                                               -*- C++ -*-
/**
 * @brief The class that implements the composition between numerical
 *        math functions implementations
 *
 * Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#include "openturns/IndicatorEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Less.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IndicatorEvaluation)

static const Factory<IndicatorEvaluation> Factory_IndicatorEvaluation;

/* Default constructor */
IndicatorEvaluation::IndicatorEvaluation()
  : EvaluationImplementation()
  , p_evaluation_()
  , comparisonOperator_()
  , threshold_(0.0)
{
  // Nothing to do
} // IndicatorEvaluation

/* Default constructor */
IndicatorEvaluation::IndicatorEvaluation(const EvaluationPointer & p_evaluation,
    const ComparisonOperator & comparisonOperator,
    const Scalar threshold)
  : EvaluationImplementation()
  , p_evaluation_()
  , comparisonOperator_(comparisonOperator)
  , threshold_(threshold)
{
  setEvaluation(p_evaluation);
  setDescription(p_evaluation->getDescription());
} // IndicatorEvaluation

/* Virtual constructor */
IndicatorEvaluation * IndicatorEvaluation::clone() const
{
  return new IndicatorEvaluation(*this);
}


/* Comparison operator */
Bool IndicatorEvaluation::operator ==(const IndicatorEvaluation & other) const
{
  return true;
}

/* String converter */
String IndicatorEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << IndicatorEvaluation::GetClassName()
      << " name=" << getName()
      << " evaluation=" << p_evaluation_->__repr__()
      << " comparisonOperator=" << comparisonOperator_
      << " threshold=" << threshold_;
  return oss;
}

/* Operator () */
Point IndicatorEvaluation::operator() (const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  const Point result(1, (comparisonOperator_.compare(p_evaluation_->operator()(inP)[0], threshold_) ? 1.0 : 0.0));
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

/* Operator () */
Sample IndicatorEvaluation::operator() (const Sample & inSample) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  const UnsignedInteger size = inSample.getSize();
  if (inSample.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inSample.getDimension();
  const Point eval(p_evaluation_->operator()(inSample).asPoint());
  Sample result(size, 1);
  SampleImplementation & resultImpl(*result.getImplementation());
  for(UnsignedInteger i = 0; i < size; ++i) {
    resultImpl(i, 0) = comparisonOperator_.compare(eval[i], threshold_) ? 1.0 : 0.0;
  }

  callsNumber_ += size;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inSample);
    outputStrategy_.store(result);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger IndicatorEvaluation::getInputDimension() const
{
  return p_evaluation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger IndicatorEvaluation::getOutputDimension() const
{
  return 1;
}

/* Accessor for the underlying evaluation */
IndicatorEvaluation::EvaluationPointer IndicatorEvaluation::getEvaluation() const
{
  return p_evaluation_;
}

void IndicatorEvaluation::setEvaluation(const EvaluationPointer & p_evaluation)
{
  if (p_evaluation->getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot use an evaluation implementation with output dimension not equal to 1";
  p_evaluation_ = p_evaluation;
}

/* Accessor for the comparison operator */
ComparisonOperator IndicatorEvaluation::getComparisonOperator() const
{
  return comparisonOperator_;
}

void IndicatorEvaluation::setComparisonOperator(const ComparisonOperator & comparisonOperator)
{
  comparisonOperator_ = comparisonOperator;
}

/* Accessor for the threshold */
Scalar IndicatorEvaluation::getThreshold() const
{
  return threshold_;
}

void IndicatorEvaluation::setThreshold(const Scalar threshold)
{
  threshold_ = threshold;
}

/* Method save() stores the object through the StorageManager */
void IndicatorEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "evaluation_", *p_evaluation_ );
  adv.saveAttribute( "comparisonOperator_", comparisonOperator_ );
  adv.saveAttribute( "threshold_", threshold_ );
}

/* Method load() reloads the object from the StorageManager */
void IndicatorEvaluation::load(Advocate & adv)
{
  TypedInterfaceObject<EvaluationImplementation> evaluation;
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "evaluation_", evaluation );
  p_evaluation_ = evaluation.getImplementation();
  adv.loadAttribute( "comparisonOperator_", comparisonOperator_ );
  adv.loadAttribute( "threshold_", threshold_ );
}

END_NAMESPACE_OPENTURNS
