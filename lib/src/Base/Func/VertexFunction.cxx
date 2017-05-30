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
#include "openturns/VertexFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/NoEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(VertexFunction);

static const Factory<VertexFunction> Factory_VertexFunction;

/* Default constructor */
VertexFunction::VertexFunction()
  : FieldFunctionImplementation()
  , function_()
{
  // Nothing to do
}

/* Parameter constructor */
VertexFunction::VertexFunction(const Function & function)
  : FieldFunctionImplementation(function.getInputDimension(), 0, function.getOutputDimension())
  , function_(function)
{
  // Set the description
  setInputDescription(function_.getInputDescription());
  setOutputDescription(function_.getOutputDescription());
}

/* Parameter constructor */
VertexFunction::VertexFunction(const EvaluationPointer & p_evaluation)
  : FieldFunctionImplementation(p_evaluation->getInputDimension(), 0, p_evaluation->getOutputDimension())
  , function_(p_evaluation)
{
  // Set the description
  setInputDescription(function_.getInputDescription());
  setOutputDescription(function_.getOutputDescription());
}

/* Parameter constructor */
VertexFunction::VertexFunction(const EvaluationImplementation & evaluation)
  : FieldFunctionImplementation(evaluation.getInputDimension(), 0, evaluation.getOutputDimension())
  , function_(evaluation)
{
  // Set the description
  setInputDescription(function_.getInputDescription());
  setOutputDescription(function_.getOutputDescription());
}

/* Virtual constructor */
VertexFunction * VertexFunction::clone() const
{
  return new VertexFunction(*this);
}

/* Comparison operator */
Bool VertexFunction::operator ==(const VertexFunction & other) const
{
  return true;
}

/* String converter */
String VertexFunction::__repr__() const
{
  OSS oss(true);
  oss << "class=" << VertexFunction::GetClassName()
      << " evaluation=" << function_.__repr__();
  return oss;
}

/* String converter */
String VertexFunction::__str__(const String & offset) const
{
  return OSS(false) << function_.__str__(offset);
}

/* Operator () */
Field VertexFunction::operator() (const Field & inFld) const
{
  if (inFld.getSpatialDimension() != getSpatialDimension()) throw InvalidArgumentException(HERE) << "Error: expected a field with mesh dimension=" << getSpatialDimension() << ", got mesh dimension=" << inFld.getSpatialDimension();
  ++callsNumber_;
  return Field(inFld.getMesh(), function_(inFld.getMesh().getVertices()));
}

/* Get the i-th marginal function */
VertexFunction::Implementation VertexFunction::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return new VertexFunction(function_.getMarginal(i));
}

/* Get the function corresponding to indices components */
VertexFunction::Implementation VertexFunction::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  return new VertexFunction(function_.getMarginal(indices));
}

/* Method save() stores the object through the StorageManager */
void VertexFunction::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "function_", function_ );
}

/* Function accessor */
Function VertexFunction::getFunction() const
{
  return function_;
}

/* Method load() reloads the object from the StorageManager */
void VertexFunction::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "function_", function_ );
}




END_NAMESPACE_OPENTURNS
