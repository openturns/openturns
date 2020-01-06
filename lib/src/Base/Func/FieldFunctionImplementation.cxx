//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all dynamical function implementations
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/FieldFunctionImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FieldFunctionImplementation)

static const Factory<FieldFunctionImplementation> Factory_FieldFunctionImplementation;

/* Default constructor */
FieldFunctionImplementation::FieldFunctionImplementation()
  : PersistentObject()
  , inputDimension_(0)
  , outputDimension_(0)
  , inputDescription_()
  , outputDescription_()
  , callsNumber_(0)
{
  // Nothing to do
}

/* Default constructor */
FieldFunctionImplementation::FieldFunctionImplementation(const Mesh & inputMesh,
    const UnsignedInteger inputDimension,
    const Mesh & outputMesh,
    const UnsignedInteger outputDimension)
  : PersistentObject()
  , inputMesh_(inputMesh)
  , outputMesh_(outputMesh)
  , inputDimension_(inputDimension)
  , outputDimension_(outputDimension)
  , inputDescription_(Description::BuildDefault(inputDimension, "x"))
  , outputDescription_(Description::BuildDefault(outputDimension, "y"))
  , callsNumber_(0)
{
  // Nothing to do
}

/* Virtual constructor */
FieldFunctionImplementation * FieldFunctionImplementation::clone() const
{
  return new FieldFunctionImplementation(*this);
}

/* Comparison operator */
Bool FieldFunctionImplementation::operator ==(const FieldFunctionImplementation & ) const
{
  return true;
}

/* String converter */
String FieldFunctionImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << FieldFunctionImplementation::GetClassName()
      << " name=" << getName()
      << " input description=" << inputDescription_
      << " output description=" << outputDescription_
      << " number of calls=" << callsNumber_.get();
  return oss;
}

/* String converter */
String FieldFunctionImplementation::__str__(const String & ) const
{
  return OSS(false) << __repr__();
}

/* Get the i-th marginal function */
FieldFunctionImplementation::Implementation FieldFunctionImplementation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
FieldFunctionImplementation::Implementation FieldFunctionImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  throw NotYetImplementedException(HERE) << "In FieldFunctionImplementation::getMarginal(const Indices & indices) const";
}

/* Input description Accessor */
void FieldFunctionImplementation::setInputDescription(const Description & inputDescription)
{
  inputDescription_ = inputDescription;
}

Description FieldFunctionImplementation::getInputDescription() const
{
  return inputDescription_;
}

/* Output description Accessor */
void FieldFunctionImplementation::setOutputDescription(const Description & outputDescription)
{
  outputDescription_ = outputDescription;
}

Description FieldFunctionImplementation::getOutputDescription() const
{
  return outputDescription_;
}

/* Accessor for the input mesh */
void FieldFunctionImplementation::setInputMesh(const Mesh & inputMesh)
{
  if (inputMesh.getDimension() != inputMesh_.getDimension())
    throw InvalidArgumentException(HERE) << "Mesh must have the same dimension";

  inputMesh_ = inputMesh;
}

Mesh FieldFunctionImplementation::getInputMesh() const
{
  return inputMesh_;
}

/* Accessor for the output mesh */
void FieldFunctionImplementation::setOutputMesh(const Mesh & outputMesh)
{
  if (outputMesh.getDimension() != outputMesh_.getDimension())
    throw InvalidArgumentException(HERE) << "Mesh must have the same dimension";
  outputMesh_ = outputMesh;
}

Mesh FieldFunctionImplementation::getOutputMesh() const
{
  return outputMesh_;
}

Bool FieldFunctionImplementation::isActingPointwise() const
{
  return false;
}

/* Operator () */
Sample FieldFunctionImplementation::operator() (const Sample & ) const
{
  throw NotYetImplementedException(HERE) << "In FieldFunctionImplementation::operator() (const Sample & inFld) const";
}

/* Operator () */
ProcessSample FieldFunctionImplementation::operator() (const ProcessSample & inPS) const
{
  if (inPS.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the given process sample has an invalid dimension. Expect a dimension " << getInputDimension() << ", got " << inPS.getDimension();
  if (inPS.getMesh().getDimension() != getInputMesh().getDimension()) throw InvalidArgumentException(HERE) << "Error: the given process sample has an invalid mesh dimension. Expect a mesh dimension " << getInputMesh().getDimension() << ", got " << inPS.getMesh().getDimension();
  const UnsignedInteger size = inPS.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the given process sample has a size of 0.";
  ProcessSample outSample(getOutputMesh(), size, getOutputDimension());
  // Simple loop over the evaluation operator based on time series
  // The calls number is updated by these calls
  for (UnsignedInteger i = 0; i < size; ++i)
    outSample[i] = operator()(inPS[i]);
  return outSample;
}

/* Accessor for input point dimension */
UnsignedInteger FieldFunctionImplementation::getInputDimension() const
{
  return inputDimension_;
}

/* Accessor for output point dimension */
UnsignedInteger FieldFunctionImplementation::getOutputDimension() const
{
  return outputDimension_;
}

/* Number of calls to the dynamical function */
UnsignedInteger FieldFunctionImplementation::getCallsNumber() const
{
  return callsNumber_.get();
}

/* Method save() stores the object through the StorageManager */
void FieldFunctionImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "inputMesh_", inputMesh_ );
  adv.saveAttribute( "outputMesh_", outputMesh_ );
  adv.saveAttribute( "inputDimension_", inputDimension_ );
  adv.saveAttribute( "outputDimension_", outputDimension_ );
  adv.saveAttribute( "inputDescription_", inputDescription_ );
  adv.saveAttribute( "outputDescription_", outputDescription_ );
  adv.saveAttribute( "callsNumber_", static_cast<UnsignedInteger>(callsNumber_.get()) );
}

/* Method load() reloads the object from the StorageManager */
void FieldFunctionImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "inputMesh_", inputMesh_ );
  adv.loadAttribute( "outputMesh_", outputMesh_ );
  adv.loadAttribute( "inputDimension_", inputDimension_ );
  adv.loadAttribute( "outputDimension_", outputDimension_ );
  adv.loadAttribute( "inputDescription_", inputDescription_ );
  adv.loadAttribute( "outputDescription_", outputDescription_ );
  UnsignedInteger callsNumber;
  adv.loadAttribute( "callsNumber_", callsNumber );
  callsNumber_ = callsNumber;
}




END_NAMESPACE_OPENTURNS
