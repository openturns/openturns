//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all dynamical function implementations
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
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
FieldFunctionImplementation::FieldFunctionImplementation(const UnsignedInteger spatialDimension)
  : PersistentObject()
  , spatialDimension_(spatialDimension)
  , inputDimension_(0)
  , outputDimension_(0)
  , inputDescription_()
  , outputDescription_()
  , callsNumber_(0)
{
  // Nothing to do
}

/* Default constructor */
FieldFunctionImplementation::FieldFunctionImplementation(const UnsignedInteger spatialDimension,
    const UnsignedInteger inputDimension,
    const UnsignedInteger outputDimension)
  : PersistentObject()
  , spatialDimension_(spatialDimension)
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
Bool FieldFunctionImplementation::operator ==(const FieldFunctionImplementation & other) const
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
String FieldFunctionImplementation::__str__(const String & offset) const
{
  return OSS(false) << offset << __repr__();
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

/* Accessor for the output mesh associated with the given input mesh */
Mesh FieldFunctionImplementation::getOutputMesh(const Mesh & inputMesh) const
{
  return inputMesh;
}


/* Operator () */
Point FieldFunctionImplementation::operator() (const Scalar timeStamp,
    const Point & inP) const
{
  return (*this)(Point(1, timeStamp), inP);
}

Point FieldFunctionImplementation::operator() (const Point & location,
    const Point & inP) const
{
  return (*this)(Field(Mesh(Sample(1, location), IndicesCollection()), Sample(1, inP))).getValues()[0];
}

Field FieldFunctionImplementation::operator() (const Field & inFld) const
{
  throw NotYetImplementedException(HERE) << "In FieldFunctionImplementation::operator() (const Field & inFld) const";
}

/* Operator () */
ProcessSample FieldFunctionImplementation::operator() (const ProcessSample & inPS) const
{
  if (inPS.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the given process sample has an invalid dimension. Expect a dimension " << getInputDimension() << ", got " << inPS.getDimension();
  if (inPS.getMesh().getDimension() != getSpatialDimension()) throw InvalidArgumentException(HERE) << "Error: the given process sample has an invalid mesh dimension. Expect a mesh dimension " << getSpatialDimension() << ", got " << inPS.getMesh().getDimension();
  const UnsignedInteger size = inPS.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the given process sample has a size of 0.";
  Field field0(operator()(inPS.getField(0)));
  ProcessSample outSample(field0.getMesh(), size, field0.getOutputDimension());
  outSample.setField(field0, 0);
  // Simple loop over the evaluation operator based on time series
  // The calls number is updated by these calls
  for (UnsignedInteger i = 1; i < size; ++i)
    outSample.setField(operator()(inPS.getField(i)), i);
  return outSample;
}

/* Accessor for mesh dimension */
UnsignedInteger FieldFunctionImplementation::getSpatialDimension() const
{
  return spatialDimension_;
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
  adv.saveAttribute( "spatialDimension_", spatialDimension_ );
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
  adv.loadAttribute( "spatialDimension_", spatialDimension_ );
  adv.loadAttribute( "inputDimension_", inputDimension_ );
  adv.loadAttribute( "outputDimension_", outputDimension_ );
  adv.loadAttribute( "inputDescription_", inputDescription_ );
  adv.loadAttribute( "outputDescription_", outputDescription_ );
  UnsignedInteger callsNumber;
  adv.loadAttribute( "callsNumber_", callsNumber );
  callsNumber_ = callsNumber;
}




END_NAMESPACE_OPENTURNS
