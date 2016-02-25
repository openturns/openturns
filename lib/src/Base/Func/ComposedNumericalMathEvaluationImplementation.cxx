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

static const Factory<ComposedNumericalMathEvaluationImplementation> RegisteredFactory;

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

/* Parameters value and description accessor */
NumericalPointWithDescription ComposedNumericalMathEvaluationImplementation::getParameter() const
{
  const NumericalPointWithDescription rightParameters(p_rightFunction_->getParameter());
  const UnsignedInteger rightDimension(rightParameters.getDimension());
  const Description rightDescription(rightParameters.getDescription());
  const NumericalPointWithDescription leftParameters(p_leftFunction_->getParameter());
  const UnsignedInteger leftDimension(leftParameters.getDimension());
  const Description leftDescription(leftParameters.getDescription());
  const UnsignedInteger dimension(rightDimension + leftDimension);
  NumericalPointWithDescription parameters(dimension);
  Description description(dimension);
  UnsignedInteger index(0);
  for (UnsignedInteger i = 0; i < rightDimension; ++i)
  {
    parameters[index] = rightParameters[i];
    description[index] = rightDescription[i];
    ++index;
  }
  for (UnsignedInteger i = 0; i < leftDimension; ++i)
  {
    parameters[index] = leftParameters[i];
    description[index] = leftDescription[i];
    ++index;
  }
  parameters.setDescription(description);
  return parameters;
}

void ComposedNumericalMathEvaluationImplementation::setParameter(const NumericalPointWithDescription & parameters)
{
  NumericalPointWithDescription rightParameters(p_rightFunction_->getParameter());
  const UnsignedInteger rightDimension(rightParameters.getDimension());
  NumericalPointWithDescription leftParameters(p_leftFunction_->getParameter());
  const UnsignedInteger leftDimension(leftParameters.getDimension());
  const Description description(parameters.getDescription());
  Description rightDescription(rightDimension);
  Description leftDescription(leftDimension);
  UnsignedInteger index(0);
  for (UnsignedInteger i = 0; i < rightDimension; ++i)
  {
    rightParameters[i] = parameters[index];
    rightDescription[i] = description[index];
    ++index;
  }
  rightParameters.setDescription(rightDescription);
  p_rightFunction_->setParameter(rightParameters);
  for (UnsignedInteger i = 0; i < leftDimension; ++i)
  {
    leftParameters[i] = parameters[index];
    leftDescription[i] = description[index];
    ++index;
  }
  leftParameters.setDescription(leftDescription);
  p_leftFunction_->setParameter(leftParameters);
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
