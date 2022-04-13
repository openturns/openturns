//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between evaluation and
 *        point to field function or between point to field function and
 *        field function
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/PointToFieldConnection.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/PointToFieldFunction.hxx"
#include "openturns/Os.hxx"

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
  : PointToFieldFunctionImplementation(pointToFieldFunction.getInputDimension(), fieldFunction.getOutputMesh(), fieldFunction.getOutputDimension())
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
  : PointToFieldFunctionImplementation(function.getInputDimension(), pointToFieldFunction.getOutputMesh(), pointToFieldFunction.getOutputDimension())
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
  if (!(i < getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1], here index=" << i << " and outputDimension=" << getOutputDimension();
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
  if (hasVisibleName()) oss << "name=" << getName() << Os::GetEndOfLine() << offset;
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
Sample PointToFieldConnection::operator() (const Point & inP) const
{
  if (inP.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a PointToFieldConnection with an argument of invalid dimension";
  callsNumber_.increment();
  const Sample outValue(startByPointToFieldFunction_ ? fieldFunction_(pointToFieldFunction_(inP)) : pointToFieldFunction_(function_(inP)));
  return outValue;
}

/* Operator () */
ProcessSample PointToFieldConnection::operator() (const Sample & inSample) const
{
  if (inSample.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a PointToFieldConnection with an argument of invalid dimension";
  callsNumber_.fetchAndAdd(inSample.getSize());
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
