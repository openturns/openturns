//                                               -*- C++ -*-
/**
 * @brief The class that implements the composition between evaluation and
 *        point to field function or between point to field function and
 *        field function
 *
 * Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#include "openturns/PointToFieldConnection.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/PointToFieldFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PointToFieldConnection)

static const Factory<PointToFieldConnection> Factory_PointToFieldConnection;

/* Default constructor */
PointToFieldConnection::PointToFieldConnection()
  : PointToFieldFunctionImplementation()
  , startByPointToFieldFunction_(true)
  , function_()
  , fieldFunction_()
  , pointToFieldFunction_()
{
  // Nothing to do
}

/* Parameters constructor */
PointToFieldConnection::PointToFieldConnection(const FieldFunction & fieldFunction,
    const PointToFieldFunction & pointToFieldFunction)
  : PointToFieldFunctionImplementation(pointToFieldFunction.getInputDimension(), fieldFunction.getOutputDimension())
  , startByPointToFieldFunction_(true)
  , function_()
  , fieldFunction_(fieldFunction)
  , pointToFieldFunction_(pointToFieldFunction)
{
  // Check if the dimensions of the field function and point to field function are compatible
  if (fieldFunction.getInputDimension() != pointToFieldFunction.getOutputDimension()) throw InvalidArgumentException(HERE) << "The input dimension=" << fieldFunction.getInputDimension() << " of the field function must be equal to the output dimension=" << pointToFieldFunction.getOutputDimension() << " of the point to field function to compose them";
  setInputDescription(pointToFieldFunction.getInputDescription());
  setOutputDescription(fieldFunction.getOutputDescription());
}

/* Parameters constructor */
PointToFieldConnection::PointToFieldConnection(const PointToFieldFunction & pointToFieldFunction,
    const Function & function)
  : PointToFieldFunctionImplementation(function.getInputDimension(), pointToFieldFunction.getOutputDimension())
  , startByPointToFieldFunction_(false)
  , function_(function)
  , fieldFunction_()
  , pointToFieldFunction_(pointToFieldFunction)
{
  // Check if the dimensions of the point to field function and the function are compatible
  if (pointToFieldFunction.getInputDimension() != function.getOutputDimension()) throw InvalidArgumentException(HERE) << "The input dimension=" << pointToFieldFunction.getInputDimension() << " of the point to field function must be equal to the output dimension=" << function_.getOutputDimension() << " of the function to compose them";
  setInputDescription(function.getInputDescription());
  setOutputDescription(pointToFieldFunction.getOutputDescription());
}

/* Virtual constructor */
PointToFieldConnection * PointToFieldConnection::clone() const
{
  return new PointToFieldConnection(*this);
}

/* Comparison operator */
Bool PointToFieldConnection::operator ==(const PointToFieldConnection & other) const
{
  if (this == &other) return true;
  return function_ == other.function_ && fieldFunction_ == other.fieldFunction_ && pointToFieldFunction_ == other.pointToFieldFunction_;
}

/* Get the i-th marginal function */
PointToFieldFunction PointToFieldConnection::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
PointToFieldFunction PointToFieldConnection::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "The indices of a marginal function must be in the range [0, dim-1] and must be different";
  if (startByPointToFieldFunction_) return new PointToFieldConnection(pointToFieldFunction_.getMarginal(indices), function_);
  return new PointToFieldConnection(fieldFunction_.getMarginal(indices), pointToFieldFunction_);
}


/* String converter */
String PointToFieldConnection::__repr__() const
{
  OSS oss(true);
  oss << "class=" << PointToFieldConnection::GetClassName()
      << " name=" << getName()
      << " startByPointToFieldFunction=" << startByPointToFieldFunction_
      << " function=" << function_
      << " fieldFunction=" << fieldFunction_
      << " pointToFieldFunction=" << pointToFieldFunction_;
  return oss;
}

String PointToFieldConnection::__str__(const String & offset) const
{
  OSS oss(false);
  if (hasVisibleName()) oss << offset << "name=" << getName() << "\n";
  if (startByPointToFieldFunction_)
  {
    oss << "(" << pointToFieldFunction_ << ")o(";
    oss << function_ << ")";
  }
  else
  {
    oss << "(" << fieldFunction_ << ")o(";
    oss << pointToFieldFunction_ << ")";
  }
  return oss;
}

/* Operator () */
Field PointToFieldConnection::operator() (const Point & inP) const
{
  if (inP.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a PointToFieldConnection with an argument of invalid dimension";
  ++callsNumber_;
  const Field outValue(startByPointToFieldFunction_ ? fieldFunction_(pointToFieldFunction_(inP)) : pointToFieldFunction_(function_(inP)));
  return outValue;
}

/* Operator () */
ProcessSample PointToFieldConnection::operator() (const Sample & inSample) const
{
  if (inSample.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a PointToFieldConnection with an argument of invalid dimension";
  callsNumber_ += inSample.getSize();
  const ProcessSample outValue(startByPointToFieldFunction_ ? fieldFunction_(pointToFieldFunction_(inSample)) : pointToFieldFunction_(function_(inSample)));
  return outValue;
}

/* Method save() stores the object through the StorageManager */
void PointToFieldConnection::save(Advocate & adv) const
{
  PointToFieldFunctionImplementation::save(adv);
  adv.saveAttribute( "startByPointToFieldFunction_", startByPointToFieldFunction_ );
  adv.saveAttribute( "function_", function_ );
  adv.saveAttribute( "fieldFunction_", fieldFunction_ );
  adv.saveAttribute( "pointToFieldFunction_", pointToFieldFunction_ );
}

/* Method load() reloads the object from the StorageManager */
void PointToFieldConnection::load(Advocate & adv)
{
  TypedInterfaceObject<PointToFieldFunctionImplementation> evaluationValue;
  PointToFieldFunctionImplementation::load(adv);
  adv.loadAttribute( "startByPointToFieldFunction_", startByPointToFieldFunction_ );
  adv.loadAttribute( "function_", function_ );
  adv.loadAttribute( "fieldFunction_", fieldFunction_ );
  adv.loadAttribute( "pointToFieldFunction_", pointToFieldFunction_ );
}

/* Composed implementation accessor */
Function PointToFieldConnection::getFunction() const
{
  return function_;
}

FieldFunction PointToFieldConnection::getFieldFunction() const
{
  return fieldFunction_;
}

/* Composed implementation accessor */
PointToFieldFunction PointToFieldConnection::getPointToFieldFunction() const
{
  return pointToFieldFunction_;
}


END_NAMESPACE_OPENTURNS
