//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all point to field function implementations
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
#include "openturns/PointToFieldFunctionImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/PointToFieldFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PointToFieldFunctionImplementation)

static const Factory<PointToFieldFunctionImplementation> Factory_PointToFieldFunctionImplementation;

/* Default constructor */
PointToFieldFunctionImplementation::PointToFieldFunctionImplementation()
  : PersistentObject()
  , outputMesh_(0)
  , inputDimension_(0)
  , outputDimension_(0)
  , inputDescription_()
  , outputDescription_()
  , callsNumber_(0)
{
  // Nothing to do
}

/* Parameter constructor */
PointToFieldFunctionImplementation::PointToFieldFunctionImplementation(const UnsignedInteger inputDimension,
    const Mesh & outputMesh,
    const UnsignedInteger outputDimension)
  : PersistentObject()
  , outputMesh_(outputMesh)
  , inputDimension_(inputDimension)
  , outputDimension_(outputDimension)
  , inputDescription_(Description::BuildDefault(inputDimension, "x"))
  , outputDescription_(Description::BuildDefault(outputDimension, "x"))
  , callsNumber_(0)
{
  // Nothing to do
}

/* Virtual constructor */
PointToFieldFunctionImplementation * PointToFieldFunctionImplementation::clone() const
{
  return new PointToFieldFunctionImplementation(*this);
}

/* Comparison operator */
Bool PointToFieldFunctionImplementation::operator ==(const PointToFieldFunctionImplementation & ) const
{
  return true;
}

/* String converter */
String PointToFieldFunctionImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << PointToFieldFunctionImplementation::GetClassName()
      << " name=" << getName()
      << " input description=" << inputDescription_
      << " output description=" << outputDescription_
      << " number of calls=" << callsNumber_.get();
  return oss;
}

/* String converter */
String PointToFieldFunctionImplementation::__str__(const String & ) const
{
  return OSS(false) << __repr__();
}

/* Get the i-th marginal function */
PointToFieldFunction PointToFieldFunctionImplementation::getMarginal(const UnsignedInteger i) const
{
  if (!(i < getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
PointToFieldFunction PointToFieldFunctionImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  throw NotYetImplementedException(HERE) << "In PointToFieldFunctionImplementation::getMarginal(const Indices & indices) const";
}

/* Input description Accessor */
void PointToFieldFunctionImplementation::setInputDescription(const Description & inputDescription)
{
  if (inputDescription.getSize() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: expected an input description of size=" << getInputDimension() << ", got size=" << inputDescription.getSize();
  inputDescription_ = inputDescription;
}

Description PointToFieldFunctionImplementation::getInputDescription() const
{
  return inputDescription_;
}

/* Output description Accessor */
void PointToFieldFunctionImplementation::setOutputDescription(const Description & outputDescription)
{
  if (outputDescription.getSize() != getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: expected an output description of size=" << getOutputDimension() << ", got size=" << outputDescription.getSize();
  outputDescription_ = outputDescription;
}

Description PointToFieldFunctionImplementation::getOutputDescription() const
{
  return outputDescription_;
}

/* Accessor for the output mesh */
Mesh PointToFieldFunctionImplementation::getOutputMesh() const
{
  return outputMesh_;
}


/* Operator () */
Sample PointToFieldFunctionImplementation::operator() (const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In PointToFieldFunctionImplementation::operator() (const Point & inP) const";
}

/* Operator () */
ProcessSample PointToFieldFunctionImplementation::operator() (const Sample & inS) const
{
  if (inS.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << getInputDimension() << ", got " << inS.getDimension();
  const UnsignedInteger size = inS.getSize();
  if (!(size > 0)) throw InvalidArgumentException(HERE) << "Error: the given sample has null size.";
  ProcessSample outSample(getOutputMesh(), size, getOutputDimension());
  // Simple loop over the evaluation operator based on time series
  // The calls number is updated by these calls
  for (UnsignedInteger i = 0; i < size; ++i)
    outSample[i] = operator()(inS[i]);
  return outSample;
}

/* Accessor for input point dimension */
UnsignedInteger PointToFieldFunctionImplementation::getInputDimension() const
{
  return inputDimension_;
}

/* Accessor for output point dimension */
UnsignedInteger PointToFieldFunctionImplementation::getOutputDimension() const
{
  return outputDimension_;
}

/* Number of calls to the dynamical function */
UnsignedInteger PointToFieldFunctionImplementation::getCallsNumber() const
{
  return callsNumber_.get();
}

/* Method save() stores the object through the StorageManager */
void PointToFieldFunctionImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "inputDimension_", inputDimension_ );
  adv.saveAttribute( "outputDimension_", outputDimension_ );
  adv.saveAttribute( "outputMesh_", outputMesh_ );
  adv.saveAttribute( "inputDescription_", inputDescription_ );
  adv.saveAttribute( "outputDescription_", outputDescription_ );
  adv.saveAttribute( "callsNumber_", static_cast<UnsignedInteger>(callsNumber_.get()) );
}

/* Method load() reloads the object from the StorageManager */
void PointToFieldFunctionImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "inputDimension_", inputDimension_ );
  adv.loadAttribute( "outputDimension_", outputDimension_ );
  adv.loadAttribute( "outputMesh_", outputMesh_ );
  adv.loadAttribute( "inputDescription_", inputDescription_ );
  adv.loadAttribute( "outputDescription_", outputDescription_ );
  UnsignedInteger callsNumber;
  adv.loadAttribute( "callsNumber_", callsNumber );
  callsNumber_ = callsNumber;
}




END_NAMESPACE_OPENTURNS
