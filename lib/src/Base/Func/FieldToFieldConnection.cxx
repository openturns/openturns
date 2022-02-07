//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between field functions or
 *        point to field and field to point functions
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/FieldToFieldConnection.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FieldToFieldConnection)

static const Factory<FieldToFieldConnection> Factory_FieldToFieldConnection;

/* Default constructor */
FieldToFieldConnection::FieldToFieldConnection()
  : FieldFunctionImplementation()
  , isFieldFunctionComposition_(true)
  , leftFieldFunction_()
  , rightFieldFunction_()
  , fieldToPointFunction_()
  , pointToFieldFunction_()
{
  // Nothing to do
}

/* Parameters constructor */
FieldToFieldConnection::FieldToFieldConnection(const FieldFunction & leftFieldFunction,
    const FieldFunction & rightFieldFunction)
  : FieldFunctionImplementation(rightFieldFunction.getInputMesh(), rightFieldFunction.getInputDimension(), leftFieldFunction.getOutputMesh(), leftFieldFunction.getOutputDimension())
  , isFieldFunctionComposition_(true)
  , leftFieldFunction_(leftFieldFunction)
  , rightFieldFunction_(rightFieldFunction)
  , fieldToPointFunction_()
  , pointToFieldFunction_()
{
  // Check if the dimensions of the left and right functions are compatible
  if (leftFieldFunction.getInputDimension() != rightFieldFunction.getOutputDimension()) throw InvalidArgumentException(HERE) << "The input dimension=" << leftFieldFunction.getInputDimension() << " of the left function must be equal to the output dimension=" << rightFieldFunction.getOutputDimension() << " of the right function to compose them";
  setInputDescription(rightFieldFunction.getInputDescription());
  setOutputDescription(leftFieldFunction.getOutputDescription());
}

/* Parameters constructor */
FieldToFieldConnection::FieldToFieldConnection(const PointToFieldFunction & pointToFieldFunction,
    const FieldToPointFunction & fieldToPointFunction)
  : FieldFunctionImplementation(fieldToPointFunction.getInputMesh(), fieldToPointFunction.getInputDimension(), pointToFieldFunction.getOutputMesh(), fieldToPointFunction.getOutputDimension())
  , isFieldFunctionComposition_(false)
  , leftFieldFunction_()
  , rightFieldFunction_()
  , fieldToPointFunction_(fieldToPointFunction)
  , pointToFieldFunction_(pointToFieldFunction)
{
  // Check if the dimensions of the point to field and field to point functions are compatible
  if (fieldToPointFunction_.getInputDimension() != pointToFieldFunction_.getOutputDimension()) throw InvalidArgumentException(HERE) << "The input dimension=" << fieldToPointFunction_.getInputDimension() << " of the field to point function must be equal to the output dimension=" << pointToFieldFunction_.getOutputDimension() << " of the point to field function to compose them";
  setInputDescription(fieldToPointFunction_.getInputDescription());
  setOutputDescription(pointToFieldFunction_.getOutputDescription());
}

/* Virtual constructor */
FieldToFieldConnection * FieldToFieldConnection::clone() const
{
  return new FieldToFieldConnection(*this);
}

/* Comparison operator */
Bool FieldToFieldConnection::operator ==(const FieldToFieldConnection & other) const
{
  if (this == &other) return true;
  return pointToFieldFunction_ == other.pointToFieldFunction_ && fieldToPointFunction_ == other.fieldToPointFunction_ && leftFieldFunction_ == other.leftFieldFunction_ && rightFieldFunction_ == other.rightFieldFunction_;
}

/* Get the i-th marginal function */
FieldToFieldConnection::Implementation FieldToFieldConnection::getMarginal(const UnsignedInteger i) const
{
  if (!(i < getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1], here index=" << i << " and outputDimension=" << getOutputDimension();
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
FieldToFieldConnection::Implementation FieldToFieldConnection::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "The indices of a marginal function must be in the range [0, dim-1] and must be different";
  if (isFieldFunctionComposition_) return new FieldToFieldConnection(leftFieldFunction_.getMarginal(indices), rightFieldFunction_);
  return new FieldToFieldConnection(pointToFieldFunction_.getMarginal(indices), fieldToPointFunction_);
}


/* String converter */
String FieldToFieldConnection::__repr__() const
{
  OSS oss(true);
  oss << "class=" << FieldToFieldConnection::GetClassName()
      << " name=" << getName()
      << " isFieldFunctionComposition=" << isFieldFunctionComposition_
      << " leftFunction=" << leftFieldFunction_.__repr__()
      << " rightFunction=" << rightFieldFunction_.__repr__()
      << " fieldToPoint=" << fieldToPointFunction_.__repr__()
      << " pointToField=" << pointToFieldFunction_.__repr__();
  return oss;
}

String FieldToFieldConnection::__str__(const String & offset) const
{
  OSS oss(false);
  if (hasVisibleName()) oss << "name=" << getName() << Os::GetEndOfLine() << offset;
  if (isFieldFunctionComposition_)
  {
    oss << "(" << leftFieldFunction_.__str__(offset) << ")o(";
    oss << rightFieldFunction_.__str__(offset) << ")";
  }
  else
  {
    oss << "(" << fieldToPointFunction_.__str__(offset) << ")o(";
    oss << pointToFieldFunction_.__str__(offset) << ")";
  }
  return oss;
}

/* Operator () */
Sample FieldToFieldConnection::operator() (const Sample & inF) const
{
  if (inF.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a FieldToFieldConnection with an argument of invalid dimension";
  if (inF.getSize() != getInputMesh().getVerticesNumber()) throw InvalidArgumentException(HERE) << "Error:";

  callsNumber_.increment();
  const Sample outValue(isFieldFunctionComposition_ ? leftFieldFunction_(rightFieldFunction_(inF)) : pointToFieldFunction_(fieldToPointFunction_(inF)));
  return outValue;
}

/* Operator () */
ProcessSample FieldToFieldConnection::operator() (const ProcessSample & inSample) const
{
  if (inSample.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a FieldToFieldConnection with an argument of invalid dimension";
  callsNumber_.fetchAndAdd(inSample.getSize());
  const ProcessSample outValue(isFieldFunctionComposition_ ? leftFieldFunction_(rightFieldFunction_(inSample)) : pointToFieldFunction_(fieldToPointFunction_(inSample)));
  return outValue;
}

/* Accessor for input point dimension */
UnsignedInteger FieldToFieldConnection::getInputDimension() const
{
  if (isFieldFunctionComposition_) return rightFieldFunction_.getInputDimension();
  return fieldToPointFunction_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger FieldToFieldConnection::getOutputDimension() const
{
  if (isFieldFunctionComposition_) return leftFieldFunction_.getOutputDimension();
  return pointToFieldFunction_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void FieldToFieldConnection::save(Advocate & adv) const
{
  FieldFunctionImplementation::save(adv);
  adv.saveAttribute( "isFieldFunctionComposition_", isFieldFunctionComposition_ );
  adv.saveAttribute( "leftFieldFunction_", leftFieldFunction_ );
  adv.saveAttribute( "rightFieldFunction_", rightFieldFunction_ );
  adv.saveAttribute( "pointToFieldFunction_", pointToFieldFunction_ );
  adv.saveAttribute( "fieldToPointFunction_", fieldToPointFunction_ );
}

/* Method load() reloads the object from the StorageManager */
void FieldToFieldConnection::load(Advocate & adv)
{
  FieldFunctionImplementation::load(adv);
  adv.loadAttribute( "isFieldFunctionComposition_", isFieldFunctionComposition_ );
  adv.loadAttribute( "leftFieldFunction_", leftFieldFunction_ );
  adv.loadAttribute( "rightFieldFunction_", rightFieldFunction_ );
  adv.loadAttribute( "pointToFieldFunction_", pointToFieldFunction_ );
  adv.loadAttribute( "fieldToPointFunction_", fieldToPointFunction_ );
}

/* Composed implementation accessor */
FieldFunction FieldToFieldConnection::getLeftFieldFunction() const
{
  return leftFieldFunction_;
}

FieldFunction FieldToFieldConnection::getRightFieldFunction() const
{
  return rightFieldFunction_;
}

/* Composed implementation accessor */
PointToFieldFunction FieldToFieldConnection::getPointToFieldFunction() const
{
  return pointToFieldFunction_;
}

FieldToPointFunction FieldToFieldConnection::getFieldToPointFunction() const
{
  return fieldToPointFunction_;
}

END_NAMESPACE_OPENTURNS
