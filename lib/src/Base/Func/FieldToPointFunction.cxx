//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all functions mapping fields to points
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/FieldToPointFunction.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FieldToPointFunction)

/* Default constructor */
FieldToPointFunction::FieldToPointFunction()
  : TypedInterfaceObject<FieldToPointFunctionImplementation>(new FieldToPointFunctionImplementation())
{
  // Nothing to do
}

/* Parameter constructor */
FieldToPointFunction::FieldToPointFunction(const Mesh & inputMesh,
    const UnsignedInteger inputDimension,
    const UnsignedInteger outputDimension)
  : TypedInterfaceObject<FieldToPointFunctionImplementation>(new FieldToPointFunctionImplementation(inputMesh, inputDimension, outputDimension))
{
  // Nothing to do
}

/* Constructor from FieldToPointFunctionImplementation */
FieldToPointFunction::FieldToPointFunction(const FieldToPointFunctionImplementation & implementation)
  : TypedInterfaceObject<FieldToPointFunctionImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
FieldToPointFunction::FieldToPointFunction(const Implementation & p_implementation)
  : TypedInterfaceObject<FieldToPointFunctionImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
FieldToPointFunction::FieldToPointFunction(FieldToPointFunctionImplementation * p_implementation)
  : TypedInterfaceObject<FieldToPointFunctionImplementation>(p_implementation)
{
  // Nothing to do
}

/* Comparison operator */
Bool FieldToPointFunction::operator ==(const FieldToPointFunction & other) const
{
  return *getImplementation() == *other.getImplementation();
}

/* String converter */
String FieldToPointFunction::__repr__() const
{
  return OSS(true) << "class=" << FieldToPointFunction::GetClassName()
         << " name=" << getName()
         << " implementation=" << getImplementation()->__repr__();
}

/* String converter */
String FieldToPointFunction::__str__(const String & offset) const
{
  return OSS(false) << getClassName() << " :" << "\n" << offset << "  " << getImplementation()->__str__( offset + "  ");
}

/* Get the i-th marginal function */
FieldToPointFunction FieldToPointFunction::getMarginal(const UnsignedInteger i) const
{
  return *(getImplementation()->getMarginal(i));
}

/* Get the function corresponding to indices components */
FieldToPointFunction FieldToPointFunction::getMarginal(const Indices & indices) const
{
  return *(getImplementation()->getMarginal(indices));
}

/* Input description Accessor */
Description FieldToPointFunction::getInputDescription() const
{
  return getImplementation()->getInputDescription();
}

void FieldToPointFunction::setInputDescription(const Description & inputDescription)
{
  copyOnWrite();
  getImplementation()->setInputDescription(inputDescription);
}

/* Output description Accessor */
Description FieldToPointFunction::getOutputDescription() const
{
  return getImplementation()->getOutputDescription();
}

void FieldToPointFunction::setOutputDescription(const Description & outputDescription)
{
  copyOnWrite();
  getImplementation()->setOutputDescription(outputDescription);
}

/* Operator () */
Point FieldToPointFunction::operator() (const Sample & inFld) const
{
  return getImplementation()->operator()(inFld);
}

/* Operator () */
Sample FieldToPointFunction::operator() (const ProcessSample & inPS) const
{
  return getImplementation()->operator()(inPS);
}

/* Accessor for input mesh */
Mesh FieldToPointFunction::getInputMesh() const
{
  return getImplementation()->getInputMesh();
}

/* Accessor for input point dimension */
UnsignedInteger FieldToPointFunction::getInputDimension() const
{
  return getImplementation()->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger FieldToPointFunction::getOutputDimension() const
{
  return getImplementation()->getOutputDimension();
}

/* Number of calls to the dynamical function */
UnsignedInteger FieldToPointFunction::getCallsNumber() const
{
  return getImplementation()->getCallsNumber();
}

END_NAMESPACE_OPENTURNS
