//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all temporal functions
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
#include "openturns/VertexValueFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/NoEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(VertexValueFunction)

static const Factory<VertexValueFunction> Factory_VertexValueFunction;

/* Default constructor */
VertexValueFunction::VertexValueFunction(const UnsignedInteger meshDimension)
  : FieldFunctionImplementation(meshDimension)
  , function_()
{
  // Nothing to do
}

/* Parameter constructor */
VertexValueFunction::VertexValueFunction(const Function & function,
    const UnsignedInteger meshDimension)
  : FieldFunctionImplementation(meshDimension, function.getInputDimension() - meshDimension, function.getOutputDimension())
  , function_(function)
{
  // Check that the given function has an input dimension large enough to be compatible with the mesh dimension
  if (function_.getInputDimension() < meshDimension) throw InvalidArgumentException(HERE) << "Error: the given function should have an input dimension at least equal to the mesh dimension=" << meshDimension << ". Here input dimension=" << function_.getInputDimension();
  // Set the descriptions
  Description inputDescription(function_.getInputDescription());
  inputDescription.erase(inputDescription.begin(), inputDescription.begin() + meshDimension);
  setInputDescription(inputDescription);
  setOutputDescription(function_.getOutputDescription());
}

/* Parameter constructor */
VertexValueFunction::VertexValueFunction(const EvaluationPointer & p_evaluation,
    const UnsignedInteger meshDimension)
  : FieldFunctionImplementation(meshDimension, p_evaluation->getInputDimension() - meshDimension, p_evaluation->getOutputDimension())
  , function_(p_evaluation)
{
  // Check that the given function has an input dimension large enough to be compatible with the mesh dimension
  if (function_.getInputDimension() < meshDimension) throw InvalidArgumentException(HERE) << "Error: the given function should have an input dimension at least equal to the mesh dimension=" << meshDimension << ". Here input dimension=" << function_.getInputDimension();
  // Set the descriptions
  Description inputDescription(function_.getInputDescription());
  inputDescription.erase(inputDescription.begin(), inputDescription.begin() + meshDimension);
  setInputDescription(inputDescription);
  setOutputDescription(function_.getOutputDescription());
}

/* Parameter constructor */
VertexValueFunction::VertexValueFunction(const EvaluationImplementation & evaluation,
    const UnsignedInteger meshDimension)
  : FieldFunctionImplementation(meshDimension, evaluation.getInputDimension() - meshDimension, evaluation.getOutputDimension())
  , function_(evaluation)
{
  // Check that the given function has an input dimension large enough to be compatible with the mesh dimension
  if (function_.getInputDimension() < meshDimension) throw InvalidArgumentException(HERE) << "Error: the given function should have an input dimension at least equal to the mesh dimension=" << meshDimension << ". Here input dimension=" << function_.getInputDimension();
  // Set the descriptions
  Description inputDescription(function_.getInputDescription());
  inputDescription.erase(inputDescription.begin(), inputDescription.begin() + meshDimension);
  setInputDescription(inputDescription);
  setOutputDescription(function_.getOutputDescription());
}

/* Virtual constructor */
VertexValueFunction * VertexValueFunction::clone() const
{
  return new VertexValueFunction(*this);
}

/* Comparison operator */
Bool VertexValueFunction::operator ==(const VertexValueFunction & other) const
{
  return true;
}

/* String converter */
String VertexValueFunction::__repr__() const
{
  OSS oss(true);
  oss << "class=" << VertexValueFunction::GetClassName()
      << " evaluation=" << function_.__repr__();
  return oss;
}

/* String converter */
String VertexValueFunction::__str__(const String & offset) const
{
  return OSS(false) << function_.__str__(offset);
}

/* Operator () */
Field VertexValueFunction::operator() (const Field & inFld) const
{
  if (inFld.getInputDimension() != getSpatialDimension()) throw InvalidArgumentException(HERE) << "Error: expected a field with mesh dimension=" << getSpatialDimension() << ", got mesh dimension=" << inFld.getInputDimension();
  ++callsNumber_;
  return Field(inFld.getMesh(), function_(inFld.getImplementation()->asSample()));
}

/* Get the i-th marginal function */
VertexValueFunction::Implementation VertexValueFunction::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return new VertexValueFunction(function_.getMarginal(i));
}

/* Get the function corresponding to indices components */
VertexValueFunction::Implementation VertexValueFunction::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  return new VertexValueFunction(function_.getMarginal(indices));
}

/* Evaluation accessor */
Function VertexValueFunction::getFunction() const
{
  return function_;
}

/* Method save() stores the object through the StorageManager */
void VertexValueFunction::save(Advocate & adv) const
{
  FieldFunctionImplementation::save(adv);
  adv.saveAttribute( "function_", function_ );
}

/* Method load() reloads the object from the StorageManager */
void VertexValueFunction::load(Advocate & adv)
{
  FieldFunctionImplementation::load(adv);
  adv.loadAttribute( "function_", function_ );
}




END_NAMESPACE_OPENTURNS
