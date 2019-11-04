//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between evaluations or
 *        field to point and point to field functions
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/PointToPointEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/NoEvaluation.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PointToPointEvaluation)

static const Factory<PointToPointEvaluation> Factory_PointToPointEvaluation;

/* Default constructor */
PointToPointEvaluation::PointToPointEvaluation()
  : EvaluationImplementation()
  , isFunctionComposition_(true)
  , leftFunction_()
  , rightFunction_()
  , fieldToPointFunction_()
  , pointToFieldFunction_()
{
  // Nothing to do
}

/* Parameters constructor */
PointToPointEvaluation::PointToPointEvaluation(const Function & leftFunction,
    const Function & rightFunction)
  : EvaluationImplementation()
  , isFunctionComposition_(true)
  , leftFunction_(leftFunction)
  , rightFunction_(rightFunction)
  , fieldToPointFunction_()
  , pointToFieldFunction_()
{
  // Check if the dimensions of the left and right functions are compatible
  if (leftFunction.getInputDimension() != rightFunction.getOutputDimension()) throw InvalidArgumentException(HERE) << "The input dimension=" << leftFunction.getInputDimension() << " of the left function must be equal to the output dimension=" << rightFunction.getOutputDimension() << " of the right function to compose them";
  setInputDescription(rightFunction.getInputDescription());
  setOutputDescription(leftFunction.getOutputDescription());
}

/* Parameters constructor */
PointToPointEvaluation::PointToPointEvaluation(const FieldToPointFunction & fieldToPointFunction,
    const PointToFieldFunction & pointToFieldFunction)
  : EvaluationImplementation()
  , isFunctionComposition_(false)
  , leftFunction_()
  , rightFunction_()
  , fieldToPointFunction_(fieldToPointFunction)
  , pointToFieldFunction_(pointToFieldFunction)
{
  // Check if the dimensions of the point to field and field to point functions are compatible
  if (fieldToPointFunction_.getInputDimension() != pointToFieldFunction_.getOutputDimension()) throw InvalidArgumentException(HERE) << "The input dimension=" << fieldToPointFunction_.getInputDimension() << " of the field to point function must be equal to the output dimension=" << pointToFieldFunction_.getOutputDimension() << " of the point to field function to compose them";
  setInputDescription(pointToFieldFunction_.getInputDescription());
  setOutputDescription(fieldToPointFunction_.getOutputDescription());
}

/* Virtual constructor */
PointToPointEvaluation * PointToPointEvaluation::clone() const
{
  return new PointToPointEvaluation(*this);
}

/* Comparison operator */
Bool PointToPointEvaluation::operator ==(const PointToPointEvaluation & other) const
{
  if (this == &other) return true;
  return pointToFieldFunction_ == other.pointToFieldFunction_ && fieldToPointFunction_ == other.fieldToPointFunction_ && leftFunction_ == other.leftFunction_ && rightFunction_ == other.rightFunction_;
}

/* Get the i-th marginal function */
Evaluation PointToPointEvaluation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return new PointToPointEvaluation(leftFunction_.getMarginal(i), rightFunction_);
}

/* Get the function corresponding to indices components */
Evaluation PointToPointEvaluation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "The indices of a marginal function must be in the range [0, dim-1] and must be different";
  if (isFunctionComposition_) return new PointToPointEvaluation(leftFunction_.getMarginal(indices), rightFunction_);
  return new PointToPointEvaluation(fieldToPointFunction_.getMarginal(indices), pointToFieldFunction_);
}


/* String converter */
String PointToPointEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << PointToPointEvaluation::GetClassName()
      << " name=" << getName()
      << " isFunctionComposition=" << isFunctionComposition_
      << " leftFunction=" << leftFunction_.__repr__()
      << " rightFunction=" << rightFunction_.__repr__()
      << " fieldToPoint=" << fieldToPointFunction_.__repr__()
      << " pointToField=" << pointToFieldFunction_.__repr__();
  return oss;
}

String PointToPointEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  if (hasVisibleName()) oss << "name=" << getName() << Os::GetEndOfLine() << offset;
  if (isFunctionComposition_)
  {
    oss << "(" << leftFunction_.__str__(offset) << ")o(";
    oss << rightFunction_.__str__(offset) << ")";
  }
  else
  {
    oss << "(" << fieldToPointFunction_.__str__(offset) << ")o(";
    oss << pointToFieldFunction_.__str__(offset) << ")";
  }
  return oss;
}

/* Operator () */
Point PointToPointEvaluation::operator() (const Point & inP) const
{
  if (inP.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a Function with an argument of invalid dimension";
  callsNumber_.increment();
  const Point leftValue(isFunctionComposition_ ? leftFunction_.operator()(rightFunction_.operator()(inP)) : fieldToPointFunction_.operator()(pointToFieldFunction_.operator()(inP)));
  return leftValue;
}

/* Operator () */
Sample PointToPointEvaluation::operator() (const Sample & inSample) const
{
  callsNumber_.fetchAndAdd(inSample.getSize());
  // In the case of a composition of functions, compute the whole
  // intermediate sample
  Sample outSample(inSample.getSize(), getOutputDimension());
  if (isFunctionComposition_) outSample = leftFunction_.operator()(rightFunction_.operator()(inSample));
  // Else compute the intermediate sample by slices of reasonable size
  else
  {
    const UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("PointToPointEvaluation-BlockSize");
    UnsignedInteger remaining = inSample.getSize();
    while (remaining > 0)
    {
      const UnsignedInteger currentBlockSize = std::min(blockSize, remaining);
      Sample inBlock(currentBlockSize, inSample.getDimension());
      for (UnsignedInteger i = 0; i < currentBlockSize; ++i)
        inBlock[i] = inSample[remaining - i - 1];
      const Sample outBlock(fieldToPointFunction_.operator()(pointToFieldFunction_.operator()(inBlock)));
      for (UnsignedInteger i = 0; i < currentBlockSize; ++i)
        outSample[remaining - i - 1] = outBlock[i];
      remaining -= currentBlockSize;
    } // while (ramaining > 0)
  } // isFunctionComposition_
  outSample.setDescription(getOutputDescription());
  return outSample;
}

/* Parameters value accessor */
Point PointToPointEvaluation::getParameter() const
{
  if (!isFunctionComposition_) throw NotYetImplementedException(HERE) << "In PointToPointEvaluation::getParameter()";
  Point parameter(rightFunction_.getParameter());
  parameter.add(leftFunction_.getParameter());
  return parameter;
}

void PointToPointEvaluation::setParameter(const Point & parameter)
{
  if (!isFunctionComposition_) throw NotYetImplementedException(HERE) << "In PointToPointEvaluation::setParameter()";
  Point rightParameter(rightFunction_.getParameter());
  const UnsignedInteger rightDimension = rightParameter.getDimension();
  Point leftParameter(leftFunction_.getParameter());
  const UnsignedInteger leftDimension = leftParameter.getDimension();
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < rightDimension; ++ i)
  {
    rightParameter[i] = parameter[index];
    ++ index;
  }
  rightFunction_.setParameter(rightParameter);
  for (UnsignedInteger i = 0; i < leftDimension; ++ i)
  {
    leftParameter[i] = parameter[index];
    ++ index;
  }
  leftFunction_.setParameter(leftParameter);
}

/* Parameters description accessor */
Description PointToPointEvaluation::getParameterDescription() const
{
  if (!isFunctionComposition_) throw NotYetImplementedException(HERE) << "In PointToPointEvaluation::getParameterDescription()";
  Description description(rightFunction_.getParameterDescription());
  description.add(leftFunction_.getParameterDescription());
  return description;
}

void PointToPointEvaluation::setParameterDescription(const Description & description)
{
  if (!isFunctionComposition_) throw NotYetImplementedException(HERE) << "In PointToPointEvaluation::setParameterDescription()";
  Description rightDescription(rightFunction_.getParameterDescription());
  const UnsignedInteger rightDimension = rightDescription.getSize();
  Description leftDescription(leftFunction_.getParameterDescription());
  const UnsignedInteger leftDimension = leftDescription.getSize();
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < rightDimension; ++ i)
  {
    rightDescription[i] = description[index];
    ++ index;
  }
  rightFunction_.setParameterDescription(rightDescription);
  for (UnsignedInteger i = 0; i < leftDimension; ++ i)
  {
    leftDescription[i] = description[index];
    ++ index;
  }
  leftFunction_.setParameterDescription(leftDescription);
}

/* Accessor for input point dimension */
UnsignedInteger PointToPointEvaluation::getInputDimension() const
{
  if (isFunctionComposition_) return rightFunction_.getInputDimension();
  return pointToFieldFunction_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger PointToPointEvaluation::getOutputDimension() const
{
  if (isFunctionComposition_) return leftFunction_.getOutputDimension();
  return fieldToPointFunction_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void PointToPointEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "isFunctionComposition_", isFunctionComposition_ );
  adv.saveAttribute( "leftFunction_", leftFunction_ );
  adv.saveAttribute( "rightFunction_", rightFunction_ );
  adv.saveAttribute( "pointToFieldFunction_", pointToFieldFunction_ );
  adv.saveAttribute( "fieldToPointFunction_", fieldToPointFunction_ );
}

/* Method load() reloads the object from the StorageManager */
void PointToPointEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "isFunctionComposition_", isFunctionComposition_ );
  adv.loadAttribute( "leftFunction_", leftFunction_ );
  adv.loadAttribute( "rightFunction_", rightFunction_ );
  adv.loadAttribute( "pointToFieldFunction_", pointToFieldFunction_ );
  adv.loadAttribute( "fieldToPointFunction_", fieldToPointFunction_ );
}

/* Composed implementation accessor */
Function PointToPointEvaluation::getLeftFunction() const
{
  return leftFunction_;
}

Function PointToPointEvaluation::getRightFunction() const
{
  return rightFunction_;
}

/* Composed implementation accessor */
PointToFieldFunction PointToPointEvaluation::getPointToFieldFunction() const
{
  return pointToFieldFunction_;
}

FieldToPointFunction PointToPointEvaluation::getFieldToPointFunction() const
{
  return fieldToPointFunction_;
}

END_NAMESPACE_OPENTURNS
