//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all dynamical function implementations
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/DynamicalFunctionImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DynamicalFunctionImplementation);

static const Factory<DynamicalFunctionImplementation> Factory_DynamicalFunctionImplementation;

/* Default constructor */
DynamicalFunctionImplementation::DynamicalFunctionImplementation(const UnsignedInteger spatialDimension)
  : PersistentObject()
  , spatialDimension_(spatialDimension)
  , inputDescription_()
  , outputDescription_()
  , callsNumber_(0)
{
  // Nothing to do
}

/* Virtual constructor */
DynamicalFunctionImplementation * DynamicalFunctionImplementation::clone() const
{
  return new DynamicalFunctionImplementation(*this);
}

/* Comparison operator */
Bool DynamicalFunctionImplementation::operator ==(const DynamicalFunctionImplementation & other) const
{
  return true;
}

/* String converter */
String DynamicalFunctionImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << DynamicalFunctionImplementation::GetClassName()
      << " name=" << getName()
      << " input description=" << inputDescription_
      << " output description=" << outputDescription_
      << " number of calls=" << callsNumber_;
  return oss;
}

/* String converter */
String DynamicalFunctionImplementation::__str__(const String & offset) const
{
  return OSS(false) << offset << __repr__();
}

/* Get the i-th marginal function */
DynamicalFunctionImplementation::Implementation DynamicalFunctionImplementation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
DynamicalFunctionImplementation::Implementation DynamicalFunctionImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  throw NotYetImplementedException(HERE) << "In DynamicalFunctionImplementation::getMarginal(const Indices & indices) const";
}

/* Input description Accessor */
void DynamicalFunctionImplementation::setInputDescription(const Description & inputDescription)
{
  inputDescription_ = inputDescription;
}

Description DynamicalFunctionImplementation::getInputDescription() const
{
  return inputDescription_;
}

/* Output description Accessor */
void DynamicalFunctionImplementation::setOutputDescription(const Description & outputDescription)
{
  outputDescription_ = outputDescription;
}

Description DynamicalFunctionImplementation::getOutputDescription() const
{
  return outputDescription_;
}

/* Accessor for the output mesh associated with the given input mesh */
Mesh DynamicalFunctionImplementation::getOutputMesh(const Mesh & inputMesh) const
{
  return inputMesh;
}


/* Operator () */
NumericalPoint DynamicalFunctionImplementation::operator() (const NumericalScalar timeStamp,
    const NumericalPoint & inP) const
{
  return (*this)(NumericalPoint(1, timeStamp), inP);
}

NumericalPoint DynamicalFunctionImplementation::operator() (const NumericalPoint & location,
    const NumericalPoint & inP) const
{
  return (*this)(Field(Mesh(NumericalSample(1, location), Collection<Indices>(0)), NumericalSample(1, inP))).getValues()[0];
}

Field DynamicalFunctionImplementation::operator() (const Field & inFld) const
{
  throw NotYetImplementedException(HERE) << "In DynamicalFunctionImplementation::operator() (const Field & inFld) const";
}

/* Operator () */
ProcessSample DynamicalFunctionImplementation::operator() (const ProcessSample & inPS) const
{
  if (inPS.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the given process sample has an invalid dimension. Expect a dimension " << getInputDimension() << ", got " << inPS.getDimension();
  if (inPS.getMesh().getDimension() != getSpatialDimension()) throw InvalidArgumentException(HERE) << "Error: the given process sample has an invalid mesh dimension. Expect a mesh dimension " << getSpatialDimension() << ", got " << inPS.getMesh().getDimension();
  const UnsignedInteger size = inPS.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the given process sample has a size of 0.";
  Field field0(operator()(inPS.getField(0)));
  ProcessSample outSample(field0.getMesh(), size, field0.getDimension());
  outSample.setField(field0, 0);
  // Simple loop over the evaluation operator based on time series
  // The calls number is updated by these calls
  for (UnsignedInteger i = 1; i < size; ++i)
    outSample.setField(operator()(inPS.getField(i)), i);
  return outSample;
}

/* Accessor for mesh dimension */
UnsignedInteger DynamicalFunctionImplementation::getSpatialDimension() const
{
  return spatialDimension_;
}

/* Accessor for input point dimension */
UnsignedInteger DynamicalFunctionImplementation::getInputDimension() const
{
  return inputDescription_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger DynamicalFunctionImplementation::getOutputDimension() const
{
  return outputDescription_.getSize();
}

/* Number of calls to the dynamical function */
UnsignedInteger DynamicalFunctionImplementation::getCallsNumber() const
{
  return callsNumber_;
}

/* Method save() stores the object through the StorageManager */
void DynamicalFunctionImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "spatialDimension_", spatialDimension_ );
  adv.saveAttribute( "inputDescription_", inputDescription_ );
  adv.saveAttribute( "outputDescription_", outputDescription_ );
  adv.saveAttribute( "callsNumber_", callsNumber_ );
}

/* Method load() reloads the object from the StorageManager */
void DynamicalFunctionImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "spatialDimension_", spatialDimension_ );
  adv.loadAttribute( "inputDescription_", inputDescription_ );
  adv.loadAttribute( "outputDescription_", outputDescription_ );
  adv.loadAttribute( "callsNumber_", callsNumber_ );
}




END_NAMESPACE_OPENTURNS
