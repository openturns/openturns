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

CLASSNAMEINIT(ValueFunction);

static const Factory<ValueFunction> Factory_ValueFunction;

/* Default constructor */
ValueFunction::ValueFunction(const UnsignedInteger meshDimension)
  : FieldFunctionImplementation(meshDimension)
  , p_evaluation_(new NoEvaluation)
{
  // Nothing to do
}

/* Parameter constructor */
ValueFunction::ValueFunction(const Function & function,
                                 const UnsignedInteger meshDimension)
  : FieldFunctionImplementation(meshDimension)
  , p_evaluation_(function.getEvaluation())
{
  // Set the descriptions
  setInputDescription(p_evaluation_->getInputDescription());
  setOutputDescription(p_evaluation_->getOutputDescription());
}

/* Parameter constructor */
ValueFunction::ValueFunction(const EvaluationPointer & p_evaluation,
                                 const UnsignedInteger meshDimension)
  : FieldFunctionImplementation(meshDimension)
  , p_evaluation_(p_evaluation)
{
  // Set the descriptions
  setInputDescription(p_evaluation_->getInputDescription());
  setOutputDescription(p_evaluation_->getOutputDescription());
}

/* Parameter constructor */
ValueFunction::ValueFunction(const EvaluationImplementation & evaluation,
                                 const UnsignedInteger meshDimension)
  : FieldFunctionImplementation(meshDimension)
  , p_evaluation_(evaluation.clone())
{
  // Set the descriptions
  setInputDescription(p_evaluation_->getInputDescription());
  setOutputDescription(p_evaluation_->getOutputDescription());
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
      << " evaluation=" << p_evaluation_->__repr__();
  return oss;
}

/* String converter */
String ValueFunction::__str__(const String & offset) const
{
  return OSS(false) << p_evaluation_->__str__(offset);
}

/* Operator () */
Field ValueFunction::operator() (const Field & inFld) const
{
  if (inFld.getSpatialDimension() != getSpatialDimension()) throw InvalidArgumentException(HERE) << "Error: expected a field with mesh dimension=" << getSpatialDimension() << ", got mesh dimension=" << inFld.getSpatialDimension();
  ++callsNumber_;
  return Field(inFld.getMesh(), (*p_evaluation_)(inFld.getValues()));
}

/* Get the i-th marginal function */
ValueFunction::Implementation ValueFunction::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return new ValueFunction(p_evaluation_->getMarginal(i));
}

/* Get the function corresponding to indices components */
ValueFunction::Implementation ValueFunction::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  return new ValueFunction(p_evaluation_->getMarginal(indices));
}

/* Method save() stores the object through the StorageManager */
void ValueFunction::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "evaluation_", *p_evaluation_ );
}

/* Evaluation accessor */
ValueFunction::EvaluationPointer ValueFunction::getEvaluation() const
{
  return p_evaluation_;
}

/* Method load() reloads the object from the StorageManager */
void ValueFunction::load(Advocate & adv)
{
  TypedInterfaceObject<EvaluationImplementation> evaluationValue;
  PersistentObject::load(adv);
  adv.loadAttribute( "evaluation_", evaluationValue );
  p_evaluation_ = evaluationValue.getImplementation();
}

END_NAMESPACE_OPENTURNS
