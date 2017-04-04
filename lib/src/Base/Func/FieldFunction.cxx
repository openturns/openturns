//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all dynamical functions
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
#include "openturns/FieldFunction.hxx"
#include "openturns/ValueFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FieldFunction);

/* Default constructor */
FieldFunction::FieldFunction(const UnsignedInteger spatialDimension)
  : TypedInterfaceObject<FieldFunctionImplementation>(new FieldFunctionImplementation(spatialDimension))
{
  // Nothing to do
}

/* Constructor from Function */
FieldFunction::FieldFunction(const Function & function,
                             const UnsignedInteger spatialDimension)
  : TypedInterfaceObject<FieldFunctionImplementation>(ValueFunction(function, spatialDimension).clone())
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
Bool FieldFunction::operator ==(const FieldFunction & other) const
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
  return OSS(false) << offset << getClassName() << " :\n" << getImplementation()->__str__( offset + "  ");
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

/* Output mesh Accessor */
Mesh FieldFunction::getOutputMesh(const Mesh & inputMesh) const
{
  return getImplementation()->getOutputMesh(inputMesh);
}

/* Operator () */
Point FieldFunction::operator() (const Scalar timeStamp,
                                 const Point & inP) const
{
  return getImplementation()->operator()(timeStamp, inP);
}

Point FieldFunction::operator() (const Point & location,
                                 const Point & inP) const
{
  return getImplementation()->operator()(location, inP);
}

Field FieldFunction::operator() (const Field & inFld) const
{
  return getImplementation()->operator()(inFld);
}

/* Operator () */
ProcessSample FieldFunction::operator() (const ProcessSample & inPS) const
{
  return getImplementation()->operator()(inPS);
}

/* Accessor for mesh dimension */
UnsignedInteger FieldFunction::getSpatialDimension() const
{
  return getImplementation()->getSpatialDimension();
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
