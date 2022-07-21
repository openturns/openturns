//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all dynamical functions
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
#include "openturns/FieldFunction.hxx"
#include "openturns/ValueFunction.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FieldFunction)

/* Default constructor */
FieldFunction::FieldFunction()
  : TypedInterfaceObject<FieldFunctionImplementation>(new FieldFunctionImplementation())
{
  // Nothing to do
}

/* Parameter constructor */
FieldFunction::FieldFunction(const Mesh & inputMesh,
                             const UnsignedInteger inputDimension,
                             const Mesh & outputMesh,
                             const UnsignedInteger outputDimension)
  : TypedInterfaceObject<FieldFunctionImplementation>(new FieldFunctionImplementation(inputMesh, inputDimension, outputMesh, outputDimension))
{
  // Nothing to do
}

/* Constructor from FieldFunctionImplementation */
FieldFunction::FieldFunction(const FieldFunctionImplementation & implementation)
  : TypedInterfaceObject<FieldFunctionImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
FieldFunction::FieldFunction(const Implementation & p_implementation)
  : TypedInterfaceObject<FieldFunctionImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
FieldFunction::FieldFunction(FieldFunctionImplementation * p_implementation)
  : TypedInterfaceObject<FieldFunctionImplementation>(p_implementation)
{
  // Nothing to do
}

/* Comparison operator */
Bool FieldFunction::operator ==(const FieldFunction & ) const
{
  return true;
}

/* String converter */
String FieldFunction::__repr__() const
{
  return OSS(true) << "class=" << FieldFunction::GetClassName()
         << " name=" << getName()
         << " implementation=" << getImplementation()->__repr__();
}

/* String converter */
String FieldFunction::__str__(const String & offset) const
{
  return OSS(false) << getClassName() << " :" << Os::GetEndOfLine() << offset << "  " << getImplementation()->__str__( offset + "  ");
}

/* Get the i-th marginal function */
FieldFunction FieldFunction::getMarginal(const UnsignedInteger i) const
{
  return *(getImplementation()->getMarginal(i));
}

/* Get the function corresponding to indices components */
FieldFunction FieldFunction::getMarginal(const Indices & indices) const
{
  return *(getImplementation()->getMarginal(indices));
}

/* Input description Accessor */
Description FieldFunction::getInputDescription() const
{
  return getImplementation()->getInputDescription();
}

/* Output description Accessor */
Description FieldFunction::getOutputDescription() const
{
  return getImplementation()->getOutputDescription();
}

/* Input mesh accessor */
void FieldFunction::setInputMesh(const Mesh & inputMesh)
{
  getImplementation()->setInputMesh(inputMesh);
}

Mesh FieldFunction::getInputMesh() const
{
  return getImplementation()->getInputMesh();
}

/* Output mesh accessor */
void FieldFunction::setOutputMesh(const Mesh & outputMesh)
{
  getImplementation()->setOutputMesh(outputMesh);
}

Mesh FieldFunction::getOutputMesh() const
{
  return getImplementation()->getOutputMesh();
}

Bool FieldFunction::isActingPointwise() const
{
  return getImplementation()->isActingPointwise();
}

/* Operator () */
Sample FieldFunction::operator() (const Sample & inFld) const
{
  return getImplementation()->operator()(inFld);
}

/* Operator () */
ProcessSample FieldFunction::operator() (const ProcessSample & inPS) const
{
  return getImplementation()->operator()(inPS);
}

/* Accessor for input point dimension */
UnsignedInteger FieldFunction::getInputDimension() const
{
  return getImplementation()->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger FieldFunction::getOutputDimension() const
{
  return getImplementation()->getOutputDimension();
}

/* Number of calls to the dynamical function */
UnsignedInteger FieldFunction::getCallsNumber() const
{
  return getImplementation()->getCallsNumber();
}

END_NAMESPACE_OPENTURNS
