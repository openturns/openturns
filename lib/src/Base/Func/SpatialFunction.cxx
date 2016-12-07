//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all spatial functions
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/SpatialFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/NumericalMathEvaluationImplementation.hxx"
#include "openturns/NoNumericalMathEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SpatialFunction);

static const Factory<SpatialFunction> Factory_SpatialFunction;

/* Default constructor */
SpatialFunction::SpatialFunction(const UnsignedInteger meshDimension)
  : DynamicalFunctionImplementation(meshDimension)
  , p_evaluation_(new NoNumericalMathEvaluationImplementation)
{
  // Nothing to do
}

/* Parameter constructor */
SpatialFunction::SpatialFunction(const NumericalMathFunction & function,
                                 const UnsignedInteger meshDimension)
  : DynamicalFunctionImplementation(meshDimension)
  , p_evaluation_(function.getEvaluation())
{
  // Set the descriptions
  setInputDescription(p_evaluation_->getInputDescription());
  setOutputDescription(p_evaluation_->getOutputDescription());
}

/* Parameter constructor */
SpatialFunction::SpatialFunction(const EvaluationImplementation & p_evaluation,
                                 const UnsignedInteger meshDimension)
  : DynamicalFunctionImplementation(meshDimension)
  , p_evaluation_(p_evaluation)
{
  // Set the descriptions
  setInputDescription(p_evaluation_->getInputDescription());
  setOutputDescription(p_evaluation_->getOutputDescription());
}

/* Parameter constructor */
SpatialFunction::SpatialFunction(const NumericalMathEvaluationImplementation & evaluation,
                                 const UnsignedInteger meshDimension)
  : DynamicalFunctionImplementation(meshDimension)
  , p_evaluation_(evaluation.clone())
{
  // Set the descriptions
  setInputDescription(p_evaluation_->getInputDescription());
  setOutputDescription(p_evaluation_->getOutputDescription());
}

/* Virtual constructor */
SpatialFunction * SpatialFunction::clone() const
{
  return new SpatialFunction(*this);
}

/* Comparison operator */
Bool SpatialFunction::operator ==(const SpatialFunction & other) const
{
  return true;
}

/* String converter */
String SpatialFunction::__repr__() const
{
  OSS oss(true);
  oss << "class=" << SpatialFunction::GetClassName()
      << " evaluation=" << p_evaluation_->__repr__();
  return oss;
}

/* String converter */
String SpatialFunction::__str__(const String & offset) const
{
  return OSS(false) << p_evaluation_->__str__(offset);
}

/* Operator () */
Field SpatialFunction::operator() (const Field & inFld) const
{
  if (inFld.getSpatialDimension() != getSpatialDimension()) throw InvalidArgumentException(HERE) << "Error: expected a field with mesh dimension=" << getSpatialDimension() << ", got mesh dimension=" << inFld.getSpatialDimension();
  ++callsNumber_;
  return Field(inFld.getMesh(), (*p_evaluation_)(inFld.getValues()));
}

/* Get the i-th marginal function */
SpatialFunction::Implementation SpatialFunction::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return new SpatialFunction(p_evaluation_->getMarginal(i));
}

/* Get the function corresponding to indices components */
SpatialFunction::Implementation SpatialFunction::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  return new SpatialFunction(p_evaluation_->getMarginal(indices));
}

/* Method save() stores the object through the StorageManager */
void SpatialFunction::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "evaluation_", *p_evaluation_ );
}

/* Evaluation accessor */
SpatialFunction::EvaluationImplementation SpatialFunction::getEvaluation() const
{
  return p_evaluation_;
}

/* Method load() reloads the object from the StorageManager */
void SpatialFunction::load(Advocate & adv)
{
  TypedInterfaceObject<NumericalMathEvaluationImplementation> evaluationValue;
  PersistentObject::load(adv);
  adv.loadAttribute( "evaluation_", evaluationValue );
  p_evaluation_ = evaluationValue.getImplementation();
}

END_NAMESPACE_OPENTURNS
