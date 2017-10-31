//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all spatial functions
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
#include "openturns/ValueFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/NoEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ValueFunction)

static const Factory<ValueFunction> Factory_ValueFunction;

/* Default constructor */
ValueFunction::ValueFunction(const UnsignedInteger meshDimension)
  : FieldFunctionImplementation(meshDimension)
  , function_()
{
  // Nothing to do
}

/* Parameter constructor */
ValueFunction::ValueFunction(const Function & function,
                             const UnsignedInteger meshDimension)
  : FieldFunctionImplementation(meshDimension, function.getInputDimension(), function.getOutputDimension())
  , function_(function)
{
  // Set the descriptions
  setInputDescription(function_.getInputDescription());
  setOutputDescription(function_.getOutputDescription());
}

/* Parameter constructor */
ValueFunction::ValueFunction(const EvaluationPointer & p_evaluation,
                             const UnsignedInteger meshDimension)
  : FieldFunctionImplementation(meshDimension, p_evaluation->getInputDimension(), p_evaluation->getOutputDimension())
  , function_(p_evaluation)
{
  // Set the descriptions
  setInputDescription(function_.getInputDescription());
  setOutputDescription(function_.getOutputDescription());
}

/* Parameter constructor */
ValueFunction::ValueFunction(const EvaluationImplementation & evaluation,
                             const UnsignedInteger meshDimension)
  : FieldFunctionImplementation(meshDimension, evaluation.getInputDimension(), evaluation.getOutputDimension())
  , function_(evaluation)
{
  // Set the descriptions
  setInputDescription(function_.getInputDescription());
  setOutputDescription(function_.getOutputDescription());
}

/* Virtual constructor */
ValueFunction * ValueFunction::clone() const
{
  return new ValueFunction(*this);
}

/* Comparison operator */
Bool ValueFunction::operator ==(const ValueFunction & other) const
{
  return true;
}

/* String converter */
String ValueFunction::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ValueFunction::GetClassName()
      << " evaluation=" << function_.__repr__();
  return oss;
}

/* String converter */
String ValueFunction::__str__(const String & offset) const
{
  return OSS(false) << function_.__str__(offset);
}

/* Operator () */
Field ValueFunction::operator() (const Field & inFld) const
{
  if (inFld.getInputDimension() != getSpatialDimension()) throw InvalidArgumentException(HERE) << "Error: expected a field with mesh dimension=" << getSpatialDimension() << ", got mesh dimension=" << inFld.getInputDimension();
  ++callsNumber_;
  return Field(inFld.getMesh(), function_(inFld.getValues()));
}

/* Get the i-th marginal function */
ValueFunction::Implementation ValueFunction::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return new ValueFunction(function_.getMarginal(i));
}

/* Get the function corresponding to indices components */
ValueFunction::Implementation ValueFunction::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  return new ValueFunction(function_.getMarginal(indices));
}

/* Method save() stores the object through the StorageManager */
void ValueFunction::save(Advocate & adv) const
{
  FieldFunctionImplementation::save(adv);
  adv.saveAttribute( "function_", function_ );
}

/* Function accessor */
Function ValueFunction::getFunction() const
{
  return function_;
}

/* Method load() reloads the object from the StorageManager */
void ValueFunction::load(Advocate & adv)
{
  FieldFunctionImplementation::load(adv);
  adv.loadAttribute( "function_", function_ );
}

END_NAMESPACE_OPENTURNS
