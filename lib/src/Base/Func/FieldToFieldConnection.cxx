//                                               -*- C++ -*-
/**
 * @brief The class that implements the composition between field functions or 
 *        point to field and field to point functions
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

#include "openturns/FieldToFieldConnection.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FieldToFieldConnection);

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
  : FieldFunctionImplementation(rightFieldFunction.getSpatialDimension(), rightFieldFunction.getInputDimension(), leftFieldFunction.getOutputDimension())
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
  : FieldFunctionImplementation(fieldToPointFunction.getSpatialDimension(), fieldToPointFunction.getInputDimension(), fieldToPointFunction.getOutputDimension())
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
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
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
  if (hasVisibleName()) oss << offset << "name=" << getName() << "\n";
  if (isFieldFunctionComposition_)
    {
      oss << "(" << leftFieldFunction_.__str__() << ")o(";
      oss << rightFieldFunction_.__str__() << ")";
    }
  else
    {
      oss << "(" << fieldToPointFunction_.__str__() << ")o(";
      oss << pointToFieldFunction_.__str__() << ")";
    }
  return oss;
}

/* Operator () */
Field FieldToFieldConnection::operator() (const Field & inF) const
{
  if (inF.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a FieldToFieldConnection with an argument of invalid dimension";
  ++callsNumber_;
  const Field outValue(isFieldFunctionComposition_ ? leftFieldFunction_(rightFieldFunction_(inF)) : pointToFieldFunction_(fieldToPointFunction_(inF)));
  return outValue;
}

/* Operator () */
ProcessSample FieldToFieldConnection::operator() (const ProcessSample & inSample) const
{
  if (inSample.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a FieldToFieldConnection with an argument of invalid dimension";
  callsNumber_ += inSample.getSize();
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
