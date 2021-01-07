//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all field to point function implementations
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
#include "openturns/FieldToPointFunctionImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FieldToPointFunctionImplementation)

static const Factory<FieldToPointFunctionImplementation> Factory_FieldToPointFunctionImplementation;

/* Default constructor */
FieldToPointFunctionImplementation::FieldToPointFunctionImplementation()
  : PersistentObject()
  , inputMesh_(0)
  , inputDimension_(0)
  , outputDimension_(0)
  , inputDescription_()
  , outputDescription_()
  , callsNumber_(0)
{
  // Nothing to do
}

/* Parameter constructor */
FieldToPointFunctionImplementation::FieldToPointFunctionImplementation(const Mesh & inputMesh,
    const UnsignedInteger inputDimension,
    const UnsignedInteger outputDimension)
  : PersistentObject()
  , inputMesh_(inputMesh)
  , inputDimension_(inputDimension)
  , outputDimension_(outputDimension)
  , inputDescription_(Description::BuildDefault(inputDimension, "x"))
  , outputDescription_(Description::BuildDefault(outputDimension, "y"))
  , callsNumber_(0)
{
  // Nothing to do
}

/* Virtual constructor */
FieldToPointFunctionImplementation * FieldToPointFunctionImplementation::clone() const
{
  return new FieldToPointFunctionImplementation(*this);
}

/* Comparison operator */
Bool FieldToPointFunctionImplementation::operator ==(const FieldToPointFunctionImplementation & ) const
{
  return true;
}

/* String converter */
String FieldToPointFunctionImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << FieldToPointFunctionImplementation::GetClassName()
      << " name=" << getName()
      << " input description=" << inputDescription_
      << " output description=" << outputDescription_
      << " number of calls=" << callsNumber_.get();
  return oss;
}

/* String converter */
String FieldToPointFunctionImplementation::__str__(const String & ) const
{
  return OSS(false) << __repr__();
}

/* Get the i-th marginal function */
FieldToPointFunctionImplementation::Implementation FieldToPointFunctionImplementation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
FieldToPointFunctionImplementation::Implementation FieldToPointFunctionImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  throw NotYetImplementedException(HERE) << "In FieldToPointFunctionImplementation::getMarginal(const Indices & indices) const";
}

/* Input description Accessor */
void FieldToPointFunctionImplementation::setInputDescription(const Description & inputDescription)
{
  if (inputDescription.getSize() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: expected an input description of size=" << getInputDimension() << ", got size=" << inputDescription.getSize();
  inputDescription_ = inputDescription;
}

Description FieldToPointFunctionImplementation::getInputDescription() const
{
  return inputDescription_;
}

/* Output description Accessor */
void FieldToPointFunctionImplementation::setOutputDescription(const Description & outputDescription)
{
  if (outputDescription.getSize() != getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: expected an output description of size=" << getOutputDimension() << ", got size=" << outputDescription.getSize();
  outputDescription_ = outputDescription;
}

Description FieldToPointFunctionImplementation::getOutputDescription() const
{
  return outputDescription_;
}


/* Operator () */
Point FieldToPointFunctionImplementation::operator() (const Sample & ) const
{
  throw NotYetImplementedException(HERE) << "In FieldToPointFunctionImplementation::operator() (const Field & inFld) const";
}

/* Operator () */
Sample FieldToPointFunctionImplementation::operator() (const ProcessSample & inPS) const
{
  if (inPS.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the given process sample has an invalid dimension. Expect a dimension " << getInputDimension() << ", got " << inPS.getDimension();
  if (inPS.getMesh().getDimension() != getInputMesh().getDimension()) throw InvalidArgumentException(HERE) << "Error: the given process sample has an invalid mesh dimension. Expect a mesh dimension " << getInputMesh().getDimension() << ", got " << inPS.getMesh().getDimension();
  const UnsignedInteger size = inPS.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the given process sample has a size of 0.";
  Sample outSample(inPS.getSize(), getOutputDimension());
  // Simple loop over the evaluation operator based on time series
  // The calls number is updated by these calls
  for (UnsignedInteger i = 0; i < size; ++i)
    outSample[i] = operator()(inPS[i]);
  callsNumber_.fetchAndAdd(size);
  return outSample;
}

/* Accessor for input mesh */
Mesh FieldToPointFunctionImplementation::getInputMesh() const
{
  return inputMesh_;
}

/* Accessor for input point dimension */
UnsignedInteger FieldToPointFunctionImplementation::getInputDimension() const
{
  return inputDimension_;
}

/* Accessor for output point dimension */
UnsignedInteger FieldToPointFunctionImplementation::getOutputDimension() const
{
  return outputDimension_;
}

/* Number of calls to the dynamical function */
UnsignedInteger FieldToPointFunctionImplementation::getCallsNumber() const
{
  return callsNumber_.get();
}

/* Method save() stores the object through the StorageManager */
void FieldToPointFunctionImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "inputMesh_", inputMesh_ );
  adv.saveAttribute( "inputDimension_", inputDimension_ );
  adv.saveAttribute( "outputDimension_", outputDimension_ );
  adv.saveAttribute( "inputDescription_", inputDescription_ );
  adv.saveAttribute( "outputDescription_", outputDescription_ );
  adv.saveAttribute( "callsNumber_", static_cast<UnsignedInteger>(callsNumber_.get()) );
}

/* Method load() reloads the object from the StorageManager */
void FieldToPointFunctionImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "inputMesh_", inputMesh_ );
  adv.loadAttribute( "inputDimension_", inputDimension_ );
  adv.loadAttribute( "outputDimension_", outputDimension_ );
  adv.loadAttribute( "inputDescription_", inputDescription_ );
  adv.loadAttribute( "outputDescription_", outputDescription_ );
  UnsignedInteger callsNumber;
  adv.loadAttribute( "callsNumber_", callsNumber );
  callsNumber_ = callsNumber;
}




END_NAMESPACE_OPENTURNS
