//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical
 *        math functions implementations
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
 */

#include "openturns/ProductEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProductEvaluation)

static const Factory<ProductEvaluation> Factory_ProductEvaluation;

/* Default constructor */
ProductEvaluation::ProductEvaluation(const Evaluation & leftEvaluation,
                                     const Evaluation & rightEvaluation)
  : EvaluationImplementation()
  , leftEvaluation_(leftEvaluation)
  , rightEvaluation_(rightEvaluation)
{
  // Check if the dimensions of the left and right functions are compatible
  if (leftEvaluation.getOutputDimension() != 1)
  {
    if (rightEvaluation.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the output dimension of at least one of the functions must be 1 in a product, here it is " << rightEvaluation.getOutputDimension();
    LOGWARN("We exchanged the functions in order to have the left one with output dimension equals to 1");
    leftEvaluation_ = rightEvaluation;
    rightEvaluation_ = leftEvaluation;
  }
  if (leftEvaluation_.getInputDimension() != rightEvaluation_.getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the two functions must have the same input dimension, here left=" << leftEvaluation_.getInputDimension() << " and right=" << rightEvaluation_.getInputDimension();
  setDescription(rightEvaluation_.getDescription());
}

/* Virtual constructor */
ProductEvaluation * ProductEvaluation::clone() const
{
  return new ProductEvaluation(*this);
}

/* Comparison operator */
Bool ProductEvaluation::operator ==(const ProductEvaluation & ) const
{
  return true;
}

/* String converter */
String ProductEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ProductEvaluation::GetClassName()
      << " name=" << getName()
      << " leftEvaluation=" << leftEvaluation_.getImplementation()->__repr__()
      << " rightEvaluation=" << rightEvaluation_.getImplementation()->__repr__();
  return oss;
}

String ProductEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  if (hasVisibleName()) oss << getName() << " :";
  if (getOutputDimension() > 1) oss << "\n" << offset;
  oss << "(" << leftEvaluation_.getImplementation()->__str__(offset) << ") * ("
      << "(" << rightEvaluation_.getImplementation()->__str__(offset) << ")";
  return oss;
}

/* Operator () */
Point ProductEvaluation::operator() (const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  const Point left(leftEvaluation_.operator()(inP));
  const Point right(rightEvaluation_.operator()(inP));
  const Point result(left[0] * right);
  callsNumber_.increment();
  return result;
}

/* Operator () */
Sample ProductEvaluation::operator() (const Sample & inSample) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inSample.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();
  callsNumber_.fetchAndAdd(size);
  const Sample leftSample(leftEvaluation_.operator()(inSample));
  Sample rightSample(rightEvaluation_.operator()(inSample));
  for (UnsignedInteger i = 0; i < size; ++i) rightSample[i] *= leftSample(i, 0);
  rightSample.setDescription(getOutputDescription());
  return rightSample;
}


/* Parameters value accessor */
Point ProductEvaluation::getParameter() const
{
  Point parameter(rightEvaluation_.getParameter());
  parameter.add(leftEvaluation_.getParameter());
  return parameter;
}


void ProductEvaluation::setParameter(const Point & parameter)
{
  Point rightParameter(rightEvaluation_.getParameter());
  const UnsignedInteger rightDimension = rightParameter.getDimension();
  PointWithDescription leftParameter(leftEvaluation_.getParameter());
  const UnsignedInteger leftDimension = leftParameter.getDimension();
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < rightDimension; ++ i)
  {
    rightParameter[i] = parameter[index];
    ++ index;
  }
  rightEvaluation_.setParameter(rightParameter);
  for (UnsignedInteger i = 0; i < leftDimension; ++ i)
  {
    leftParameter[i] = parameter[index];
    ++ index;
  }
  leftEvaluation_.setParameter(leftParameter);
}


/* Parameters description accessor */
Description ProductEvaluation::getParameterDescription() const
{
  Description description(rightEvaluation_.getParameterDescription());
  description.add(leftEvaluation_.getParameterDescription());
  return description;
}


/* Accessor for input point dimension */
UnsignedInteger ProductEvaluation::getInputDimension() const
{
  return rightEvaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger ProductEvaluation::getOutputDimension() const
{
  return rightEvaluation_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void ProductEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "leftEvaluation_", leftEvaluation_ );
  adv.saveAttribute( "rightEvaluation_", rightEvaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "leftEvaluation_", leftEvaluation_ );
  adv.loadAttribute( "rightEvaluation_", rightEvaluation_ );
}

END_NAMESPACE_OPENTURNS
