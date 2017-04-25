//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for the trend transform
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
#include "openturns/TrendTransform.hxx"
#include "openturns/TrendEvaluation.hxx"
#include "openturns/InverseTrendTransform.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TrendTransform);

static const Factory<TrendTransform> Factory_TrendTransform;

/* Default constructor */
TrendTransform::TrendTransform()
  : VertexValueFunction()
{
  // Nothing to do
}

/* Parameter constructor */
TrendTransform::TrendTransform(const Function & function)
  : VertexValueFunction(TrendEvaluation(function), function.getInputDimension())
{
  // Nothing to do
}

/* Parameter constructor */
TrendTransform::TrendTransform(const EvaluationPointer & p_evaluation)
  : VertexValueFunction(TrendEvaluation(*p_evaluation), p_evaluation->getInputDimension())
{
  // Nothing to do
}

/* Parameter constructor */
TrendTransform::TrendTransform(const EvaluationImplementation & evaluation)
  : VertexValueFunction(TrendEvaluation(evaluation), evaluation.getInputDimension())
{
  // Nothing to do
}

/* Virtual constructor */
TrendTransform * TrendTransform::clone() const
{
  return new TrendTransform(*this);
}

/* Comparison operator */
Bool TrendTransform::operator ==(const TrendTransform & other) const
{
  return (getEvaluation() == other.getEvaluation());
}

/* String converter */
String TrendTransform::__repr__() const
{
  OSS oss(true);
  oss << "class=" << TrendTransform::GetClassName()
      << " evaluation=" << p_evaluation_->__repr__();
  return oss;
}

/* String converter */
String TrendTransform::__str__(const String & offset) const
{
  return OSS(false) << p_evaluation_->__str__(offset);
}

/* Operator () */
Field TrendTransform::operator() (const Field & inFld) const
{
  if (inFld.getSpatialDimension() != p_evaluation_->getInputDimension()) throw InvalidArgumentException(HERE) << "Error: expected a Field with mesh dimension=" << p_evaluation_->getInputDimension() << ", got mesh dimension=" << inFld.getSpatialDimension();
  if (inFld.getDimension() != p_evaluation_->getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: expected a Field with dimension=" << p_evaluation_->getOutputDimension() << ", got dimension=" << inFld.getDimension();
  ++callsNumber_;
  return Field(inFld.getMesh(), inFld.getValues() + (*p_evaluation_)(inFld.getMesh().getVertices()));
}

/* Inverse accessor */
InverseTrendTransform TrendTransform::getInverse() const
{
  return InverseTrendTransform(p_evaluation_);
}

/* Method save() stores the object through the StorageManager */
void TrendTransform::save(Advocate & adv) const
{
  VertexValueFunction::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void TrendTransform::load(Advocate & adv)
{
  VertexValueFunction::load(adv);
}

END_NAMESPACE_OPENTURNS
