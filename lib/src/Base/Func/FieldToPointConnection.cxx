//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between a function
 *        and a field to point function or between a field to point function
 *        and a field function
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
 *
 */

#include "openturns/FieldToPointConnection.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/NoEvaluation.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FieldToPointConnection)

static const Factory<FieldToPointConnection> Factory_FieldToPointConnection;

/* Default constructor */
FieldToPointConnection::FieldToPointConnection()
  : FieldToPointFunctionImplementation()
  , startByFieldToPointFunction_(true)
  , function_()
  , fieldFunction_()
  , fieldToPointFunction_()
{
  // Nothing to do
}

/* Parameters constructor */
FieldToPointConnection::FieldToPointConnection(const Function & function,
    const FieldToPointFunction & fieldToPointFunction)
  : FieldToPointFunctionImplementation(fieldToPointFunction.getInputMesh(), fieldToPointFunction.getInputDimension(), function.getOutputDimension())
  , startByFieldToPointFunction_(true)
  , function_(function)
  , fieldFunction_()
  , fieldToPointFunction_(fieldToPointFunction)
{
  // Check if the dimensions of the left and right functions are compatible
  if (function.getInputDimension() != fieldToPointFunction.getOutputDimension()) throw InvalidArgumentException(HERE) << "The input dimension=" << fieldToPointFunction.getInputDimension() << " of the field to point function must be equal to the output dimension=" << function.getOutputDimension() << " of the function to compose them";
  setInputDescription(fieldToPointFunction.getInputDescription());
  setOutputDescription(function.getOutputDescription());
}

/* Parameters constructor */
FieldToPointConnection::FieldToPointConnection(const FieldToPointFunction & fieldToPointFunction,
    const FieldFunction & fieldFunction)
  : FieldToPointFunctionImplementation(fieldFunction.getInputMesh(), fieldFunction.getInputDimension(), fieldToPointFunction.getOutputDimension())
  , startByFieldToPointFunction_(false)
  , function_()
  , fieldFunction_(fieldFunction)
  , fieldToPointFunction_(fieldToPointFunction)
{
  // Check if the dimensions of the point to field and field to point functions are compatible
  if (fieldToPointFunction_.getInputDimension() != fieldFunction_.getOutputDimension()) throw InvalidArgumentException(HERE) << "The input dimension=" << fieldToPointFunction_.getInputDimension() << " of the field to point function must be equal to the output dimension=" << fieldFunction_.getOutputDimension() << " of the field function to compose them";
  setInputDescription(fieldFunction_.getInputDescription());
  setOutputDescription(fieldToPointFunction_.getOutputDescription());
}

/* Virtual constructor */
FieldToPointConnection * FieldToPointConnection::clone() const
{
  return new FieldToPointConnection(*this);
}

/* Comparison operator */
Bool FieldToPointConnection::operator ==(const FieldToPointConnection & other) const
{
  if (this == &other) return true;
  return function_ == other.function_ && fieldFunction_ == other.fieldFunction_ && fieldToPointFunction_ == other.fieldToPointFunction_;
}

/* Get the i-th marginal function */
FieldToPointConnection::Implementation FieldToPointConnection::getMarginal(const UnsignedInteger i) const
{
  if (!(i < getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1], here index=" << i << " and outputDimension=" << getOutputDimension();
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
FieldToPointConnection::Implementation FieldToPointConnection::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "The indices of a marginal function must be in the range [0, dim-1] and must be different";
  if (startByFieldToPointFunction_) return new FieldToPointConnection(function_.getMarginal(indices), fieldToPointFunction_);
  return new FieldToPointConnection(fieldToPointFunction_.getMarginal(indices), fieldFunction_);
}


/* String converter */
String FieldToPointConnection::__repr__() const
{
  OSS oss(true);
  oss << "class=" << FieldToPointConnection::GetClassName()
      << " name=" << getName()
      << " isLeftFunctionComposition=" << startByFieldToPointFunction_
      << " function=" << function_
      << " fieldFunction=" << fieldFunction_
      << " fieldToPointFunction=" << fieldToPointFunction_;
  return oss;
}

String FieldToPointConnection::__str__(const String & offset) const
{
  OSS oss(false);
  if (hasVisibleName()) oss << "name=" << getName() << Os::GetEndOfLine() << offset;
  if (startByFieldToPointFunction_)
  {
    oss << "(" << function_ << ")o(";
    oss << fieldToPointFunction_ << ")";
  }
  else
  {
    oss << "(" << fieldToPointFunction_ << ")o(";
    oss << fieldFunction_ << ")";
  }
  return oss;
}

/* Operator () */
Point FieldToPointConnection::operator() (const Sample & inF) const
{
  if (inF.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a FieldToPointConnection with an argument of invalid dimension";
  if (inF.getSize() != getInputMesh().getVerticesNumber()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a FieldToPointConnection with an argument of invalid size";
  callsNumber_.increment();
  const Point outValue(startByFieldToPointFunction_ ? function_(fieldToPointFunction_(inF)) : fieldToPointFunction_(fieldFunction_(inF)));
  return outValue;
}

/* Operator () */
Sample FieldToPointConnection::operator() (const ProcessSample & inSample) const
{
  if (inSample.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a FieldToPointConnection with an argument of invalid dimension";
  callsNumber_.fetchAndAdd(inSample.getSize());
  // In the case of a composition of functions, compute the whole
  // intermediate sample
  Sample outSample(inSample.getSize(), getOutputDimension());
  if (startByFieldToPointFunction_) outSample = function_(fieldToPointFunction_(inSample));
  // Else compute the intermediate sample by slices of reasonable size
  else
  {
    const UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("FieldToPointConnection-BlockSize");
    UnsignedInteger remaining = inSample.getSize();
    while (remaining > 0)
    {
      const UnsignedInteger currentBlockSize = std::min(blockSize, remaining);
      ProcessSample inBlock(inSample.getMesh(), currentBlockSize, inSample.getDimension());
      for (UnsignedInteger i = 0; i < currentBlockSize; ++i)
        inBlock[i] = inSample[remaining - i - 1];
      const Sample outBlock(fieldToPointFunction_(fieldFunction_(inBlock)));
      for (UnsignedInteger i = 0; i < currentBlockSize; ++i)
        outSample[remaining - i - 1] = outBlock[i];
      remaining -= currentBlockSize;
    } // while (ramaining > 0)
  } // startByFieldToPointFunction_
  outSample.setDescription(getOutputDescription());
  return outSample;
}

/* Accessor for input point dimension */
UnsignedInteger FieldToPointConnection::getInputDimension() const
{
  if (startByFieldToPointFunction_) return fieldToPointFunction_.getInputDimension();
  return fieldFunction_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger FieldToPointConnection::getOutputDimension() const
{
  if (startByFieldToPointFunction_) return function_.getOutputDimension();
  return fieldToPointFunction_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void FieldToPointConnection::save(Advocate & adv) const
{
  FieldToPointFunctionImplementation::save(adv);
  adv.saveAttribute( "startByFieldToPointFunction_", startByFieldToPointFunction_ );
  adv.saveAttribute( "function_", function_ );
  adv.saveAttribute( "fieldFunction_", fieldFunction_ );
  adv.saveAttribute( "fieldToPointFunction_", fieldToPointFunction_ );
}

/* Method load() reloads the object from the StorageManager */
void FieldToPointConnection::load(Advocate & adv)
{
  FieldToPointFunctionImplementation::load(adv);
  adv.loadAttribute( "startByFieldToPointFunction_", startByFieldToPointFunction_ );
  adv.loadAttribute( "function_", function_ );
  adv.loadAttribute( "fieldFunction_", fieldFunction_ );
  adv.loadAttribute( "fieldToPointFunction_", fieldToPointFunction_ );
}

/* Composed implementation accessor */
Function FieldToPointConnection::getFunction() const
{
  return function_;
}

FieldFunction FieldToPointConnection::getFieldFunction() const
{
  return fieldFunction_;
}

/* Composed implementation accessor */
FieldToPointFunction FieldToPointConnection::getFieldToPointFunction() const
{
  return fieldToPointFunction_;
}

END_NAMESPACE_OPENTURNS
