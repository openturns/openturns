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
 * \author $LastChangedBy: lebrun $
 * \date   $LastChangedDate: 2008-07-03 08:31:34 +0200 (jeu, 03 jui 2008) $
 */

#include "openturns/ProductEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProductEvaluation)

static const Factory<ProductEvaluation> Factory_ProductEvaluation;

/* Default constructor */
ProductEvaluation::ProductEvaluation(const EvaluationPointer & p_leftEvaluation,
                                     const EvaluationPointer & p_rightEvaluation)
  : EvaluationImplementation()
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
ProductEvaluation * ProductEvaluation::clone() const
{
  return new ProductEvaluation(*this);
}

/* Comparison operator */
Bool ProductEvaluation::operator ==(const ProductEvaluation & other) const
{
  return true;
}

/* String converter */
String ProductEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ProductEvaluation::GetClassName()
      << " name=" << getName()
      << " leftEvaluation=" << p_leftEvaluation_->__repr__()
      << " rightEvaluation=" << p_rightEvaluation_->__repr__();
  return oss;
}

String ProductEvaluation::__str__(const String & offset) const
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
Point ProductEvaluation::operator() (const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  const Point left(p_leftEvaluation_->operator()(inP));
  const Point right(p_rightEvaluation_->operator()(inP));
  const Point result(left[0] * right);
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

/* Operator () */
Sample ProductEvaluation::operator() (const Sample & inSample) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inSample.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();
  callsNumber_ += size;
  const Sample leftSample(p_leftEvaluation_->operator()(inSample));
  Sample rightSample(p_rightEvaluation_->operator()(inSample));
  for (UnsignedInteger i = 0; i < size; ++i) rightSample[i] *= leftSample[i][0];
  rightSample.setDescription(getOutputDescription());
  return rightSample;
}


/* Parameters value accessor */
Point ProductEvaluation::getParameter() const
{
  Point parameter(p_rightEvaluation_->getParameter());
  parameter.add(p_leftEvaluation_->getParameter());
  return parameter;
}


void ProductEvaluation::setParameter(const Point & parameter)
{
  Point rightParameter(p_rightEvaluation_->getParameter());
  const UnsignedInteger rightDimension = rightParameter.getDimension();
  PointWithDescription leftParameter(p_leftEvaluation_->getParameter());
  const UnsignedInteger leftDimension = leftParameter.getDimension();
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < rightDimension; ++ i)
  {
    rightParameter[i] = parameter[index];
    ++ index;
  }
  p_rightEvaluation_->setParameter(rightParameter);
  for (UnsignedInteger i = 0; i < leftDimension; ++ i)
  {
    leftParameter[i] = parameter[index];
    ++ index;
  }
  p_leftEvaluation_->setParameter(leftParameter);
}


/* Parameters description accessor */
Description ProductEvaluation::getParameterDescription() const
{
  Description description(p_rightEvaluation_->getParameterDescription());
  description.add(p_leftEvaluation_->getParameterDescription());
  return description;
}


/* Accessor for input point dimension */
UnsignedInteger ProductEvaluation::getInputDimension() const
{
  return p_rightEvaluation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger ProductEvaluation::getOutputDimension() const
{
  return p_rightEvaluation_->getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void ProductEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "leftEvaluation_", *p_leftEvaluation_ );
  adv.saveAttribute( "rightEvaluation_", *p_rightEvaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.saveAttribute( "leftEvaluation_", *p_leftEvaluation_ );
  adv.saveAttribute( "rightEvaluation_", *p_rightEvaluation_ );
}

END_NAMESPACE_OPENTURNS
