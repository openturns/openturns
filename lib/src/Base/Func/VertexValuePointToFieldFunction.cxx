//                                               -*- C++ -*-
/**
 *  @brief Function mapping a point to a field.
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

#include "openturns/VertexValuePointToFieldFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/PointToFieldFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(VertexValuePointToFieldFunction)

static const Factory<VertexValuePointToFieldFunction> Factory_VertexValuePointToFieldFunction;

/* Default constructor */
VertexValuePointToFieldFunction::VertexValuePointToFieldFunction()
  : PointToFieldFunctionImplementation()
{}

/* Constructor with parameters */
VertexValuePointToFieldFunction::VertexValuePointToFieldFunction(const Function & function,
    const Mesh & mesh)
  : PointToFieldFunctionImplementation(std::max(static_cast<SignedInteger>(function.getInputDimension()) - static_cast<SignedInteger>(mesh.getDimension()), static_cast<SignedInteger>(0)), mesh, function.getOutputDimension())
  , function_(function)
{
  if (!(function_.getInputDimension() >= mesh.getDimension()))
    throw InvalidArgumentException(HERE) << "Error: the given function should have an input dimension at least equal to the mesh dimension=" << mesh.getDimension() << ". Here input dimension=" << function_.getInputDimension();
  Description inputDescription(function_.getInputDescription());
  inputDescription.erase(inputDescription.begin(), inputDescription.begin() + mesh.getDimension());
  setInputDescription(inputDescription);
  setOutputDescription(function_.getOutputDescription());
}

/* Virtual constructor */
VertexValuePointToFieldFunction * VertexValuePointToFieldFunction::clone() const
{
  return new VertexValuePointToFieldFunction(*this);
}

/* Operator () */
Sample VertexValuePointToFieldFunction::operator() (const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  callsNumber_.increment();
  Sample inputValues(getOutputMesh().getVertices());
  inputValues.stack(Sample(getOutputMesh().getVerticesNumber(), inP));
  return function_(inputValues);
}

/** Get the function corresponding to indices components */
PointToFieldFunction VertexValuePointToFieldFunction::getMarginal(const Indices & indices) const
{
  return VertexValuePointToFieldFunction(function_.getMarginal(indices), getOutputMesh());
}

/* String converter */
String VertexValuePointToFieldFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " function=" << function_;
  return oss;
}


String VertexValuePointToFieldFunction::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << GetClassName()
      << "(" << function_ << ")";
  return oss;
}

/* Evaluation accessor */
Function VertexValuePointToFieldFunction::getFunction() const
{
  return function_;
}

/* Method save() stores the object through the StorageManager */
void VertexValuePointToFieldFunction::save(Advocate & adv) const
{
  PointToFieldFunctionImplementation::save(adv);
  adv.saveAttribute("function_", function_);
}

/* Method load() reloads the object from the StorageManager */
void VertexValuePointToFieldFunction::load(Advocate & adv)
{
  PointToFieldFunctionImplementation::load(adv);
  adv.loadAttribute("function_", function_);
}

}
