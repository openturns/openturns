//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all functions mapping points into fields
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
#include "openturns/PointToFieldFunction.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PointToFieldFunction)

/* Default constructor */
PointToFieldFunction::PointToFieldFunction()
  : TypedInterfaceObject<PointToFieldFunctionImplementation>(new PointToFieldFunctionImplementation())
{
  // Nothing to do
}

/* Parameter constructor */
PointToFieldFunction::PointToFieldFunction(const UnsignedInteger inputDimension,
    const Mesh & outputMesh,
    const UnsignedInteger outputDimension)
  : TypedInterfaceObject<PointToFieldFunctionImplementation>(new PointToFieldFunctionImplementation(inputDimension, outputMesh, outputDimension))
{
  // Nothing to do
}

/* Constructor from PointToFieldFunctionImplementation */
PointToFieldFunction::PointToFieldFunction(const PointToFieldFunctionImplementation & implementation)
  : TypedInterfaceObject<PointToFieldFunctionImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
PointToFieldFunction::PointToFieldFunction(const Implementation & p_implementation)
  : TypedInterfaceObject<PointToFieldFunctionImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
PointToFieldFunction::PointToFieldFunction(PointToFieldFunctionImplementation * p_implementation)
  : TypedInterfaceObject<PointToFieldFunctionImplementation>(p_implementation)
{
  // Nothing to do
}

/* Comparison operator */
Bool PointToFieldFunction::operator ==(const PointToFieldFunction & ) const
{
  return true;
}

/* String converter */
String PointToFieldFunction::__repr__() const
{
  return OSS(true) << "class=" << PointToFieldFunction::GetClassName()
         << " name=" << getName()
         << " implementation=" << getImplementation()->__repr__();
}

/* String converter */
String PointToFieldFunction::__str__(const String & offset) const
{
  return OSS(false) << getClassName() << " :" << Os::GetEndOfLine() << offset << "  " << getImplementation()->__str__( offset + "  ");
}

/* Get the i-th marginal function */
PointToFieldFunction PointToFieldFunction::getMarginal(const UnsignedInteger i) const
{
  return getImplementation()->getMarginal(i);
}

/* Get the function corresponding to indices components */
PointToFieldFunction PointToFieldFunction::getMarginal(const Indices & indices) const
{
  return getImplementation()->getMarginal(indices);
}

/* Input description Accessor */
void PointToFieldFunction::setInputDescription(const Description & inputDescription)
{
  copyOnWrite();
  getImplementation()->setInputDescription(inputDescription);
}

Description PointToFieldFunction::getInputDescription() const
{
  return getImplementation()->getInputDescription();
}

/* Output description Accessor */
void PointToFieldFunction::setOutputDescription(const Description & outputDescription)
{
  copyOnWrite();
  getImplementation()->setOutputDescription(outputDescription);
}

Description PointToFieldFunction::getOutputDescription() const
{
  return getImplementation()->getOutputDescription();
}

/* Output mesh Accessor */
Mesh PointToFieldFunction::getOutputMesh() const
{
  return getImplementation()->getOutputMesh();
}

/* Operator () */
Sample PointToFieldFunction::operator() (const Point & inP) const
{
  return getImplementation()->operator()(inP);
}

/* Operator () */
ProcessSample PointToFieldFunction::operator() (const Sample & inS) const
{
  return getImplementation()->operator()(inS);
}

/* Accessor for input point dimension */
UnsignedInteger PointToFieldFunction::getInputDimension() const
{
  return getImplementation()->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger PointToFieldFunction::getOutputDimension() const
{
  return getImplementation()->getOutputDimension();
}

/* Number of calls to the dynamical function */
UnsignedInteger PointToFieldFunction::getCallsNumber() const
{
  return getImplementation()->getCallsNumber();
}

END_NAMESPACE_OPENTURNS
