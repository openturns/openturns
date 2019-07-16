//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical
 *        math functions implementations
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/ComposedEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ComposedEvaluation)

static const Factory<ComposedEvaluation> Factory_ComposedEvaluation;

/* Default constructor */
ComposedEvaluation::ComposedEvaluation(const Evaluation & leftFunction,
                                       const Evaluation & rightFunction)
  : EvaluationImplementation()
  , leftFunction_(leftFunction)
  , rightFunction_(rightFunction)
{
  // Check if the dimensions of the left and right functions are compatible
  if (leftFunction.getInputDimension() != rightFunction.getOutputDimension()) throw InvalidArgumentException(HERE) << "The input dimension=" << leftFunction.getInputDimension() << " of the left function must be equal to the output dimension=" << rightFunction.getOutputDimension() << " of the right function to compose them";
  setInputDescription(rightFunction.getInputDescription());
  setOutputDescription(leftFunction.getOutputDescription());
}

/* Virtual constructor */
ComposedEvaluation * ComposedEvaluation::clone() const
{
  return new ComposedEvaluation(*this);
}

/* Comparison operator */
Bool ComposedEvaluation::operator ==(const ComposedEvaluation & ) const
{
  return true;
}

/* Get the i-th marginal function */
Evaluation ComposedEvaluation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return new ComposedEvaluation(leftFunction_.getMarginal(i), rightFunction_);
}

/* Get the function corresponding to indices components */
Evaluation ComposedEvaluation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "The indices of a marginal function must be in the range [0, dim-1] and must be different";
  return new ComposedEvaluation(leftFunction_.getMarginal(indices), rightFunction_);
}


/* String converter */
String ComposedEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ComposedEvaluation::GetClassName()
      << " name=" << getName()
      << " leftFunction=" << leftFunction_.__repr__()
      << " rightFunction=" << rightFunction_.__repr__();
  return oss;
}

String ComposedEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  if (hasVisibleName()) oss << "name=" << getName() << Os::GetEndOfLine() << offset;
  oss << "(" << leftFunction_.__str__(offset) << ")o(";
  oss << rightFunction_.__str__(offset) << ")";
  return oss;
}

/* Operator () */
Point ComposedEvaluation::operator() (const Point & inP) const
{
  if (inP.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a Function with an argument of invalid dimension";
  callsNumber_.increment();
  const Point rightValue(rightFunction_.operator()(inP));
  const Point leftValue(leftFunction_.operator()(rightValue));
  return leftValue;
}

/* Operator () */
Sample ComposedEvaluation::operator() (const Sample & inSample) const
{
  callsNumber_.fetchAndAdd(inSample.getSize());
  const Sample rightSample(rightFunction_.operator()(inSample));
  Sample leftSample(leftFunction_.operator()(rightSample));
  leftSample.setDescription(getOutputDescription());
  return leftSample;
}

/* Parameters value accessor */
Point ComposedEvaluation::getParameter() const
{
  Point parameter(rightFunction_.getParameter());
  parameter.add(leftFunction_.getParameter());
  return parameter;
}

void ComposedEvaluation::setParameter(const Point & parameter)
{
  Point rightParameter(rightFunction_.getParameter());
  const UnsignedInteger rightDimension = rightParameter.getDimension();
  Point leftParameter(leftFunction_.getParameter());
  const UnsignedInteger leftDimension = leftParameter.getDimension();
  if (parameter.getDimension() != rightDimension + leftDimension)
    throw InvalidArgumentException(HERE) << "Required parameter of dimension " << rightDimension + leftDimension << " provided " << parameter.getDimension();
  std::copy(parameter.begin(), parameter.begin() + rightDimension, rightParameter.begin());
  rightFunction_.setParameter(rightParameter);
  std::copy(parameter.begin() + rightDimension, parameter.begin() + rightDimension + leftDimension, leftParameter.begin());
  leftFunction_.setParameter(leftParameter);
}

/* Parameters description accessor */
Description ComposedEvaluation::getParameterDescription() const
{
  Description description(rightFunction_.getParameterDescription());
  description.add(leftFunction_.getParameterDescription());
  return description;
}

void ComposedEvaluation::setParameterDescription(const Description & description)
{
  Description rightDescription(rightFunction_.getParameterDescription());
  const UnsignedInteger rightDimension = rightDescription.getSize();
  Description leftDescription(leftFunction_.getParameterDescription());
  const UnsignedInteger leftDimension = leftDescription.getSize();
  if (description.getSize() != rightDimension + leftDimension)
    throw InvalidArgumentException(HERE) << "Required parameter description of dimension " << rightDimension + leftDimension << " provided " << description.getSize();
  std::copy(description.begin(), description.begin() + rightDimension, rightDescription.begin());
  rightFunction_.setParameterDescription(rightDescription);
  std::copy(description.begin() + rightDimension, description.begin() + rightDimension + leftDimension, leftDescription.begin());
  leftFunction_.setParameterDescription(leftDescription);
}

/* Accessor for input point dimension */
UnsignedInteger ComposedEvaluation::getInputDimension() const
{
  return rightFunction_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger ComposedEvaluation::getOutputDimension() const
{
  return leftFunction_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void ComposedEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "leftFunction_", leftFunction_ );
  adv.saveAttribute( "rightFunction_", rightFunction_ );
}

/* Method load() reloads the object from the StorageManager */
void ComposedEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "leftFunction_", leftFunction_ );
  adv.loadAttribute( "rightFunction_", rightFunction_ );
}

/* Composed implementation accessor */
Evaluation ComposedEvaluation::getLeftEvaluation() const
{
  return leftFunction_;
}

Evaluation ComposedEvaluation::getRightEvaluation() const
{
  return rightFunction_;
}

/* Linearity accessors */
Bool ComposedEvaluation::isLinear() const
{
  return leftFunction_.isLinear() && rightFunction_.isLinear();
}

Bool ComposedEvaluation::isLinearlyDependent(const UnsignedInteger index) const
{
  return leftFunction_.isLinearlyDependent(index) && rightFunction_.isLinearlyDependent(index);
}

END_NAMESPACE_OPENTURNS
