//                                               -*- C++ -*-
/**
 * @brief The class that implements the composition between numerical
 *        math functions implementations
 *
 * Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#include "openturns/ComposedNumericalMathEvaluationImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ComposedNumericalMathEvaluationImplementation);

static const Factory<ComposedNumericalMathEvaluationImplementation> Factory_ComposedNumericalMathEvaluationImplementation;

/* Default constructor */
ComposedNumericalMathEvaluationImplementation::ComposedNumericalMathEvaluationImplementation(const EvaluationImplementation & p_leftFunction,
    const EvaluationImplementation & p_rightFunction)
  : NumericalMathEvaluationImplementation()
  , p_leftFunction_(p_leftFunction)
  , p_rightFunction_(p_rightFunction)
{
  // Check if the dimensions of the left and right functions are compatible
  if (p_leftFunction->getInputDimension() != p_rightFunction->getOutputDimension()) throw InvalidArgumentException(HERE) << "The input dimension of the left function must be equal to the output dimension of the right function to compose them";
  setInputDescription(p_rightFunction->getInputDescription());
  setOutputDescription(p_leftFunction->getOutputDescription());
}

/* Virtual constructor */
ComposedNumericalMathEvaluationImplementation * ComposedNumericalMathEvaluationImplementation::clone() const
{
  return new ComposedNumericalMathEvaluationImplementation(*this);
}

/* Comparison operator */
Bool ComposedNumericalMathEvaluationImplementation::operator ==(const ComposedNumericalMathEvaluationImplementation & other) const
{
  return true;
}

/* Get the i-th marginal function */
ComposedNumericalMathEvaluationImplementation::Implementation ComposedNumericalMathEvaluationImplementation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return new ComposedNumericalMathEvaluationImplementation(p_leftFunction_->getMarginal(i), p_rightFunction_);
}

/* Get the function corresponding to indices components */
ComposedNumericalMathEvaluationImplementation::Implementation ComposedNumericalMathEvaluationImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension() - 1)) throw InvalidArgumentException(HERE) << "The indices of a marginal function must be in the range [0, dim-1] and  must be different";
  return new ComposedNumericalMathEvaluationImplementation(p_leftFunction_->getMarginal(indices), p_rightFunction_);
}


/* String converter */
String ComposedNumericalMathEvaluationImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ComposedNumericalMathEvaluationImplementation::GetClassName()
      << " name=" << getName()
      << " leftFunction=" << p_leftFunction_->__repr__()
      << " rightFunction=" << p_rightFunction_->__repr__();
  return oss;
}

String ComposedNumericalMathEvaluationImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  if (hasVisibleName()) oss << offset << "name=" << getName() << "\n";
  oss << "(" << p_leftFunction_->__str__(offset) << ")o(";
  oss << p_rightFunction_->__str__(offset) << ")";
  return oss;
}

/* Operator () */
NumericalPoint ComposedNumericalMathEvaluationImplementation::operator() (const NumericalPoint & inP) const
{
  if (inP.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a NumericalMathFunction with an argument of invalid dimension";
  ++callsNumber_;
  const NumericalPoint rightValue(p_rightFunction_->operator()(inP));
  const NumericalPoint leftValue(p_leftFunction_->operator()(rightValue));
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(leftValue);
  }
  return leftValue;
}

/* Operator () */
NumericalSample ComposedNumericalMathEvaluationImplementation::operator() (const NumericalSample & inSample) const
{
  callsNumber_ += inSample.getSize();
  const NumericalSample rightSample(p_rightFunction_->operator()(inSample));
  NumericalSample leftSample(p_leftFunction_->operator()(rightSample));
  leftSample.setDescription(getOutputDescription());
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inSample);
    outputStrategy_.store(leftSample);
  }
  return leftSample;
}

/* Parameters value accessor */
NumericalPoint ComposedNumericalMathEvaluationImplementation::getParameter() const
{
  NumericalPoint parameter(p_rightFunction_->getParameter());
  parameter.add(p_leftFunction_->getParameter());
  return parameter;
}

void ComposedNumericalMathEvaluationImplementation::setParameter(const NumericalPoint & parameter)
{
  NumericalPoint rightParameter(p_rightFunction_->getParameter());
  const UnsignedInteger rightDimension(rightParameter.getDimension());
  NumericalPoint leftParameter(p_leftFunction_->getParameter());
  const UnsignedInteger leftDimension(leftParameter.getDimension());
  UnsignedInteger index(0);
  for (UnsignedInteger i = 0; i < rightDimension; ++ i)
  {
    rightParameter[i] = parameter[index];
    ++ index;
  }
  p_rightFunction_->setParameter(rightParameter);
  for (UnsignedInteger i = 0; i < leftDimension; ++ i)
  {
    leftParameter[i] = parameter[index];
    ++ index;
  }
  p_leftFunction_->setParameter(leftParameter);
}

/* Parameters description accessor */
Description ComposedNumericalMathEvaluationImplementation::getParameterDescription() const
{
  Description description(p_rightFunction_->getParameterDescription());
  description.add(p_leftFunction_->getParameterDescription());
  return description;
}

void ComposedNumericalMathEvaluationImplementation::setParameterDescription(const Description & description)
{
  Description rightDescription(p_rightFunction_->getParameterDescription());
  const UnsignedInteger rightDimension(rightDescription.getSize());
  Description leftDescription(p_leftFunction_->getParameterDescription());
  const UnsignedInteger leftDimension(leftDescription.getSize());
  UnsignedInteger index(0);
  for (UnsignedInteger i = 0; i < rightDimension; ++ i)
  {
    rightDescription[i] = description[index];
    ++ index;
  }
  p_rightFunction_->setParameterDescription(rightDescription);
  for (UnsignedInteger i = 0; i < leftDimension; ++ i)
  {
    leftDescription[i] = description[index];
    ++ index;
  }
  p_leftFunction_->setParameterDescription(leftDescription);
}

/* Accessor for input point dimension */
UnsignedInteger ComposedNumericalMathEvaluationImplementation::getInputDimension() const
{
  return p_rightFunction_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger ComposedNumericalMathEvaluationImplementation::getOutputDimension() const
{
  return p_leftFunction_->getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void ComposedNumericalMathEvaluationImplementation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "leftFunction_", *p_leftFunction_ );
  adv.saveAttribute( "rightFunction_", *p_rightFunction_ );
}

/* Method load() reloads the object from the StorageManager */
void ComposedNumericalMathEvaluationImplementation::load(Advocate & adv)
{
  TypedInterfaceObject<NumericalMathEvaluationImplementation> evaluationValue;
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute( "leftFunction_", evaluationValue );
  p_leftFunction_ = evaluationValue.getImplementation();
  adv.loadAttribute( "rightFunction_", evaluationValue );
  p_rightFunction_ = evaluationValue.getImplementation();
}

/* Composed implementation accessor */
ComposedNumericalMathEvaluationImplementation::EvaluationImplementation ComposedNumericalMathEvaluationImplementation::getLeftEvaluationImplementation() const
{
  return p_leftFunction_;
}

ComposedNumericalMathEvaluationImplementation::EvaluationImplementation ComposedNumericalMathEvaluationImplementation::getRightEvaluationImplementation() const
{
  return p_rightFunction_;
}

END_NAMESPACE_OPENTURNS
