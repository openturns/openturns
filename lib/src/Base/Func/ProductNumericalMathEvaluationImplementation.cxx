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
 * \author $LastChangedBy: lebrun $
 * \date   $LastChangedDate: 2008-07-03 08:31:34 +0200 (jeu, 03 jui 2008) $
 */

#include "openturns/ProductNumericalMathEvaluationImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProductNumericalMathEvaluationImplementation);

static const Factory<ProductNumericalMathEvaluationImplementation> RegisteredFactory;

/* Default constructor */
ProductNumericalMathEvaluationImplementation::ProductNumericalMathEvaluationImplementation(const EvaluationImplementation & p_leftEvaluation,
    const EvaluationImplementation & p_rightEvaluation)
  : NumericalMathEvaluationImplementation()
  , p_leftEvaluation_(p_leftEvaluation)
  , p_rightEvaluation_(p_rightEvaluation)
{
  // Check if the dimensions of the left and right functions are compatible
  if (p_leftEvaluation->getOutputDimension() != 1)
  {
    if (p_rightEvaluation->getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the output dimension of at least one of the functions must be 1 in a product.";
    LOGWARN("We exchanged the functions in order to have the left one with output dimension equals to 1");
    p_leftEvaluation_ = p_rightEvaluation;
    p_rightEvaluation_ = p_leftEvaluation;
  }
  if (p_leftEvaluation_->getInputDimension() != p_rightEvaluation_->getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the two functions must have the same input dimension.";
  setDescription(p_rightEvaluation_->getDescription());
}

/* Virtual constructor */
ProductNumericalMathEvaluationImplementation * ProductNumericalMathEvaluationImplementation::clone() const
{
  return new ProductNumericalMathEvaluationImplementation(*this);
}

/* Comparison operator */
Bool ProductNumericalMathEvaluationImplementation::operator ==(const ProductNumericalMathEvaluationImplementation & other) const
{
  return true;
}

/* String converter */
String ProductNumericalMathEvaluationImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ProductNumericalMathEvaluationImplementation::GetClassName()
      << " name=" << getName()
      << " leftEvaluation=" << p_leftEvaluation_->__repr__()
      << " rightEvaluation=" << p_rightEvaluation_->__repr__();
  return oss;
}

String ProductNumericalMathEvaluationImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset;
  if (hasVisibleName()) oss << getName() << " :";
  if (getOutputDimension() > 1) oss << "\n";
  oss << "(" << p_leftEvaluation_->__str__(offset) << ") * ("
      << "(" << p_rightEvaluation_->__str__(offset) << ")";
  return oss;
}

/* Operator () */
NumericalPoint ProductNumericalMathEvaluationImplementation::operator() (const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension(getInputDimension());
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  const NumericalPoint left(p_leftEvaluation_->operator()(inP));
  const NumericalPoint right(p_rightEvaluation_->operator()(inP));
  const NumericalPoint result(left[0] * right);
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

/* Operator () */
NumericalSample ProductNumericalMathEvaluationImplementation::operator() (const NumericalSample & inSample) const
{
  const UnsignedInteger inputDimension(getInputDimension());
  if (inSample.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inSample.getDimension();
  const UnsignedInteger size(inSample.getSize());
  callsNumber_ += size;
  const NumericalSample leftSample(p_leftEvaluation_->operator()(inSample));
  NumericalSample rightSample(p_rightEvaluation_->operator()(inSample));
  for (UnsignedInteger i = 0; i < size; ++i) rightSample[i] *= leftSample[i][0];
  rightSample.setDescription(getOutputDescription());
  return rightSample;
}

/* Parameters value and description accessor */
NumericalPointWithDescription ProductNumericalMathEvaluationImplementation::getParameter() const
{
  NumericalPointWithDescription rightParameters(p_rightEvaluation_->getParameter());
  UnsignedInteger rightDimension(rightParameters.getDimension());
  Description rightDescription(rightParameters.getDescription());
  NumericalPointWithDescription leftParameters(p_leftEvaluation_->getParameter());
  UnsignedInteger leftDimension(leftParameters.getDimension());
  Description leftDescription(leftParameters.getDescription());
  UnsignedInteger dimension(rightDimension + leftDimension);
  NumericalPointWithDescription parameters(dimension);
  Description description(dimension);
  UnsignedInteger index(0);
  for (UnsignedInteger i = 0; i < leftDimension; i++)
  {
    parameters[index] = leftParameters[i];
    description[index] = leftDescription[i];
    index++;
  }
  for (UnsignedInteger i = 0; i < rightDimension; i++)
  {
    parameters[index] = rightParameters[i];
    description[index] = rightDescription[i];
    index++;
  }
  parameters.setDescription(description);
  return parameters;
}

void ProductNumericalMathEvaluationImplementation::setParameter(const NumericalPointWithDescription & parameters)
{
  NumericalPointWithDescription rightParameters(p_rightEvaluation_->getParameter());
  UnsignedInteger rightDimension(rightParameters.getDimension());
  NumericalPointWithDescription leftParameters(p_leftEvaluation_->getParameter());
  UnsignedInteger leftDimension(leftParameters.getDimension());
  Description description(parameters.getDescription());
  Description rightDescription(rightDimension);
  Description leftDescription(leftDimension);
  UnsignedInteger index(0);
  for (UnsignedInteger i = 0; i < leftDimension; ++i)
  {
    leftParameters[i] = parameters[index];
    leftDescription[i] = description[index];
    index++;
  }
  leftParameters.setDescription(leftDescription);
  p_leftEvaluation_->setParameter(leftParameters);
  for (UnsignedInteger i = 0; i < rightDimension; ++i)
  {
    rightParameters[i] = parameters[index];
    rightDescription[i] = description[index];
    index++;
  }
  rightParameters.setDescription(rightDescription);
  p_rightEvaluation_->setParameter(rightParameters);
}

/* Accessor for input point dimension */
UnsignedInteger ProductNumericalMathEvaluationImplementation::getInputDimension() const
{
  return p_rightEvaluation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger ProductNumericalMathEvaluationImplementation::getOutputDimension() const
{
  return p_rightEvaluation_->getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void ProductNumericalMathEvaluationImplementation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "leftEvaluation_", *p_leftEvaluation_ );
  adv.saveAttribute( "rightEvaluation_", *p_rightEvaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductNumericalMathEvaluationImplementation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.saveAttribute( "leftEvaluation_", *p_leftEvaluation_ );
  adv.saveAttribute( "rightEvaluation_", *p_rightEvaluation_ );
}

END_NAMESPACE_OPENTURNS
